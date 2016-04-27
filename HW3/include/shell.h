#include <string>

#define PATH_MAX 4096

class Shell {
	public:
		Shell();
		void run();

	private:
		void show_prompt();
		std::string read_command();
		bool parse_command(std::string);
};