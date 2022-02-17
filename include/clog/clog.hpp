﻿//
// Copyright (c) 2022 D.Miwa
// This software is released under the MIT License.
//

#pragma once

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>

//
// basic macro
//

#define CLOG_DEBUG CLOG_OUTPUT(clog::severity::debug, nullptr)
#define CLOG_INFO  CLOG_OUTPUT(clog::severity::info, nullptr)
#define CLOG_WARN  CLOG_OUTPUT(clog::severity::warn, nullptr)
#define CLOG_ERROR CLOG_OUTPUT(clog::severity::error, nullptr)
#define CLOG       CLOG_OUTPUT(clog::severity::none, nullptr)

#define CLOG_DEBUG_(tag) CLOG_OUTPUT(clog::severity::debug, tag)
#define CLOG_INFO_(tag)  CLOG_OUTPUT(clog::severity::info, tag)
#define CLOG_WARN_(tag)  CLOG_OUTPUT(clog::severity::warn, tag)
#define CLOG_ERROR_(tag) CLOG_OUTPUT(clog::severity::error, tag)
#define CLOG_(tag)       CLOG_OUTPUT(clog::severity::none, tag)

#define CLOG_ARGS(...) clog::internal::args_to_string("" #__VA_ARGS__, ##__VA_ARGS__)

#ifdef _MSC_VER
#define CLOG_GET_FUNC() __FUNCTION__
#else
#define CLOG_GET_FUNC() __PRETTY_FUNCTION__
#endif

#define CLOG_GET_FILE() clog::internal::basename(__FILE__)

//
// option macro
//

// #define CLOG_OPTION_DISABLE_LOGGING
// #define CLOG_OPTION_MIN_SEVERITY_INFO
// #define CLOG_OPTION_MIN_SEVERITY_WARN
// #define CLOG_OPTION_MIN_SEVERITY_ERROR

#if !defined(CLOG_OPTION_OUTPUT_TO)
#define CLOG_OPTION_OUTPUT_TO std::cout
#endif

// #define CLOG_OPTION_DISABLE_OUTPUT_DATETIME
// #define CLOG_OPTION_DISABLE_OUTPUT_DATETIME_YEAR
// #define CLOG_OPTION_DISABLE_OUTPUT_SEVERITY
// #define CLOG_OPTION_DISABLE_OUTPUT_FILE
// #define CLOG_OPTION_DISABLE_OUTPUT_FUNC
// #define CLOG_OPTION_DISABLE_OUTPUT_LINE
// #define CLOG_OPTION_DISABLE_OUTPUT_TAG

// clang-format off
#if !defined(CLOG_OPTION_DENY_TAGS)
#define CLOG_OPTION_DENY_TAGS { nullptr }
#endif
#if !defined(CLOG_OPTION_DENY_EXCLUDE_TAGS)
#define CLOG_OPTION_DENY_EXCLUDE_TAGS { nullptr }
#endif
// clang-format on

// internal macro

#if defined(_MSC_VER)
#define CLOG_OUTPUT(severity, tag) __pragma(warning(push)) __pragma(warning(disable : 4127)) CLOG_OUTPUT_INTERNAL(severity, tag) __pragma(warning(pop))
#else
#define CLOG_OUTPUT(severity, tag) CLOG_OUTPUT_INTERNAL(severity, tag)
#endif

#define CLOG_OUTPUT_INTERNAL(severity, tag)                                    \
    if (![]() {                                                                \
            constexpr bool result = clog::internal::can_output(severity, tag); \
            return result;                                                     \
        }()) {                                                                 \
    } else                                                                     \
        clog::logger(CLOG_OPTION_OUTPUT_TO)                                    \
            += clog::record(severity, tag, CLOG_GET_FILE(), CLOG_GET_FUNC(), __LINE__).ref()

namespace clog {

#if defined(_MSC_VER)
extern "C" {
using LPCSTR = const char*;
void OutputDebugStringA(LPCSTR lpOutputString);
}
#endif

#if defined(__ANDROID__)
#include <android/log.h>
#endif

enum severity {
    none,
    error,
    warn,
    info,
    debug
};

namespace internal {
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

