#define CLOG_OPTION_DISABLE_LOGGING
#include "clog/clog.hpp"

namespace example {

void disable_example()
{
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
}

} // namespace example
