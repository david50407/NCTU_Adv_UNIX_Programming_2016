#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <curl/curl.h>

#if defined(LOG_FILE)
	#define RED ""
	#define CLEAR ""
	#define LOGGER logger
	#define INIT_LOGGER() { LOGGER = orig_fopen("hijack.log", "a+"); }
	#define FREE_LOGGER() { orig_fclose(LOGGER); LOGGER = NULL; }

FILE *logger = NULL;
#else
	#define RED "\033[1;31m"
	#define CLEAR "\033[m"
	#define LOGGER stderr
	#define INIT_LOGGER() {}
	#define FREE_LOGGER() {}
#endif // defined(LOG_FILE)

#define INFO(format, ...) orig_fprintf(LOGGER, RED "*** " format " ***" CLEAR "\n", __VA_ARGS__)
#define DATA(name, format, ...) orig_fprintf(LOGGER, RED "*** " name " BEGIN ***" CLEAR "\n" format "\n" RED "*** " name "  END  ***" CLEAR "\n", __VA_ARGS__)

#define mapping_func(ret, name, args) \
	typedef ret ( * decl_##name ) args; \
	static decl_##name orig_##name;
mapping_func(int, __libc_start_main, (int (*)(int, char **, char **), int, char **, void (*)(void), void (*)(void), void (*)(void), void (*)));
mapping_func(FILE *, fopen, (const char *, const char *));
mapping_func(int, fprintf, (FILE *, const char*, ...));
mapping_func(int, fputs, (const char *, FILE *));
mapping_func(int, fputc, (int, FILE *));
mapping_func(char *, fgets, (char *, int, FILE *));
mapping_func(int, fileno, (FILE *));
mapping_func(int, fflush, (FILE *));
mapping_func(int, fseek, (FILE *, long int, int));
mapping_func(size_t, fread, (void *, size_t, size_t, FILE *));
mapping_func(size_t, fwrite, (const void *, size_t, size_t, FILE *));
mapping_func(int, fclose, (FILE *));
mapping_func(int, open, (const char *, int));
mapping_func(ssize_t, read, (int, void *, size_t));
mapping_func(ssize_t, write, (int, const void *, size_t));
mapping_func(off_t, lseek, (int, off_t, int));
mapping_func(int, ftruncate, (int, off_t));
mapping_func(int, fsetxattr, (int, const char *, const void *, size_t, int));
mapping_func(int, close, (int));
mapping_func(int, pipe, (int[2]));
mapping_func(int, mkdir, (const char *, mode_t));
mapping_func(CURL *, curl_easy_init, ());
mapping_func(void, curl_easy_cleanup, (CURL *));
mapping_func(int, getaddrinfo, (const char *, const char *, const struct addrinfo *, struct addrinfo **));
mapping_func(int, socket, (int, int, int));
mapping_func(int, connect, (int, const struct sockaddr *, socklen_t));
mapping_func(const char*, inet_ntop, (int, const void *, char *, socklen_t));
mapping_func(ssize_t, recv, (int, void *, size_t, int));
mapping_func(int, SSL_read, (void *, void *, int));
mapping_func(int, SSL_write, (void *, const void *, int));
#undef mapping_func

__attribute__((constructor)) static void initialize() {
#define load_original_func(x) orig_##x = (decl_##x)dlsym(RTLD_NEXT, #x)
	load_original_func(__libc_start_main);
	load_original_func(fopen);
	load_original_func(fprintf);
	load_original_func(fputs);
	load_original_func(fputc);
	load_original_func(fgets);
	load_original_func(fileno);
	load_original_func(fflush);
	load_original_func(fseek);
	load_original_func(fread);
	load_original_func(fwrite);
	load_original_func(fclose);
	load_original_func(open);
	load_original_func(read);
	load_original_func(write);
	load_original_func(lseek);
	load_original_func(ftruncate);
	load_original_func(fsetxattr);
	load_original_func(close);
	load_original_func(pipe);
	load_original_func(mkdir);
	load_original_func(curl_easy_init);
	load_original_func(curl_easy_cleanup);
	load_original_func(getaddrinfo);
	load_original_func(socket);
	load_original_func(connect);
	load_original_func(inet_ntop);
	load_original_func(recv);
	load_original_func(SSL_read);
	load_original_func(SSL_write);

	INIT_LOGGER();
#undef load_original_func
}

__attribute__((destructor)) static void finalize() {
	FREE_LOGGER();
}

int __libc_start_main(int (*main)(int, char **, char **), int argc, char ** ubp_av, \
	void (*init)(void), void (*fini)(void), void (*rtld_fini)(void), void (*stack_end)) {
	INFO("__libc_start_main(%p, %d, %p, %p, %p, %p, %p)", main, argc, ubp_av, init, fini, rtld_fini, stack_end);
	return orig___libc_start_main(main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}

FILE * fopen(const char * filename, const char * mode) {
	FILE * ret = orig_fopen(filename, mode);
	INFO("fopen(%s, %s) = %lld", filename, mode, ret);
	return ret;
}

int fputs(const char * str, FILE * stream) {
	INFO("fputs(%s, %lld)", str, stream);
	return orig_fputs(str, stream);
}

int fputc(int character, FILE * stream) {
	INFO("fputc(%d, %lld)", character, stream);
	return orig_fputc(character, stream);
}

char * fgets(char * str, int num, FILE * stream) {
	INFO("fgets(%s, %d, %lld)", str, num, stream);
	return orig_fgets(str, num, stream);
}

int fileno(FILE *stream) {
	INFO("fileno(%lld)", stream);
	return orig_fileno(stream);
}

int fflush(FILE *stream) {
	INFO("fflush(%lld)", stream);
	return orig_fflush(stream);
}

int fseek(FILE *stream, long int offset, int origin) {
	INFO("fseek(%lld, %lld, %d)", stream, offset, origin);
	return orig_fseek(stream, offset, origin);
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream) {
	size_t ret = orig_fread(ptr, size, count, stream);
	INFO("fread(%p, %d, %d, %lld) = %lld", ptr, size, count, stream, ret);
	DATA("fread", "%*s", count * size, ptr);
	return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream) {
	INFO("fwrite(%p, %d, %d, %lld)", ptr, size, count, stream);
	DATA("fwrite", "%*s", count * size, ptr);
	return orig_fwrite(ptr, size, count, stream);
}

int fclose(FILE * stream) {
	INFO("fclose(%lld)", stream);
	return orig_fclose(stream);
}

int open(const char *pathname, int flags) {
	int ret = orig_open(pathname, flags);
	INFO("open(%s, %d) = %d", pathname, flags, ret);
	return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
	ssize_t ret = orig_write(fd, buf, count);
	INFO("write(%d, %p, %lld", fd, buf, count);
	DATA("write", "%*s", count, buf);
	return ret;
}

ssize_t read(int fd, void *buf, size_t count) {
	ssize_t ret = orig_read(fd, buf, count);
	INFO("read(%d, %p, %lld", fd, buf, count);
	DATA("read", "%*s", count, buf);
	return ret;
}

off_t lseek(int fd, off_t offset, int whence) {
	INFO("lseek(%d, %d, %d)", fd, offset, whence);
	return orig_lseek(fd, offset, whence);
}

int ftruncate(int fd, off_t length) {
	INFO("ftruncate(%d, %d)", fd, length);
	return orig_ftruncate(fd, length);
}

int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags) {
	INFO("fsetxattr(%d, %s, %p, %d, %d)", fd, name, value, size, flags);
	return orig_fsetxattr(fd, name, value, size, flags);
}

int close(int fd) {
	INFO("close(%d)", fd);
	return orig_close(fd);
}

int pipe(int pipefd[2]) {
	INFO("pipe({%d, %d})", pipefd[0], pipefd[1]);
	return orig_pipe(pipefd);
}

int mkdir(const char *pathname, mode_t mode) {
	INFO("mkdir(%s, %d)", pathname, mode);
	return orig_mkdir(pathname, mode);
}

CURL *curl_easy_init() {
	CURL *ret = orig_curl_easy_init();
	INFO("curl_easy_init() = %p", ret);
	return ret;
}

void curl_easy_cleanup(CURL *handle) {
	INFO("curl_easy_cleanup(%p)", handle);
	orig_curl_easy_cleanup(handle);
}

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) {
	INFO("getaddrinfo(%s, %s, %p, %p)", node, service, hints, res);
	INFO("Host(%s)", node);
	return orig_getaddrinfo(node, service, hints, res);
}

