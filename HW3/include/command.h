#if !defined(__COMMAND_H__)
#define __COMMAND_H__

#include <list>
#include <vector>
#include <string>
#include <initializer_list>

namespace Childish {
	class Command {
		public:
			using Cmdlist = std::vector<std::string>;
			using Chain = std::vector<class Command>;

		private:
			std::vector<std::string> args;
			std::string redirect_in;
			std::string redirect_out;

		public:
			~Command();
			std::vector<std::string> get_args() { return args; }
			std::string get_redirect_in() { return redirect_in; }
			std::string get_redirect_out() { return redirect_out; }
			static Chain parse_commands(const std::string &);
		private:
			Command();
			Command(const std::initializer_list<std::string>);
	};
}; // namespace Childish

#endif // !defined(__COMMAND_H__)
