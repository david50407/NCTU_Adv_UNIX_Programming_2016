#include <vector>
#include <string>

namespace Childish {
	class Command {
		public:
			using Cmdlist = std::vector<std::string>;

		public:
			static bool execute(const Cmdlist);
		private:
			static bool cmd_exit(const Cmdlist &); 
			static bool cmd_execute(const Cmdlist &);
	};
}; // namespace Childish
