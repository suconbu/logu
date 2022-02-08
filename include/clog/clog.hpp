//
// Copyright (c) 2022 D.Miwa
// This software is released under the MIT License.
//

#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace clog {

enum severity {
    none,
    error,
    warn,
    info,
    debug
};

namespace regex_static {
    // clang-format off

    // http://cpptruths.blogspot.com/2011/07/compile-time-regex-matcher-using.html

    constexpr int match_c(const char *regexp, const char *text);
    constexpr int matchhere_c(const char *regexp, const char *text);
    constexpr int matchstar_c(int c, const char *regexp, const char *text);
    constexpr int matchend_c(const char * regexp, const char * text);

    constexpr int match_c(const char *regexp, const char *text)
    {
        return (regexp[0] == '^') ? matchhere_c(regexp+1, text) : 
                                    matchend_c(regexp, text);
    }

    /* matchhere: search for regexp at beginning of text */
    constexpr int matchhere_c(const char *regexp, const char *text)
    {
        return (regexp[0] == '\0') ? 1 : 
                (regexp[1] == '*') ? matchstar_c(regexp[0], regexp+2, text) :
                (regexp[0] == '$' && regexp[1] == '\0') ? (*text == '\0') :
                (*text!='\0' && (regexp[0]=='.' || regexp[0]==*text)) ? 
                matchhere_c(regexp+1, text+1) : 0;
    }

    constexpr int matchend_c(const char * regexp, const char * text)
    {
        return matchhere_c(regexp, text) ? 1 : 
                (*text == '\0') ? 0 : matchend_c(regexp, text+1);
    }

    /* matchstar: search for c*regexp at beginning of text */
    constexpr int matchstar_c(int c, const char * regexp, const char *text)
    {
        return matchhere_c(regexp, text) ? 1 : 
                (*text != '\0' && (*text == c || c == '.')) ? 
                matchstar_c(c, regexp, text+1) : 0;
    }

    // clang-format on
} // namespace regex_static

namespace util {
    constexpr const char* severity_to_str(clog::severity severity)
    {
        const char* s = "-----";
        if (severity == clog::severity::debug) {
            s = "debug";
        } else if (severity == clog::severity::info) {
            s = "info ";
        } else if (severity == clog::severity::warn) {
            s = "warn ";
        } else if (severity == clog::severity::error) {
            s = "error";
        } else {
        }
        return s;
    }

    constexpr char path_separator()
    {
#if defined(_WIN32)
        return '\\';
#else
        return '/';
#endif
    }

    constexpr const char* strrchr_static(const char* s, const char* p, char c)
    {
        return (p == s) ? p : (*p == c) ? (p + 1) :
                                          strrchr_static(s, p - 1, c);
    }

    constexpr size_t strlen_static(const char* s)
    {
        return *s ? 1 + strlen_static(s + 1) : 0;
    }

    constexpr const char* basename(const char* s)
    {
        return (s != nullptr) ? strrchr_static(s, s + strlen_static(s) - 1, path_separator()) : nullptr;
    }

    inline void localtime_s(struct tm* t, const time_t* time)
    {
#if defined(_WIN32) && defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
        *t = *::localtime(time);
#elif defined(_WIN32)
        ::localtime_s(t, time);
#else
        ::localtime_r(time, t);
#endif
    }

    inline void operator<<(std::ostringstream& stream, const char* data)
    {
        std::operator<<(stream, data ? data : "(null)");
    }

    constexpr bool match_list_recursive_static(const char* s, const char* const patterns[], size_t count, size_t index)
    {
        return (count <= index)                                    ? false :
            (clog::regex_static::match_c(patterns[index], s) != 0) ? true :
                                                                     match_list_recursive_static(s, patterns, count, index + 1);
    }

    constexpr bool match_list_static(const char* s, const char* const patterns[], size_t count)
    {
        return (s != nullptr) ? match_list_recursive_static(s, patterns, count, 0) : false;
    }
}

struct record {
    record(clog::severity severity_, const char* tag_, const char* file_, const char* func_, size_t line_)
        : severity(severity_)
        , tag(tag_)
        , file(file_)
        , func(func_)
        , line(line_)
        , time(std::chrono::system_clock::now())
    {
    }

    clog::record& ref() { return *this; }

    template <typename Type>
    clog::record& operator<<(const Type& data)
    {
        using namespace clog::util;
        ss << data;
        return *this;
    }

    std::string message() const
    {
        return ss.str();
    }

    const clog::severity severity;
    const char* tag;
    const char* file;
    const char* func;
    const size_t line;
    const std::chrono::system_clock::time_point time;
    std::ostringstream ss;
};

struct logger {
    logger(std::ostream& stream)
        : os(stream)
    {
    }

