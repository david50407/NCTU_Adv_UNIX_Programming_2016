#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> splitStr(const std::string &src, const std::string &delim) {
	std::vector<std::string> result;
	const auto delimLength = delim.size();
	size_t beginPos = 0, endPos = src.find(delim); // Locate the first delimiter in string
	while (endPos != std::string::npos) {
		result.push_back(src.substr(beginPos, endPos - beginPos)); // Extract the sub-string before current delimiter
		beginPos = endPos + delimLength;
		endPos = src.find(delim, beginPos);  // Locate the next delimiter in string
	}
	result.push_back(src.substr(beginPos, endPos - beginPos));  // Extract the last sub-string
	return result;
}

std::string joinStr(const std::vector<std::string> &src, const std::string &delim) {
	if (src.size() == 0)
		return std::string("");
	std::stringstream ss;
	ss << src[0];
	for (auto it = src.begin() + 1; it != src.end(); ++it)
		ss << delim << *it;
	return ss.str();
}

std::string safePath(const std::string &unsafe) {
	std::vector<std::string> walking;
	for (auto path : splitStr(unsafe, "/")) {
		if (path == ".") // current directory
			continue; // skip it
		if (path == "..") // parent directory
			walking.pop_back();
		else
			walking.push_back(path);
	}
	if (walking.size() == 1)
		return std::string("/");
	return joinStr(walking, "/");
}

std::string guessMimeTypeByFilename(const std::string &filename) {
	// Spec: Possible static objects include text files, html files, scripts, style sheet files, images, audio files, and video files.
	auto extPos = filename.rfind(".");
	std::string ext = extPos == std::string::npos ? "" : filename.substr(extPos);

	if (ext == ".txt")
		return "text/plain";
	if (ext == ".html" || ext == ".html")
		return "text/html";
	if (ext == ".js")
		return "text/javascript";
	if (ext == ".css")
		return "text/css";
	if (ext == ".png")
		return "image/png";
	if (ext == ".jpg" || ext == ".jpeg")
		return "image/jpeg";
	if (ext == ".gif")
		return "image/gif";
	if (ext == ".ogg")
		return "audio/ogg";
	if (ext == ".mp4")
		return "video/mp4";

	return "application/plain";
}
