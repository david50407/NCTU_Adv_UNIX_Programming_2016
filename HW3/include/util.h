#include <vector>
#include <string>

namespace Childish {
	class Util {
		public:
			static int execvp(const std::string, const std::vector<std::string>);
	};
}; // namespace Childish