    void operator+=(const clog::record& record)
    {
        std::ostream& stream = os;

        // datetime
        const auto timet = std::chrono::system_clock::to_time_t(record.time);
        struct tm localt = {};
        util::localtime_s(&localt, &timet);
        char datetime_buf[20];
        const char* datetime_fmt = "%Y-%m-%d %H:%M:%S";
        std::strftime(datetime_buf, sizeof(datetime_buf), datetime_fmt, &localt);
        stream << "[" << datetime_buf;
        const auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(record.time.time_since_epoch()).count() % 1000;
        stream << "." << std::setw(3) << std::setfill('0') << msec << "] ";

        // severity
        stream << "[" << clog::util::severity_to_str(record.severity) << "] ";

        // file
        stream << "[" << record.file << "] ";

        // func/line
        stream << "[" << record.func << "@" << record.line << "] ";

        // tag
        if (record.tag != 0 && record.tag[0] != 0) {
            stream << "[" << record.tag << "] ";
        }

        // message
        stream << record.message() << std::endl;
    }

    std::ostream& os;
};

} // namespace clog

// macros

#if !defined(CLOG_OPTION_OUTPUT_STREAM)
#define CLOG_OPTION_OUTPUT_STREAM std::cout
#endif

#if defined(CLOG_OPTION_DISABLE_LOGGING)
#ifdef _MSC_VER
#define CLOG_CHECK_SEVERITY(severity_) __pragma(warning(push)) __pragma(warning(disable : 4127)) false __pragma(warning(pop))
#else //_MSC_VER
#define CLOG_CHECK_SEVERITY(severity_) false
#endif //_MSC_VER
#elif defined(CLOG_OPTION_MIN_SEVERITY_INFO)
#define CLOG_CHECK_SEVERITY(severity_) (severity_ <= clog::severity::info)
#elif defined(CLOG_OPTION_MIN_SEVERITY_WARN)
#define CLOG_CHECK_SEVERITY(severity_) (severity_ <= clog::severity::warn)
#elif defined(CLOG_OPTION_MIN_SEVERITY_ERROR)
#define CLOG_CHECK_SEVERITY(severity_) (severity_ <= clog::severity::error)
#else
#define CLOG_CHECK_SEVERITY(severity_) true
#endif

// clang-format off
#if !defined(CLOG_OPTION_DENY_TAGS)
#define CLOG_OPTION_DENY_TAGS { 0 }
#endif
#if !defined(CLOG_OPTION_DENY_EXCLUDE_TAGS)
#define CLOG_OPTION_DENY_EXCLUDE_TAGS { 0 }
#endif
// clang-format on

#ifdef _MSC_VER
#define CLOG_GET_FUNC() __FUNCTION__
#else
#define CLOG_GET_FUNC() __PRETTY_FUNCTION__
#endif

#define CLOG_GET_FILE() clog::util::basename(__FILE__)

#define CLOG_INTERNAL(severity, tag)                                                                                                                              \
    if (!CLOG_CHECK_SEVERITY(severity) || ![]() {                                                                                                                 \
            constexpr const char* allow[] = CLOG_OPTION_DENY_EXCLUDE_TAGS;                                                                                        \
            constexpr size_t allow_count = sizeof(allow) / sizeof(allow[0]);                                                                                      \
                                                                                                                                                                  \
            constexpr const char* deny[] = CLOG_OPTION_DENY_TAGS;                                                                                                 \
            constexpr size_t deny_count = sizeof(deny) / sizeof(deny[0]);                                                                                         \
                                                                                                                                                                  \
            constexpr bool allowed = (static_cast<const char*>(tag) != nullptr && allow[0] != 0) ? clog::util::match_list_static(tag, allow, allow_count) : true; \
            constexpr bool result = (!allowed && deny[0] != 0) ? !clog::util::match_list_static(tag, deny, deny_count) : true;                                    \
            return result;                                                                                                                                        \
        }()) {                                                                                                                                                    \
    } else                                                                                                                                                        \
        clog::logger(CLOG_OPTION_OUTPUT_STREAM)                                                                                                                   \
            += clog::record(severity, tag, CLOG_GET_FILE(), CLOG_GET_FUNC(), __LINE__).ref()

#define CLOG_DEBUG CLOG_INTERNAL(clog::severity::debug, nullptr)
#define CLOG_INFO  CLOG_INTERNAL(clog::severity::info, nullptr)
#define CLOG_WARN  CLOG_INTERNAL(clog::severity::warn, nullptr)
#define CLOG_ERROR CLOG_INTERNAL(clog::severity::error, nullptr)
#define CLOG       CLOG_INTERNAL(clog::severity::none, nullptr)

#define CLOG_DEBUG_(tag) CLOG_INTERNAL(clog::severity::debug, tag)
#define CLOG_INFO_(tag)  CLOG_INTERNAL(clog::severity::info, tag)
#define CLOG_WARN_(tag)  CLOG_INTERNAL(clog::severity::warn, tag)
#define CLOG_ERROR_(tag) CLOG_INTERNAL(clog::severity::error, tag)
#define CLOG_(tag)       CLOG_INTERNAL(clog::severity::none, tag)
