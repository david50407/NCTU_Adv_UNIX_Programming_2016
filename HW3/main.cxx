#include <cstdio>
#include <shell.h>
#include <signal_handler.hpp>

using Childish::Shell;
using Childish::SignalHandler;

int main(int argc, char **argv, char **envp) {
	Shell sh;
	SignalHandler::init();
	sh.run();

	return 0;
}
