//
// Copyright (c) 2022 D.Miwa
// This software is released under the MIT License.
//

#pragma once

#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

//
// basic macro
//

#define CLOG_DEBUG CLOG_OUTPUT(clog::severity::debug, "")
#define CLOG_INFO  CLOG_OUTPUT(clog::severity::info, "")
#define CLOG_WARN  CLOG_OUTPUT(clog::severity::warn, "")
#define CLOG_ERROR CLOG_OUTPUT(clog::severity::error, "")
#define CLOG       CLOG_OUTPUT(clog::severity::none, "")

#define CLOG_DEBUG_(tag) CLOG_OUTPUT(clog::severity::debug, tag)
#define CLOG_INFO_(tag)  CLOG_OUTPUT(clog::severity::info, tag)
#define CLOG_WARN_(tag)  CLOG_OUTPUT(clog::severity::warn, tag)
#define CLOG_ERROR_(tag) CLOG_OUTPUT(clog::severity::error, tag)
#define CLOG_(tag)       CLOG_OUTPUT(clog::severity::none, tag)

#define CLOG_VARSTR(...) clog::internal::args_to_string("" #__VA_ARGS__, ##__VA_ARGS__)

#define CLOG_GET(name) clog::logger_holder<CLOG_GET_HASH(name)>::get()

#ifdef _MSC_VER
#define CLOG_GET_FUNC() __FUNCTION__
#else
#define CLOG_GET_FUNC() __PRETTY_FUNCTION__
#endif
#define CLOG_GET_FILE()    clog::internal::basename(__FILE__)
#define CLOG_GET_HASH(str) clog::internal::murmur3::murmur3(str, clog::internal::strlen_static(str))

//
// overridable macros
//

// #define CLOG_DISABLE_LOGGING
// #define CLOG_DISABLE_SEVERITY_UPTO

#if !defined(CLOG_DEFAULT_OUTPUT_TO)
#define CLOG_DEFAULT_OUTPUT_TO std::cout
#endif

// #define CLOG_OPTION_DISABLE_OUTPUT_DATETIME
// #define CLOG_OPTION_DISABLE_OUTPUT_DATETIME_YEAR
// #define CLOG_OPTION_DISABLE_OUTPUT_SEVERITY
// #define CLOG_OPTION_DISABLE_OUTPUT_FILE
// #define CLOG_OPTION_DISABLE_OUTPUT_FUNC
// #define CLOG_OPTION_DISABLE_OUTPUT_LINE
// #define CLOG_OPTION_DISABLE_OUTPUT_TAG

