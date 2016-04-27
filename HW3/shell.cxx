#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <shell.h>
#include <command.h>

Shell::Shell() {
}

void Shell::run() {
	while (true) {
		show_prompt();
		const std::string cmd = read_command();
		if (!parse_command(cmd))
			break;
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

bool Shell::parse_command(const std::string cmd) {
	Command::Cmdlist cmds;
	bool quote_flag = false;
	cmds.push_back(std::string());
	for (auto it = cmd.begin(); it < cmd.end(); ++it) {
		auto &now = cmds.back();
		switch (*it) {
			case '\\': // handle escape characters
				++it;
				switch (*it) {
					case 'n': 
						now.append("\n");
						break;
					case '\\':
						now.append("\\");
						break;
					case '/':
						now.append("\\/");
						break;
					default:
						now += *it;
				}
				break;
			case '"':
				quote_flag ^= true;
				break;
			case ' ': // split command arguments
				if (!quote_flag)
					cmds.push_back(std::string());
				else
					now += ' ';
				break;
			default:
				now += *it;
		}
	}

	if (Command::execute(cmds))
		return true;
	printf("Input: \n%s\n", cmd.c_str());
	return true;
}
