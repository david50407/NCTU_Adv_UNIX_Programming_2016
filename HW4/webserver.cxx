#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "helper.hpp"

#define DBG std::cerr

int startListen(const int);
void parseRequest(const int);
std::vector<std::string> listDirectory(const std::string &);
void sendFile(const int, const std::string &);

inline void sendResponse(const int sockfd, const std::string header, const std::string body) {
	write(sockfd, "HTTP/1.1 ", 9);
	write(sockfd, header.c_str(), header.length());
	write(sockfd, "\n\n", 2);
	write(sockfd, body.c_str(), body.length());
}

inline void fail(const char * msg) {
	std::cerr << "Failed: " << msg << std::endl;
	exit(-1);
}

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage " << argv[0] << " port webroot_path" << std::endl;
		exit(-1);
	}

	// Change dir to the webroot
	if (chdir(argv[2]) != 0) {
		std::cerr << "Failed to chdir to: " << argv[2] << std::endl;
		exit(-1);
	}

	int port;
	std::sscanf(argv[1], "%d", &port);

	int listenfd = startListen(port);
	if (listenfd == -1) {
		std::cerr << "Failed to listen 0.0.0.0:" << port << std::endl;
		exit(-1);
	}

	// Main loop
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	for (;;) {
		int sockfd = accept(listenfd, (struct sockaddr *) &clientaddr, &addrlen);
		if (fork() == 0) { // Child process
			parseRequest(sockfd);
			close(sockfd);
			exit(0);
		} else { // Main process
			close(sockfd);
		}
	}

	return 0;
}

int startListen(const int port) {
	int fd;
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int val = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(val));

	struct linger ld;
	ld.l_onoff = ld.l_linger = 0;
	setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld));

	sin.sin_port = htons(port);
	if ((bind(fd, (struct sockaddr *) &sin, sizeof(sin)) < 0) || (listen(fd, 500) < 0))
		return -1;

	return fd;
}

