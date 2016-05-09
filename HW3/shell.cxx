#include <unistd.h>
#include <pwd.h>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <iostream>
#include <sstream>
#include <string>

#include <color.hpp>
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
		if (!builtin_command(cmds))
			pm.execute_commands(cmds);
	}
}

void Shell::show_prompt() {
	char pwd[PATH_MAX];
	getcwd(pwd, sizeof(pwd));
	std::cout << Color::Yellow << pwd << Color::Bright << " $ " << Color::Reset;
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

bool Shell::builtin_command(const Command::Chain &chain) {
	return builtin_command_$__error(chain)
		|| builtin_command_exit(chain)
		|| builtin_command_cd(chain)
		|| builtin_command_export(chain)
		|| builtin_command_unset(chain)
		;
}

bool Shell::builtin_command_$__error(const Command::Chain &chain) {
	if (chain.front().get_args()[0] != "$__error")
		return false;

	const auto &args = chain.front().get_args();
	std::cerr << Color::Bright << Color::Red << "Error: " << args[1] << std::endl;
}

bool Shell::builtin_command_exit(const Command::Chain &chain) {
	if (chain.front().get_args()[0] == "exit")
		exit(0);
	return false;
}

bool Shell::builtin_command_cd(const Command::Chain &chain) {
	if (chain.front().get_args()[0] != "cd")
		return false;

	const auto &args = chain.front().get_args();
	char path[PATH_MAX], now_path[$PATH_MAX];
	std::string target = args[1];

	if (args.size() == 1) { // home
		char *home;
		if ((home = getenv("HOME")) == NULL)
			home = ::getpwuid(::getuid())->pw_dir;
		target = std::string(home);
	} else if (args[1] == "-") { // last location
		char *old_pwd;
		if ((old_pwd = ::getenv("OLDPWD")) == NULL) // no old pwd
			return true;
		target = std::string(old_pwd);
	}

	::realpath(target.c_str(), path);
	if (::getcwd(now_path, PATH_MAX) == NULL) {
		std::cerr << Color::Bright << Color::Yellow << "Warning: cd: Cannot get current working directory." << Color::Reset << std::endl;
		now_path[0] = '\0';
	}

	if (::chdir(path)) {
		std::cerr << Color::Bright << Color::Red << "cd: No such directory: " << path << Color::Reset << std::endl;
		return true;
	}

	if (now_path[0] != '\0')
		::setenv("OLDPWD", now_path, 1);
	::setenv("PWD", path, 1);

	return true;
}

bool Shell::builtin_command_export(const Command::Chain &chain) {
	if (chain.front().get_args()[0] != "export")
		return false;

	const auto &args = chain.front().get_args();
	if (args.size() == 1) {
		std::cerr << Color::Bright << Color::Red << "Usage: export $name [$value]" << Color::Reset << std::endl;
		return true;
	}

	auto name = std::string(args[1]);
	if (name[0] == '"')
		name.erase(name.size() - 1, 1).erase(0, 1);
	
	if (args.size() > 2) { // setent
		auto value = std::string(args[2]);
		if (value[0] == '"')
			value.erase(value.size() - 1, 1).erase(0, 1);
		::setenv(name.c_str(), value.c_str(), 1);
	} else { // getenv
		char *env;
		if ((env = ::getenv(name.c_str())) == NULL) // not exist
			std::cout << name << "=" << std::endl;
		else
			std::cout << name << "=" << env << std::endl;
	}

	return true;
}

bool Shell::builtin_command_unset(const Command::Chain &chain) {
	if (chain.front().get_args()[0] != "unset")
		return false;

	const auto &args = chain.front().get_args();
	if (args.size() == 1) {
		std::cerr << Color::Bright << Color::Red << "Usage: unset $name" << Color::Reset << std::endl;
		return true;
	}

	auto name = std::string(args[1]);
	if (name[0] == '"')
		name.erase(name.size() - 1, 1).erase(0, 1);
	::unsetenv(name.c_str());

	return true;
}