    constexpr bool match_list_recursive_static(const char* s, const char* const patterns[], size_t count, size_t index)
    {
        return (count <= index)                                              ? false :
            (clog::internal::regex_static::match_c(patterns[index], s) != 0) ? true :
                                                                               match_list_recursive_static(s, patterns, count, index + 1);
    }

    constexpr bool match_list_static(const char* s, const char* const patterns[], size_t count)
    {
        return (s != nullptr) ? match_list_recursive_static(s, patterns, count, 0) : false;
    }

    template <typename Type, size_t Size>
    constexpr size_t count_of(const Type (&)[Size])
    {
        return Size;
    }

    template <typename ValueType>
    struct output_wrapper {
        static void output(std::ostream& os, const ValueType& x)
        {
            os << x;
        }
    };

    template <typename ValueType>
    struct output_wrapper<ValueType*> {
        static void output(std::ostream& os, const ValueType* x)
        {
            if (x != nullptr) {
                os << x;
            } else {
                os << "(null)";
            }
        }
    };

    inline void output_args(std::ostream& os) { (void)os; }

    template <typename Ty>
    inline void output_args(std::ostream& os, Ty arg)
    {
        output_wrapper<Ty>::output(os, arg);
    }

    template <typename First, typename... Args>
    inline void output_args(std::ostream& os, First first, Args... args)
    {
        output_wrapper<First>::output(os, first);
        os << ", ";
        output_args(os, args...);
    }

    template <typename... Args>
    inline std::string args_to_string(const char* argstr, Args... args)
    {
        std::ostringstream ss;
        if (0 < sizeof...(args)) {
            ss << "(" << argstr << ") -> (";
            output_args(ss, args...);
            ss << ") ";
        } else {
            if (*argstr != '\0') {
                ss << argstr << " ";
            }
        }
        return ss.str();
    }

    constexpr const char* deny_tags[] = CLOG_OPTION_DENY_TAGS;
    constexpr const char* deny_exclude_tags[] = CLOG_OPTION_DENY_EXCLUDE_TAGS;

    constexpr bool can_output_severity(clog::severity severity)
    {
#if defined(CLOG_OPTION_DISABLE_LOGGING)
        return severity != severity; // false
#elif defined(CLOG_OPTION_MIN_SEVERITY_INFO)
        return severity <= clog::severity::info;
#elif defined(CLOG_OPTION_MIN_SEVERITY_WARN)
        return severity <= clog::severity::warn;
#elif defined(CLOG_OPTION_MIN_SEVERITY_ERROR)
        return severity <= clog::severity::error;
#else
        return severity == severity; // true
#endif
    }

    constexpr bool match_deny_tags(const char* tag)
    {
        return (deny_tags[0] != nullptr) ?
            clog::internal::match_list_static(tag, deny_tags, clog::internal::count_of(deny_tags)) :
            false;
    }

    constexpr bool match_deny_exclude_tags(const char* tag)
    {
        return (deny_exclude_tags[0] != nullptr) ?
            clog::internal::match_list_static(tag, deny_exclude_tags, clog::internal::count_of(deny_exclude_tags)) :
            false;
    }

    constexpr bool can_output_tag(const char* tag)
    {
        return (tag != nullptr && match_deny_tags(tag)) ? match_deny_exclude_tags(tag) : true;
    }

    constexpr bool can_output(clog::severity severity, const char* tag)
    {
        return can_output_severity(severity) && can_output_tag(tag);
    }

    constexpr bool is_empty(const char* s)
    {
        return s == nullptr || *s == '\0';
    }
} // namespace internal

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
        clog::internal::output_wrapper<Type>::output(ss, data);
        return *this;
    }

    template <typename... Args>
    clog::record& format(const char* fmt, Args... args)
    {
        char buf[1024];
        snprintf(buf, sizeof(buf), fmt, args...);
        ss << buf;
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

struct formatter {
    static void format(const clog::record& record, std::ostream& stream)
    {
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_DATETIME)
        datetime(record, stream);
#endif
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_SEVERITY)
        severity(record, stream);
#endif
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_FILE)
        file(record, stream);
