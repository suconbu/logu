#include "clog/clog.hpp"

#include <string>

namespace example {
void severities_example();
void datatypes_example();
void severityfilter_example();
void tagfilter_example();
void disable_example();
void format_example();
void argument_example();
void fileoutput_example();
void outputhandler_example();
void instance_example();
}

int main()
{
    example::severities_example();
    example::datatypes_example();
    example::severityfilter_example();
    example::tagfilter_example();
    example::disable_example();
    example::format_example();
    example::argument_example();
    example::fileoutput_example();
    example::outputhandler_example();
    example::instance_example();

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
    int* ptr = nullptr;
    CLOG << ptr;
}

void format_example()
{
    std::cout << "========== " << __func__ << " ==========" << std::endl;

    CLOG.format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "none none none");
    CLOG_DEBUG.format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "debug debug debug");
    CLOG_INFO.format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "info info info");
    CLOG_WARN.format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "warn warn warn");
    CLOG_ERROR.format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "error error error");

    CLOG_("example").format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "none none none");
    CLOG_DEBUG_("example").format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "debug debug debug");
    CLOG_INFO_("example").format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "info info info");
    CLOG_WARN_("example").format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "warn warn warn");
    CLOG_ERROR_("example").format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "error error error");
}

void argument_example()
{
    std::cout << "========== " << __func__ << " ==========" << std::endl;

    int n = 1;
    unsigned int u = 0xFFFFu;
    double f = 3.141592653589793;
    const char* cc = "const char*";
    std::string ss = "std::string";
    void* ptr = nullptr;
    CLOG << CLOG_VARSTR(n, u, f, cc, ss, "literal", ptr);
}

void example_handler_1(const char* str)
{
    std::cout << "<<< example_handler_1 >>> " << str << std::endl;
    // std::cout << "<<< example_handler_1 >>> " << record.message() << std::endl;
}

void instance_example()
{
    std::cout << "========== " << __func__ << " ==========" << std::endl;

    CLOG_GET("name")
        .set_severity(clog::severity::info)
        .set_handler(example_handler_1, std::cout, "hogeeee.log", clog::windows_debugger);
    CLOG_DEBUG_("name") << "multi handlers";
    CLOG_ERROR_("name") << "multi handlers";
    // CLOG_GET("name") += clog::record(clog::severity::debug, nullptr, clog::internal::basename(__FILE__), __func__, 0).ref() << "multi handlers";
    // CLOG_GET("name") += clog::record(clog::severity::error, nullptr, clog::internal::basename(__FILE__), __func__, 0).ref() << "multi handlers";
    CLOG_GET("name")
        .set_handler();
    CLOG_ERROR_("name") << "handler nothing";
    // CLOG_GET("name") += clog::record(clog::severity::debug, nullptr, clog::internal::basename(__FILE__), __func__, 0).ref() << "handler nothing";
}

} // namespace example
