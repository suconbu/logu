//
// Copyright (c) 2022 D.Miwa
// This software is released under the MIT License.
//

#pragma once

#include <array>
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

#define CLOG_DEBUG CLOG_OUTPUT(clog::severity::debug, CLOG_DEFAULT_INSTANCE_NAME)
#define CLOG_INFO  CLOG_OUTPUT(clog::severity::info, CLOG_DEFAULT_INSTANCE_NAME)
#define CLOG_WARN  CLOG_OUTPUT(clog::severity::warn, CLOG_DEFAULT_INSTANCE_NAME)
#define CLOG_ERROR CLOG_OUTPUT(clog::severity::error, CLOG_DEFAULT_INSTANCE_NAME)
#define CLOG       CLOG_OUTPUT(clog::severity::always, CLOG_DEFAULT_INSTANCE_NAME)

#define CLOG_DEBUG_IF(condition) CLOG_OUTPUT_IF(clog::severity::debug, CLOG_DEFAULT_INSTANCE_NAME, condition)
#define CLOG_INFO_IF(condition)  CLOG_OUTPUT_IF(clog::severity::info, CLOG_DEFAULT_INSTANCE_NAME, condition)
#define CLOG_WARN_IF(condition)  CLOG_OUTPUT_IF(clog::severity::warn, CLOG_DEFAULT_INSTANCE_NAME, condition)
#define CLOG_ERROR_IF(condition) CLOG_OUTPUT_IF(clog::severity::error, CLOG_DEFAULT_INSTANCE_NAME, condition)
#define CLOG_IF(condition)       CLOG_OUTPUT_IF(clog::severity::always, CLOG_DEFAULT_INSTANCE_NAME, condition)

#define CLOG_DEBUG_(name) CLOG_OUTPUT(clog::severity::debug, name)
#define CLOG_INFO_(name)  CLOG_OUTPUT(clog::severity::info, name)
#define CLOG_WARN_(name)  CLOG_OUTPUT(clog::severity::warn, name)
#define CLOG_ERROR_(name) CLOG_OUTPUT(clog::severity::error, name)
#define CLOG_(name)       CLOG_OUTPUT(clog::severity::always, name)

#define CLOG_DEBUG_IF_(name, condition) CLOG_OUTPUT_IF(clog::severity::debug, name, condition)
#define CLOG_INFO_IF_(name, condition)  CLOG_OUTPUT_IF(clog::severity::info, name, condition)
#define CLOG_WARN_IF_(name, condition)  CLOG_OUTPUT_IF(clog::severity::warn, name, condition)
#define CLOG_ERROR_IF_(name, condition) CLOG_OUTPUT_IF(clog::severity::error, name, condition)
#define CLOG_IF_(name, condition)       CLOG_OUTPUT_IF(clog::severity::always, name, condition)

#define CLOG_VARS(...) clog::internal::args_to_string("" #__VA_ARGS__, ##__VA_ARGS__)

#define CLOG_GET(name) clog::logger_holder<CLOG_HASH(name)>::get()

#ifdef _MSC_VER
#define CLOG_FUNC() __FUNCTION__
#else
#define CLOG_FUNC() __PRETTY_FUNCTION__
#endif

#define CLOG_FILE() clog::internal::basename(__FILE__)

#define CLOG_HASH(str) clog::internal::murmur3::murmur3(str, clog::internal::strlen_static(str))

//
// overridable macros
//

// #define CLOG_DISABLE_LOGGING
// #define CLOG_DISABLE_SEVERITY clog::severity::debug
// #define CLOG_DISABLE_INSTANCE_NAMES { "name-pattern" }

#if !defined(CLOG_DEFAULT_INSTANCE_NAME)
#define CLOG_DEFAULT_INSTANCE_NAME ""
#endif

#if !defined(CLOG_DEFAULT_HANDLER)
#define CLOG_DEFAULT_HANDLER std::cout
#endif

// internal macro

#define CLOG_OUTPUT_IF(severity, name, condition) \
    if (!(condition)) {                           \
    } else                                        \
        CLOG_OUTPUT(severity, name)

