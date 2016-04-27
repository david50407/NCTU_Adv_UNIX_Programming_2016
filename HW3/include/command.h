#include <vector>
#include <string>

class Command {
	public:
		using Cmdlist = std::vector<std::string>;

	public:
		static bool execute(const Cmdlist);
	private:
		static bool cmd_exit(const Cmdlist &); 
		static bool cmd_execute(const Cmdlist &);
};
