#include <string>

#include <command.h>
#include <process_manager.h>

namespace Childish {
	const int $PATH_MAX = 4096;

	class Shell {
		public:
			Shell();
			void run();

		private:
			void show_prompt();
			std::string read_command();
			bool builtin_command(const Command::Chain &);
			bool builtin_command_exit(const Command::Chain &);
			bool builtin_command_cd(const Command::Chain &);
			bool builtin_command_export(const Command::Chain &);
			bool builtin_command_unset(const Command::Chain &);

		public:
			ProcessManager pm;
	};
}; // namespace Childish
