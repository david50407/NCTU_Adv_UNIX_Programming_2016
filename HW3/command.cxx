#include <glob.h>
#include <string>
#include <iostream>
#include <initializer_list>

#include <command.h>
#include <util.h>

#ifndef GLOB_TILDE
	#define GLOB_TILDE 0
#endif
#ifndef GLOB_BRACE
	#define GLOB_BRACE 0
#endif

namespace Childish {

	Command::Command() :
		Command({}) {
	}
	
	Command::Command(const std::initializer_list<std::string> args) :
		args(args), redirect_in(""), redirect_out("") {
	}

	Command::~Command() {
	}

	Command::Chain Command::parse_commands(const std::string &str) {
		Command::Chain chain = { Command() };
		std::vector<std::string> args = { "" };
		bool quote_flag = false;

		// parse args
		for (auto it = str.begin(); it != str.end(); ++it) {
			auto &now = args.back();
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
					if (quote_flag)
						now += ' ';
					else if (now.size() > 0)
						args.emplace_back(std::string());
					break;
				case '>':
				case '<':
				case '|':
					if (now.size() > 0)
						args.emplace_back(std::string());
					args.back() += *it;
					args.emplace_back(std::string());
					break;
				default:
					now += *it;
			}
		}

		// parse commands
		for (auto it = args.begin(); it != args.end(); ++it) {
			auto &now = chain.back();
			if (it->size() == 1 && (*it)[0] == '<') {
				if (now.args.size() == 0)
					goto syntax_error;
				if (++it == args.end())
					goto syntax_error;
				if (now.redirect_in.size() > 0 )
					goto syntax_error;
				now.redirect_in = std::string(*it);
				continue;
			}
			if (it->size() == 1 && (*it)[0] == '>') {
				if (now.args.size() == 0)
					goto syntax_error;
				if (++it == args.end())
					goto syntax_error;
				if (now.redirect_out.size() > 0 )
					goto syntax_error;
				now.redirect_out = std::string(*it);
				continue;
			}
			if (it->size() == 1 && (*it)[0] == '|') {
				if (now.args.size() == 0)
					goto syntax_error;
				if (++it == args.end())
					goto syntax_error;
				chain.emplace_back(Command({*it}));
				continue;
			}
			if (now.redirect_in.size() > 0 || now.redirect_out.size() > 0)
				goto syntax_error;

			// expand * ?
			::glob_t glob_result;
			if ((*it)[0] == '"' || ::glob(it->c_str(), GLOB_TILDE | GLOB_BRACE, NULL, &glob_result) != 0) { // not match
				now.args.push_back(*it);
			} else {
				for (int i = 0; i < glob_result.gl_pathc; ++i)
					now.args.emplace_back(std::string(glob_result.gl_pathv[i]));
			}
			::globfree(&glob_result);
		}

		return chain;

syntax_error: ;
		return {
			Command({"$__error", "Syntax error."})
		};
	}
}; // namespace Childish
