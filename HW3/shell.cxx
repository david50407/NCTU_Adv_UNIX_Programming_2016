#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#include <shell.h>
#include <command.h>
#include <signal_handler.hpp>

using Childish::Shell;
using Childish::Command;

Shell::Shell() :
	pm() {

	auto quit_handler = [=] (const int signal) {
		std::cin.clear();
		std::cout << std::endl;
		show_prompt();
		std::cout.flush();
	};
	SignalHandler::subscribe(SIGINT, quit_handler);
	SignalHandler::subscribe(SIGQUIT, quit_handler);
}

void Shell::run() {
	while (true) {
		show_prompt();
		const auto cmds = Command::parse_commands(read_command());
		if (cmds.size() == 0)
			continue;
		if (cmds.front().get_args()[0] == "$__error") {
			std::cout << "\033[1;31m" << cmds.front().get_args()[1] << "\033[m" << std::endl;
			continue;
		}
		if (cmds.front().get_args()[0] == "exit")
			::exit(0);
		pm.execute_commands(cmds);
	}
}

void Shell::show_prompt() {
	char pwd[PATH_MAX];
	getcwd(pwd, sizeof(pwd));
	printf("%s $ ", pwd);
}

std::string Shell::read_command() {	
	std::string cmd = "\\"; // back-slash and will be removed on first read.
	for (std::string line; cmd.back() == '\\'; ) { // multi-line command
		std::getline(std::cin, line);
		cmd.pop_back(); // remove tail back-slash
		cmd.append(line);
	}
	return cmd;
}