#endif
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_FUNC)
        func(record, stream);
#endif // CLOG_OPTION_DISABLE_OUTPUT_FUNC
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_TAG)
        tag(record, stream);
#endif
        stream << record.message();
    }

    static void datetime(const clog::record& record, std::ostream& stream)
    {
        // datetime
        const auto timet = std::chrono::system_clock::to_time_t(record.time);
        struct tm localt = {};
        internal::localtime_s(&localt, &timet);
        char datetime_buf[20];
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_DATETIME_YEAR)
        const char* datetime_fmt = "%Y-%m-%d %H:%M:%S";
#else
        const char* datetime_fmt = "%m-%d %H:%M:%S";
#endif
        std::strftime(datetime_buf, sizeof(datetime_buf), datetime_fmt, &localt);
        stream << "[" << datetime_buf;
        const auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(record.time.time_since_epoch()).count() % 1000;
        stream << "." << std::setw(3) << std::setfill('0') << msec << "] ";
    }

    static void severity(const clog::record& record, std::ostream& stream)
    {
        // severity
        stream << "[" << severity_to_str(record.severity) << "] ";
    }

    static void file(const clog::record& record, std::ostream& stream)
    {
        // file
        if (!clog::internal::is_empty(record.file)) {
#if defined(CLOG_OPTION_DISABLE_OUTPUT_FUNC) && !defined(CLOG_OPTION_DISABLE_OUTPUT_LINE)
            stream << "[" << record.file << "@" << record.line << "] ";
#else
            stream << "[" << record.file << "] ";
#endif
        }
    }

    static void func(const clog::record& record, std::ostream& stream)
    {
        // func/line
        if (!clog::internal::is_empty(record.func)) {
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_LINE)
            stream << "[" << record.func << "@" << record.line << "] ";
#else
            stream << "[" << record.func << "] ";
#endif
        }
    }

    static void tag(const clog::record& record, std::ostream& stream)
    {
        // tag
        if (!clog::internal::is_empty(record.tag)) {
            stream << "[" << record.tag << "] ";
        }
    }

    static constexpr const char* severity_to_str(clog::severity severity)
    {
        return (severity == clog::severity::debug) ? "debug" :
            (severity == clog::severity::info)     ? "info " :
            (severity == clog::severity::warn)     ? "warn " :
            (severity == clog::severity::error)    ? "error" :
                                                     "-----";
    }
};

struct logger {
    using handler = std::function<void(const clog::record&)>;

    logger(std::ostream& stream)
        : output_stream(stream)
        , output_handler(nullptr)
    {
    }

    logger(handler handler_)
        : output_stream(std::cout)
        , output_handler(handler_)
    {
    }

    void operator+=(const clog::record& record)
    {
        if (output_handler != nullptr) {
            output_handler(record);
        } else {
            clog::formatter::format(record, output_stream);
            output_stream << std::endl;
        }
    }

    std::ostream& output_stream;
    handler output_handler;
};

#if defined(_MSC_VER)
inline void windows_debugger(const clog::record& record)
{
    std::ostringstream ss;
    clog::formatter::format(record, ss);
    OutputDebugStringA(ss.str().c_str());
}
#endif

#if defined(__ANDROID__)
inline void android_debugger(const clog::record& record)
{
    android_LogPriority priority = (record.severity == clog::severity::debug) ? ANDROID_LOG_DEBUG :
        (record.severity == clog::severity::info)                             ? ANDROID_LOG_INFO :
        (record.severity == clog::severity::warn)                             ? ANDROID_LOG_WARN :
        (record.severity == clog::severity::error)                            ? ANDROID_LOG_ERROR :
                                                                                ANDROID_LOG_FATAL;
    const char* tag = (record.tag != nullptr) ? record.tag : "";
    std::ostringstream ss;
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_FILE)
    clog::formatter::file(record, ss);
#endif
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_FUNC)
    clog::formatter::func(record, ss);
#endif
    ss << record.message();
    __android_log_print(priority, tag, "%s", ss.str().c_str());
}
#endif

} // namespace clog
