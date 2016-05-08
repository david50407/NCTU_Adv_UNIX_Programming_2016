#include <string>

#include <command.h>
#include <process_manager.h>

namespace Childish {
	const int PATH_MAX = 4096;

	class Shell {
		public:
			Shell();
			void run();

		private:
			void show_prompt();
			std::string read_command();

		public:
			ProcessManager pm;
	};
}; // namespace Childish