void parseRequest(const int sockfd) {
	std::stringstream httpRequest;
	char buffer[25565];
	
	// Receiving http request
	for (int rcvd;;) {
		rcvd = recv(sockfd, buffer, sizeof(buffer), 0);
		if (rcvd < 0)
			fail("recv() error");
		buffer[rcvd] = '\0';
		httpRequest << buffer;
		if (rcvd != sizeof(buffer)) // No more
			break;
	}

	DBG << httpRequest.str() << std::endl;

	// Parse http verb
	std::string httpVerb;
	httpRequest >> httpVerb;
	if (httpVerb != "GET" && httpVerb != "POST") {
		sendResponse(sockfd, "405 Method Not Allowed", "");
		return;
	}

	// Parse request URI
	std::string requestURI, requestPath, requestQuery, realPath;
	httpRequest >> requestURI;
	int requestQueryPos = requestURI.find_first_of("?");
	if (requestQueryPos == std::string::npos) { // Not found
		requestPath = std::string(requestURI);
		requestQuery = "";
	} else {
		// Spec: If a requested URL contains a question mark (?), you can simply ignore the question mark and all texts after the mark.
		requestPath = requestURI.substr(0, requestQueryPos);
		requestQuery = requestURI.substr(requestQueryPos);
	}
	requestPath = safePath(requestPath);
	if (requestPath == "/")
		realPath = "."; // Webroot
	else
		realPath = requestPath.substr(1);

	// Parse http version
	std::string httpVersion;
	httpRequest >> httpVersion;
	if (httpVersion != "HTTP/1.1" && httpVersion != "HTTP/1.0") {
		sendResponse(sockfd, "400 Bad Request", "");
		return;
	}

	if (access(realPath.c_str(), F_OK) != 0) { // Path not exists
		// Spec: If a requested object does not exist, your server has to return a "403 FORBIDDEN" status and provide error messages.
		// Spec: Note that we INTENTIONALLY use 403 error code for missing objects and 404 error codes for inaccessible objects.
		sendResponse(sockfd, "403 Forbidden", "");
		return;
	}

	struct stat pathStat;
	stat(realPath.c_str(), &pathStat);
	if (S_ISDIR(pathStat.st_mode)) { // Is a directory
		// Spec: If a requested object is an accessible directory, please follow the descriptions in the next section.
		if (requestPath.back() != '/') {
			// Spec: If a requested object is a directory, your program have to check whether the requested directory has a slash (/) at the end of the URL.
			// Spec: If the URL does not have a slash suffix, you have to respond a "301 MOVE PERMANENTLY" status and a new location
			//       with a slash appended to the original requested URL.
			std::stringstream ss;
			ss << "301 Moved Permanently\n"
				"Location: " << requestPath << "/" << requestQuery;
			sendResponse(sockfd, ss.str(), "");
			return;
		}

		// Spec: If a slash is already given in the URL, your server has to search for a default index file,
		//       e.g., index.html, in the directory.
		std::string defaultPage = realPath;
		defaultPage.append("index.html");
		if (access(defaultPage.c_str(), F_OK) == 0) { // Default page exists
			if (access(defaultPage.c_str(), R_OK) != 0) { // Not readable
				// Spec: If there is a index.html file, but it is not readable, you can simply send "403 FORBIDDEN" status to the browser.
				sendResponse(sockfd, "403 Forbidden", "");
				return;
			}
			// Spec: If there is a readable index.html file, you can simply send the content of index.html back to the browser.
			stat(defaultPage.c_str(), &pathStat);
			std::stringstream ss;
			ss << "200 OK\n"
				"Content-Type: text/html\n"
				"Content-Length: " << pathStat.st_size;
			sendResponse(sockfd, ss.str(), "");
			sendFile(sockfd, defaultPage);
			return;
		}

		try {
			// Spec: If there is not a readable index.html file, but the directory is readable, you have to list the files and
			//       directories found in the requested directory. The list can be in either a plain-text document or a html document.
			// Spec: It would be a plus if your response is a html document with hyperlinks point to the location of files and directories in the requested directory.
			auto list = listDirectory(realPath);
			std::stringstream htmlStream;
			htmlStream << "<h1>List of: " << requestPath << "</h1>\n"
				"<hr />\n"
				"<ul>\n";
			for (auto item : list) {
				// TODO: URI escape & HTML escape
				htmlStream << R"(    <li><a href=")" << item << R"(">)" << item << "</a></li>\n";
			}
			htmlStream << "</ul>\n";
			std::string html = htmlStream.str();

			std::stringstream header;
			header << "200 OK\n"
				"Content-Type: text/html\n"
				"Content-Length: " << html.size();
			sendResponse(sockfd, header.str(), html);
		} catch (...) {
			sendResponse(sockfd, "404 Not Found", "");
		}

		return;
	} /* else if (requestPath.substr(0, 9) == "/cgi-bin/") { // CGI-mode
		// TODO: run cgi
		return;
	} */

	if (access(realPath.c_str(), R_OK) != 0) {
		// Spec: If a requested object is inaccessible, your server has to return a "404 NOT FOUND" status and provide error messages.
		// Spec: Note that we INTENTIONALLY use 403 error code for missing objects and 404 error codes for inaccessible objects.
		sendResponse(sockfd, "404 Not Found", "");
		return;
	}

	// Spec: If a requested object can be found, your server has to return "200 OK" status as well as the proper headers and
	//       content for the browser to render the document correctly.
	std::stringstream ss;
	ss << "200 OK\n"
		"Content-Type: " << guessMimeTypeByFilename(realPath) << "\n"
		"Content-Length: " << pathStat.st_size;
	sendResponse(sockfd, ss.str(), "");
	sendFile(sockfd, realPath);
	return;
}

std::vector<std::string> listDirectory(const std::string &path) {
	struct dirent *readDir;
	DIR *dir = opendir(path.c_str());

	if (dir == NULL) // Failed to open the dir
		throw std::exception();

	std::vector<std::string> result;
	while ((readDir = readdir(dir)) != NULL) {
		if (readDir->d_name[0] == '.' &&
				readDir->d_name[1] == '\0') // `.`
			continue;
		if (readDir->d_name[0] == '.' &&
				readDir->d_name[1] == '.' &&
				readDir->d_name[2] == '\0' && path == ".") // `..` and webroot
			continue;
		result.push_back(readDir->d_name);
	}
	closedir(dir);

	return result;
}

void sendFile(const int sockfd, const std::string &path) {
	FILE *fp = fopen(path.c_str(), "rb");
	int status;
	char buffer[25565];

	while ((status = fread(buffer, 1, sizeof(buffer), fp)) == sizeof(buffer))
		write(sockfd, buffer, sizeof(buffer));
	write(sockfd, buffer, status);

	fclose(fp);
}
