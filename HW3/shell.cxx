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

Shell::Shell() {
}

void Shell::run() {
	while (true) {
		show_prompt();
		const auto cmds = Command::parse_commands(read_command());
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
