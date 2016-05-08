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
			std::list<std::string> args;
			std::string redirect_in;
			std::string redirect_out;

		public:
			~Command();
			static Chain parse_commands(const std::string &);

			static bool execute(const Cmdlist);
		private:
			Command();
			Command(const std::initializer_list<std::string>);

			static bool cmd_exit(const Cmdlist &); 
			static bool cmd_execute(const Cmdlist &);
	};
}; // namespace Childish

#endif // !defined(__COMMAND_H__)
