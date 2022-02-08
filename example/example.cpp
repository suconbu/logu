#include "clog/clog.hpp"

#include <string>

namespace example {
void severities_example();
void datatypes_example();
void severityfilter_example();
void tagfilter_example();
void disable_example();
}

int main()
{
    example::severities_example();
    example::datatypes_example();
    example::severityfilter_example();
    example::tagfilter_example();
    example::disable_example();

    return 0;
}

namespace example {

void severities_example()
{
    std::cout << "========== " << __func__ << " ==========" << std::endl;

    // No need to add "std::endl" to the end of line.
    CLOG_DEBUG << "debug debug debug";
    CLOG_INFO << "info info info";
    CLOG_WARN << "warning warning warning";
    CLOG_ERROR << "error error error";

    // No-severity macros are not limitted by "CLOG_MIN_SEVERITY_..."
    CLOG << "none none none";
}

void datatypes_example()
{
    std::cout << "========== " << __func__ << " ==========" << std::endl;

    CLOG << 1;
    CLOG << 0xFFFFu;
    CLOG << 3.141592653589793;
    CLOG << 6.72e10;
    CLOG << false;
    CLOG << 'c';
    CLOG << "string";
    CLOG << std::string("std::string");
    // You can also specify a string pointer that may be "nullptr".
    const char* ptr = nullptr;
    CLOG << ptr;
}

} // namespace example
