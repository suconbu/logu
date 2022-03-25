#define CLOG_DISABLE_TAG_PATTERNS \
    {                             \
        ".*ternal$", "quick"      \
    }
#define CLOG_DISABLE_TAG_PATTERNS_EXCLUDE \
    {                                     \
        "^ex.*"                           \
    }
#include "clog/clog.hpp"

// tagは廃止した。これは削除する。

namespace example {

namespace tags {
    constexpr const char* external = "external";
    constexpr const char* internal = "internal";
    constexpr const char* database = "database";
    constexpr const char* exchange = "exchange";
    constexpr const char* longtag = "The quick brown fox jumps over the lazy dog";
}

void tagfilter_example()
{
    std::cout << "========== " << __func__ << " ==========" << std::endl;

    CLOG_DEBUG_("example") << "debug debug debug";
    CLOG_INFO_("example") << "info info info";
    CLOG_WARN_("example") << "warning warning warning";
    CLOG_ERROR_("example") << "error error error";

    // No matched CLOG_OPTION_DENY_TAGS
    // -> Output
    CLOG_(tags::database) << "database database database";

    // No matched CLOG_OPTION_DENY_TAGS
    // -> Output
    CLOG_(tags::exchange) << "exchange exchange exchange";

    // Matched ".*ternal$" in CLOG_OPTION_DENY_TAGS
    // -> Don't output
    CLOG_(tags::internal) << "internal internal internal";

    // Matched ".*ternal$" in CLOG_OPTION_DENY_TAGS,
    // but also matched "^ex.*" in CLOG_OPTION_DENY_EXCLUDE_TAGS
    // -> Output
    CLOG_(tags::external) << "external external external";

    // Filtering by tag name is done at compile time.
    CLOG << "begin loop";
    for (size_t i = 0; i < 1000000; i++) {
        // Matched "quick" in CLOG_OPTION_DENY_TAGS
        // -> Don't output
        CLOG_(tags::longtag) << "...";
    }
    CLOG << "end loop";
}

} // namespace example
