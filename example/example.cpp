#include "clog/clog.hpp"

#include <string>

class example_formatter : public clog::formatter_base {
    std::string format(const clog::record& record) override
    {
        return "[example_formatter] " + record.message();
    }
};

int main()
{
    // Basic usage

    CLOG_DEBUG << "debug message";
    CLOG_INFO << "info message";
    CLOG_WARN << "warning message";
    CLOG_ERROR << "error message";
    CLOG << "message with no severity specified";

    // Customize default logger

    CLOG_GET_DEFAULT()
        .set_severity(clog::severity::warn)
        .set_formatter(clog::formatter()
            .set_option(clog::formatter::option::datetime_year, false)
            .set_option(clog::formatter::option::datetime_microsecond, true)
            .set_option(clog::formatter::option::func, false)
            .set_option(clog::formatter::option::threadid, false));

    CLOG_DEBUG << "debug message";
    CLOG_INFO << "info message";
    CLOG_WARN << "warn message";
    CLOG_ERROR << "error message";
    CLOG << "message with no severity specified";

    // Use named logger

    CLOG_GET("example")
        .set_handler(std::cout, "example.log")
        .set_formatter(example_formatter());

    int n = 1234567890;
    double f = 3.141592653589793;
    const char* str = "hello";

    CLOG_("example").format("n:%d f:%.3f str:%s", n, f, str);

    CLOG_INFO_("example") << CLOG_VARSTR(n, f, str);

    return 0;
}
