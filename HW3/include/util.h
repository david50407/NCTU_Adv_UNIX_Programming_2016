#include <vector>
#include <string>

namespace Childish {
	class Util {
		public:
			static int execvp(const std::string, const std::vector<std::string>);
			static bool setpgid(pid_t, pid_t);
			static bool pipe(int [2]);
	};
}; // namespace Childish
