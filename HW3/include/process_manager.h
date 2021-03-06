#include <map>
#include <list>

#include <command.h>

#define PipeIn 0
#define PipeOut 1

namespace Childish {
	struct Process {
		Command cmd;
		int fd[2];
		pid_t pid;
	};

	using ProcessList = std::list<Process>;
	class ProcessManager {
		public:
			ProcessManager();
			~ProcessManager();
			void execute_commands(const Command::Chain &);

		private:
			void wait_proc(const pid_t);

		private:
			std::map<pid_t, ProcessList> __process_groups;
			static pid_t fg_pgid;
	};
}; // namespace Childish