#if defined(_MSC_VER)
#define CLOG_OUTPUT(severity, name) __pragma(warning(push)) __pragma(warning(disable : 4127)) CLOG_OUTPUT_BODY(severity, name) __pragma(warning(pop))
#else
#define CLOG_OUTPUT(severity, name) CLOG_OUTPUT_BODY(severity, name)
#endif

#define CLOG_OUTPUT_BODY(severity, name)                                                                                               \
    if (!clog::internal::should_output(severity) || !clog::internal::should_output(name) || !CLOG_GET(name).should_output(severity)) { \
    } else                                                                                                                             \
        CLOG_GET(name)                                                                                                                 \
            += clog::record(severity, name, CLOG_FILE(), CLOG_FUNC(), __LINE__)

namespace clog {

#if defined(_MSC_VER)
using LPCSTR = const char*;
extern "C" void OutputDebugStringA(LPCSTR lpOutputString);
#endif

#if defined(__ANDROID__)
#include <android/log.h>
#endif

#if defined(__linux__)
extern "C" {
#include <syslog.h>
}
#endif

enum severity {
    debug,
    info,
    warn,
    error,
    always
};

enum class output_option {
    datetime, // default:true
    datetime_year, // default:true
    datetime_microsecond, // default:true
    severity, // default:true
    file, // default:true
    func, // default:true
    line, // default:true
    instance_name // default:true
};

namespace internal {
    struct output_option_array {
        bool operator[](clog::output_option index) const { return array[static_cast<int>(index)]; }
        bool& operator[](clog::output_option index) { return array[static_cast<int>(index)]; }
        std::array<bool, 8> array = {
            true, // datetime
            true, // datetime_year
            false, // datetime_microsecond
            true, // severity
            true, // file
            true, // func
            true, // line
            true, // instance_name
        };
    };
}

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

    constexpr bool should_output(clog::severity severity)
    {
#if defined(CLOG_DISABLE_LOGGING)
        return severity != severity; // false
#elif defined(CLOG_DISABLE_SEVERITY)
        return CLOG_DISABLE_SEVERITY <= severity;
#else
        return severity == severity; // true
#endif
    }

    constexpr bool should_output(const char* name)
    {
#if defined(CLOG_DISABLE_INSTANCE_NAMES)
        return match_list_static(name, CLOG_DISABLE_INSTANCE_NAMES, clog::internal::count_of(CLOG_DISABLE_INSTANCE_NAMES));
#else
        return name == name; // true
#endif
    }

    constexpr bool is_empty(const char* s)
    {
        return s == nullptr || *s == '\0';
    }
} // namespace internal

struct record {
    record(clog::severity severity_, const char* instance_name_, const char* file_, const char* func_, size_t line_)
        : severity(severity_)
        , instance_name(instance_name_)
        , file(file_)
        , func(func_)
        , line(line_)
        , time(std::chrono::system_clock::now())
    {
    }

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
    const char* instance_name;
    const char* file;
    const char* func;
    const size_t line;
    const std::chrono::system_clock::time_point time;
    std::ostringstream ss;
};

struct formatter {
    static void format(const clog::record& record, const clog::internal::output_option_array& output_options, std::ostream& stream)
    {
        if (output_options[clog::output_option::datetime]) {
            datetime(record, output_options, stream);
        }
        if (output_options[clog::output_option::severity]) {
            severity(record, output_options, stream);
        }
        if (output_options[clog::output_option::file]) {
            file(record, output_options, stream);
        }
        if (output_options[clog::output_option::func]) {
            func(record, output_options, stream);
        }
        if (output_options[clog::output_option::instance_name]) {
            instance_name(record, output_options, stream);
        }

        stream << record.message();
    }

    static void datetime(const clog::record& record, const clog::internal::output_option_array& output_options, std::ostream& stream)
    {
        // datetime
        const auto timet = std::chrono::system_clock::to_time_t(record.time);
        struct tm localt = {};
        internal::localtime_s(&localt, &timet);
        char datetime_buf[20];
        const char* datetime_fmt = output_options[clog::output_option::datetime_year] ? "%Y-%m-%d %H:%M:%S" : "%m-%d %H:%M:%S";
        std::strftime(datetime_buf, sizeof(datetime_buf), datetime_fmt, &localt);
        stream << "[" << datetime_buf;
        if (output_options[clog::output_option::datetime_microsecond]) {
            const auto usec = std::chrono::duration_cast<std::chrono::microseconds>(record.time.time_since_epoch()).count() % 1000000;
            stream << "." << std::setw(6) << std::setfill('0') << usec << "] ";
        } else {
            const auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(record.time.time_since_epoch()).count() % 1000;
            stream << "." << std::setw(3) << std::setfill('0') << msec << "] ";
        }
    }

