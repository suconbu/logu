#include "logu/logu.hpp"

#include <string>

class example_formatter : public logu::formatter_base {
    std::string format(const logu::record& record) override
    {
        return "[example_formatter] " + record.message();
    }
};

int main()
{
    // Basic usage

    LOGU_DEBUG << "debug message";
    LOGU_INFO << "info message";
    LOGU_WARN << "warning message";
    LOGU_ERROR << "error message";
    LOGU << "message with no severity specified";

    // Customize default logger

    LOGU_GET_DEFAULT()
        .set_severity(logu::severity::warn)
        .set_formatter(logu::formatter()
            .set_option(logu::formatter::option::datetime_year, false)
            .set_option(logu::formatter::option::datetime_microsecond, true)
            .set_option(logu::formatter::option::func, false)
            .set_option(logu::formatter::option::threadid, false));

    LOGU_DEBUG << "debug message";
    LOGU_INFO << "info message";
    LOGU_WARN << "warn message";
    LOGU_ERROR << "error message";
    LOGU << "message with no severity specified";

    // Use named logger

    LOGU_GET("example")
        .set_handler(std::cout, "example.log")
        .set_formatter(example_formatter());

    int n = 1234567890;
    double f = 3.141592653589793;
    const char* str = "hello";

    LOGU_("example").format("n:%d f:%.3f str:%s", n, f, str);

    LOGU_INFO_("example") << LOGU_VARSTR(n, f, str);

    return 0;
}