int socket(int domain, int type, int protocol) {
	INFO("socket(%d, %d, %d)", domain, type, protocol);
	return orig_socket(domain, type, protocol);
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len) {
	INFO("connect(%d, %p, %d)", socket, address, address_len);
	return orig_connect(socket, address, address_len);
}

const char* inet_ntop(int af, const void *src, char *dst, socklen_t size) {
	const char *ret = orig_inet_ntop(af, src, dst, size);
	INFO("inet_ntop(%d, %p, %p, %d) = %s", af, src, dst, size, ret);
	INFO("IP(%s)", ret);
	return ret;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
	ssize_t ret = orig_recv(sockfd, buf, len, flags);
	INFO("recv(%d, %p, %d, %d) = %lld", sockfd, buf, len, flags, ret);
	DATA("recv", "%*s", len, buf);
	return ret;
}

int SSL_read(void *ssl, void *buf, int num) {
	int ret = orig_SSL_read(ssl, buf, num);
	INFO("SSL_read(%p, %p, %d) = %d", ssl, buf, num, ret);
	DATA("SSL_read", "%*s", num, buf);
	return ret;
}

int SSL_write(void *ssl, const void *buf, int num) {
	int ret = orig_SSL_write(ssl, buf, num);
	INFO("SSL_write(%p, %p, %d) = %d", ssl, buf, num, ret);
	DATA("SSL_write", "%*s", num, buf);
	return ret;
}
