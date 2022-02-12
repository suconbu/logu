#define CLOG_OPTION_OUTPUT_TO gLogStream
#include "clog/clog.hpp"
#include <fstream>

constexpr auto filename = "fileoutput_example.log";
std::ofstream gLogStream(filename);

namespace example {

void fileoutput_example()
{
    std::cout << "========== " << __func__ << " ==========" << std::endl;

    CLOG_DEBUG << "debug debug debug";
    CLOG_INFO << "info info info";
    CLOG_WARN << "warning warning warning";
    CLOG_ERROR << "error error error";
    CLOG << "none none none";

    CLOG_DEBUG_("example") << "debug debug debug";
    CLOG_INFO_("example") << "info info info";
    CLOG_WARN_("example") << "warning warning warning";
    CLOG_ERROR_("example") << "error error error";
    CLOG_("example") << "none none none";

    {
        std::cout << "begin dump >>>" << std::endl;
        std::ifstream fs(filename);
        if (fs.is_open()) {
            std::string line;
            while (getline(fs, line)) {
                std::cout << line << std::endl;
            }
        }
        std::cout << "<<< end dump" << std::endl;
    }
}

} // namespace example