    static void severity(const clog::record& record, const clog::internal::output_option_array& output_options, std::ostream& stream)
    {
        (void)output_options;
        stream << "[" << severity_to_str(record.severity) << "] ";
    }

    static void file(const clog::record& record, const clog::internal::output_option_array& output_options, std::ostream& stream)
    {
        if (!clog::internal::is_empty(record.file)) {
            if (output_options[clog::output_option::line]) {
                stream << "[" << record.file << "@" << record.line << "] ";
            } else {
                stream << "[" << record.file << "] ";
            }
        }
    }

    static void func(const clog::record& record, const clog::internal::output_option_array& output_options, std::ostream& stream)
    {
        // func/line
        if (!clog::internal::is_empty(record.func)) {
            if (output_options[clog::output_option::line]) {
                stream << "[" << record.func << "@" << record.line << "] ";
            } else {
                stream << "[" << record.func << "] ";
            }
        }
    }

    static void instance_name(const clog::record& record, const clog::internal::output_option_array& output_options, std::ostream& stream)
    {
        (void)output_options;
        if (!clog::internal::is_empty(record.instance_name)) {
            stream << "[" << record.instance_name << "] ";
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
#if defined(CLOG_DEFAULT_HANDLER)
        set_handler(CLOG_DEFAULT_HANDLER);
#endif
    }

    void operator+=(const clog::record& record)
    {
        std::ostringstream os;
        clog::formatter::format(record, output_options, os);
        for (auto& handler : handlers) {
            handler.output(record, os.str().c_str());
        }
    }

    bool should_output(clog::severity severity) const
    {
        return enable_logging && (min_severity <= severity);
    }

    logger& set_severity(bool enable)
    {
        enable_logging = enable;
        return *this;
    }

    logger& set_severity(clog::severity severity)
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

    logger& set_output_option(clog::output_option option, bool enable)
    {
        output_options[option] = enable;
        return *this;
    }

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

    bool enable_logging = true;
    clog::severity min_severity = clog::severity::debug;
    std::vector<handler> handlers;
    clog::internal::output_option_array output_options;
};

namespace handler {
    inline void windows_debugger(const char* str)
    {
#if defined(_MSC_VER)
        OutputDebugStringA(str);
#else
        (void)str;
#endif
    }

    inline void android_debugger(const clog::record& record)
    {
#if defined(__ANDROID__)
        const android_LogPriority priority = (record.severity == clog::severity::debug) ? ANDROID_LOG_DEBUG :
            (record.severity == clog::severity::info)                                   ? ANDROID_LOG_INFO :
            (record.severity == clog::severity::warn)                                   ? ANDROID_LOG_WARN :
            (record.severity == clog::severity::error)                                  ? ANDROID_LOG_ERROR :
                                                                                          ANDROID_LOG_FATAL;
        const char* tag = (record.instance_name != nullptr) ? record.instance_name : "";
        std::ostringstream ss;
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_FILE)
        clog::formatter::file(record, ss);
#endif
#if !defined(CLOG_OPTION_DISABLE_OUTPUT_FUNC)
        clog::formatter::func(record, ss);
#endif
        ss << record.message();
        __android_log_print(priority, tag, "%s", ss.str().c_str());
#else
        (void)record;
#endif
    }

    inline void linux_syslog(const clog::record& record, const char* str)
    {
#if defined(__linux__)
        const int level = (record.severity == clog::severity::debug) ? LOG_DEBUG :
            (record.severity == clog::severity::info)                ? LOG_INFO :
            (record.severity == clog::severity::warn)                ? LOG_WARNING :
            (record.severity == clog::severity::error)               ? LOG_ERR :
                                                                       LOG_CRIT;
        syslog(LOG_USER | level, "%s", str);
#else
        (void)record;
        (void)str;
#endif
    }
} // namespace handler

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