// clang-format off
#if !defined(CLOG_DISABLE_INSTANCES)
#define CLOG_DISABLE_INSTANCES { nullptr }
#endif
#if !defined(CLOG_DISABLE_TAG_PATTERNS)
#define CLOG_DISABLE_TAG_PATTERNS { nullptr }
#endif
#if !defined(CLOG_DISABLE_TAG_PATTERNS_EXCLUDE)
#define CLOG_DISABLE_TAG_PATTERNS_EXCLUDE { nullptr }
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
        CLOG_GET(tag)                                                          \
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
    namespace murmur3 {
        constexpr uint32_t seed = 0;

        constexpr uint32_t to_uint32(char const* key, size_t i = sizeof(uint32_t), uint32_t u32 = 0) { return i ? to_uint32(key, i - 1, (u32 << 8) | key[i - 1]) : u32; }

        constexpr uint32_t murmur3a_5(uint32_t h) { return (h * 5) + 0xe6546b64; }
        constexpr uint32_t murmur3a_4(uint32_t h) { return murmur3a_5((h << 13) | (h >> 19)); }
        constexpr uint32_t murmur3a_3(uint32_t k, uint32_t h) { return murmur3a_4(h ^ k); }
        constexpr uint32_t murmur3a_2(uint32_t k, uint32_t h) { return murmur3a_3(k * 0x1b873593, h); }
        constexpr uint32_t murmur3a_1(uint32_t k, uint32_t h) { return murmur3a_2((k << 15) | (k >> 17), h); }
        constexpr uint32_t murmur3a_0(uint32_t k, uint32_t h) { return murmur3a_1(k * 0xcc9e2d51, h); }
        constexpr uint32_t murmur3a(char const* key, size_t i, uint32_t h = seed) { return i ? murmur3a(key + sizeof(uint32_t), i - 1, murmur3a_0(to_uint32(key), h)) : h; }

        constexpr uint32_t murmur3b_3(uint32_t k, uint32_t h) { return h ^ k; }
        constexpr uint32_t murmur3b_2(uint32_t k, uint32_t h) { return murmur3b_3(k * 0x1b873593, h); }
        constexpr uint32_t murmur3b_1(uint32_t k, uint32_t h) { return murmur3b_2((k << 15) | (k >> 17), h); }
        constexpr uint32_t murmur3b_0(uint32_t k, uint32_t h) { return murmur3b_1(k * 0xcc9e2d51, h); }
        constexpr uint32_t murmur3b(char const* key, size_t i, uint32_t h) { return i ? murmur3b_0(to_uint32(key, i), h) : h; }

        constexpr uint32_t murmur3c_4(uint32_t h) { return h ^ (h >> 16); }
        constexpr uint32_t murmur3c_3(uint32_t h) { return murmur3c_4(h * 0xc2b2ae35); }
        constexpr uint32_t murmur3c_2(uint32_t h) { return murmur3c_3(h ^ (h >> 13)); }
        constexpr uint32_t murmur3c_1(uint32_t h) { return murmur3c_2(h * 0x85ebca6b); }
        constexpr uint32_t murmur3c_0(uint32_t h) { return murmur3c_1(h ^ (h >> 16)); }
        constexpr uint32_t murmur3c(uint32_t h, size_t len) { return murmur3c_0(h ^ (uint32_t)len); }

        constexpr uint32_t murmur3(char const* str, size_t len) { return murmur3c(murmur3b(str + ((len >> 2) * sizeof(uint32_t)), len & 3, murmur3a(str, len >> 2)), len); }
        constexpr uint32_t operator"" _murmur3(char const* str, size_t len) { return murmur3(str, len); }
    }

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

    constexpr const char* disable_tag_patterns[] = CLOG_DISABLE_TAG_PATTERNS;
    constexpr const char* disable_tag_patterns_exclude[] = CLOG_DISABLE_TAG_PATTERNS_EXCLUDE;

    constexpr bool can_output_severity(clog::severity severity)
    {
#if defined(CLOG_DISABLE_LOGGING)
        return severity != severity; // false
#elif defined(CLOG_DISABLE_SEVERITY_UPTO)
        return severity <= CLOG_DISABLE_SEVERITY_UPTO;
#else
        return severity == severity; // true
#endif
    }

    constexpr bool match_disabled_tag(const char* tag)
    {
        return (disable_tag_patterns[0] != nullptr) ?
            clog::internal::match_list_static(tag, disable_tag_patterns, clog::internal::count_of(disable_tag_patterns)) :
            false;
    }

    constexpr bool match_disabled_tag_exclude(const char* tag)
    {
        return (disable_tag_patterns_exclude[0] != nullptr) ?
            clog::internal::match_list_static(tag, disable_tag_patterns_exclude, clog::internal::count_of(disable_tag_patterns_exclude)) :
            false;
    }

    constexpr bool can_output_tag(const char* tag)
    {
        return (tag != nullptr && match_disabled_tag(tag)) ? match_disabled_tag_exclude(tag) : true;
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
    logger()
    {
#if defined(CLOG_DEFAULT_OUTPUT_TO)
        set_handler(CLOG_DEFAULT_OUTPUT_TO);
#endif
    }

    void operator+=(const clog::record& record)
    {
        std::ostringstream os;
        clog::formatter::format(record, os);
        for (auto& handler : handlers) {
            handler.output(record, os.str().c_str());
        }
    }

    logger& set_severity(clog::severity severity) &
    {
        min_severity = severity;
        return *this;
    }

    template <typename... Args>
    logger& set_handler(Args&&... args)
    {
        handlers.clear();
        set_handler_internal(std::forward<Args>(args)...);
        return *this;
    }

    template <typename First, typename... Args>
    void set_handler_internal(First&& first, Args&&... args)
    {
        handlers.emplace_back(first);
        set_handler_internal(std::forward<Args>(args)...);
    }

    template <typename First>
    void set_handler_internal(First&& first)
    {
        handlers.emplace_back(first);
    }

    void set_handler_internal() { }

    // 出力項目の設定
    // logger& set_outputoption(outputoption&& option)
    // {
    // }

    struct handler {
        using functype_str = std::function<void(const char*)>;
        using functype_record = std::function<void(const clog::record&)>;
        using functype_record_str = std::function<void(const clog::record&, const char*)>;

        // clang-format off
        handler(functype_str func) : output_func_str(func) { }
        handler(functype_record func) : output_func_record(func) { }
        handler(functype_record_str func) : output_func_record_str(func) { }
        handler(std::ostream& stream) : output_stream(stream) { }
        // clang-format on

        handler(const char* filename)
            : output_filestream(std::make_unique<std::ofstream>(filename))
            , output_stream(*output_filestream)
        {
        }

        void output(const clog::record& record, const char* str)
        {
            if (output_func_str != nullptr) {
                output_func_str(str);
            } else if (output_func_record != nullptr) {
                output_func_record(record);
            } else if (output_func_record_str != nullptr) {
                output_func_record_str(record, str);
            } else {
                output_stream << str << std::endl;
            }
        }

        std::unique_ptr<std::ofstream> output_filestream;
        std::ostream& output_stream = std::cout;
        const functype_str output_func_str;
        const functype_record output_func_record;
        const functype_record_str output_func_record_str;
    };

    clog::severity min_severity = clog::severity::debug;
    std::vector<handler> handlers;
};

#if defined(_MSC_VER)
inline void
windows_debugger(const clog::record& record)
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

template <uint32_t LoggerId>
class logger_holder {
public:
    static clog::logger& get()
    {
        static logger_holder<LoggerId> logger_holder;
        return logger_holder.logger_;
    }

private:
    clog::logger logger_;

    logger_holder() { }
};

} // namespace clog
