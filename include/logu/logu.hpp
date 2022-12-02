//
// Copyright (c) 2022 D.Miwa
// This software is released under the MIT License, see LICENSE.
//

#pragma once

#include <array>
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <vector>

// Options:
// LOGU_DISABLE_LOGGING - If defined, disable all macros

// Basic logging macros

#define LOGU_DEBUG LOGU_OUTPUT(logu::severity::debug, LOGU_DEFAULT_TAGNAME)
#define LOGU_INFO  LOGU_OUTPUT(logu::severity::info, LOGU_DEFAULT_TAGNAME)
#define LOGU_WARN  LOGU_OUTPUT(logu::severity::warn, LOGU_DEFAULT_TAGNAME)
#define LOGU_ERROR LOGU_OUTPUT(logu::severity::error, LOGU_DEFAULT_TAGNAME)
#define LOGU       LOGU_OUTPUT(logu::severity::none, LOGU_DEFAULT_TAGNAME)

#define LOGU_DEBUG_(tagname) LOGU_OUTPUT(logu::severity::debug, tagname)
#define LOGU_INFO_(tagname)  LOGU_OUTPUT(logu::severity::info, tagname)
#define LOGU_WARN_(tagname)  LOGU_OUTPUT(logu::severity::warn, tagname)
#define LOGU_ERROR_(tagname) LOGU_OUTPUT(logu::severity::error, tagname)
#define LOGU_(tagname)       LOGU_OUTPUT(logu::severity::none, tagname)

// Get logger instance
#define LOGU_GET(tagname)        logu::internal::logger_holder::get(tagname)
#define LOGU_GET_STATIC(tagname) logu::internal::static_logger_holder<LOGU_HASH(tagname)>::get(tagname)
#define LOGU_GET_DEFAULT()       LOGU_GET_STATIC(LOGU_DEFAULT_TAGNAME)

// Get function name (e.g. "void myclass::myfunc()")
#ifdef _MSC_VER
#define LOGU_FUNC() __FUNCTION__
#else
#define LOGU_FUNC() __PRETTY_FUNCTION__
#endif

// Get filename excluding directory path
#define LOGU_FILE() logu::internal::basename(__FILE__)

// Get hash code from string
#define LOGU_HASH(str) logu::internal::murmur3::murmur3(str, logu::internal::strlen_static(str))

// String the given arguments together with their values (e.g. "(n, str) -> (123, hello)")
#define LOGU_VARSTR(...) logu::internal::args_to_string("" #__VA_ARGS__, ##__VA_ARGS__)

//
// Internal macro
//

#define LOGU_DEFAULT_TAGNAME ""

// clang-format off

#define LOGU_OUTPUT(severity, tagname)    \
    LOGU_SHOULD_OUTPUT(severity, tagname) \
        LOGU_GET_STATIC(tagname) += logu::record(severity, tagname, LOGU_FILE(), LOGU_FUNC(), __LINE__)

#if defined(LOGU_DISABLE_LOGGING)

#if defined(_MSC_VER)
#define LOGU_SHOULD_OUTPUT(severity, tagname) \
    __pragma(warning(push))                   \
    __pragma(warning(disable : 4127))         \
    if (true) { }                             \
    else __pragma(warning(pop))
#else // _MSC_VAR
#define LOGU_SHOULD_OUTPUT(severity, tagname) \
    if (true) { } else
#endif // _MSC_VAR

#else // LOGU_DISABLE_LOGGING

#define LOGU_SHOULD_OUTPUT(severity, tagname)         \
    if (!LOGU_GET_STATIC(tagname).should_output(severity)) { } else

#endif // LOGU_DISABLE_LOGGING

// clang-format on

#if defined(_WIN32)
#include <time.h>
#endif

#if defined(__ANDROID__)
#if defined(LOGU_ENABLE_PLATFORM_LOGGER_ANDROID)
#include <android/log.h>
#endif
#endif

#if defined(__linux__)
#include <unistd.h>
#if !defined(__BIONIC__)
#include <sys/syscall.h>
#endif
#if defined(LOGU_ENABLE_PLATFORM_LOGGER_LINUX)
#include <syslog.h>
#endif
#endif

namespace logu {

enum severity {
    debug,
    info,
    warn,
    error,
    none
};

namespace internal {

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

    constexpr bool is_null_or_empty(const char* s)
    {
        return s == nullptr || *s == '\0';
    }

#if defined(_WIN32)
    extern "C" __declspec(dllimport) unsigned long __stdcall GetCurrentThreadId();
#endif
    inline uint64_t get_threadid()
    {
#if defined(_WIN32)
        return static_cast<uint64_t>(GetCurrentThreadId());
#elif defined(__BIONIC__)
        return gettid();
#elif defined(__linux__)
        return static_cast<uint64_t>(syscall(__NR_gettid));
#else
        return 0;
#endif
    }

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

    class noncopyable {
    protected:
        noncopyable() = default;

    private:
        noncopyable(const noncopyable&);
        noncopyable& operator=(const noncopyable&);
    };

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
#if defined(_MSC_VER)
#pragma warning(disable : 4127)
        if (0 < sizeof...(args)) {
#pragma warning(default : 4127)
#else
        if (0 < sizeof...(args)) {
#endif
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

} // namespace internal

class record {
public:
    record(logu::severity severity, const char* tagname, const char* file, const char* func, size_t line)
        : severity_(severity)
        , tagname_(tagname)
        , file_(file)
        , func_(func)
        , line_(line)
        , threadid_(logu::internal::get_threadid())
        , time_(std::chrono::system_clock::now())
    {
    }

    record() = delete;

    logu::severity severity() const { return severity_; };
    const char* tagname() const { return tagname_; };
    const char* file() const { return file_; };
    const char* func() const { return func_; };
    size_t line() const { return line_; };
    uint64_t threadid() const { return threadid_; };
    std::chrono::system_clock::time_point time() const { return time_; };

    template <typename Type>
    logu::record& operator<<(const Type& data)
    {
        logu::internal::output_wrapper<Type>::output(ss_, data);
        return *this;
    }

    template <typename... Args>
    logu::record& format(const char* fmt, Args... args)
    {
        char buf[1024];
        snprintf(buf, sizeof(buf), fmt, args...);
        ss_ << buf;
        return *this;
    }

    std::string message() const
    {
        return ss_.str();
    }

private:
    const logu::severity severity_;
    const char* const tagname_;
    const char* const file_;
    const char* const func_;
    const size_t line_;
    const uint64_t threadid_;
    const std::chrono::system_clock::time_point time_;
    std::ostringstream ss_;
};

class formatter_base {
public:
    virtual ~formatter_base() = default;
    virtual std::string format(const logu::record& record) = 0;
};

class formatter : public logu::formatter_base {
public:
    enum class option {
        datetime,
        datetime_year,
        datetime_microsecond,
        severity,
        threadid,
        file,
        func,
        line,
        tagname
    };

public:
    virtual ~formatter() = default;

    std::string format(const logu::record& record)
    {
        std::ostringstream stream;
        if (options_.at(option::datetime)) {
            datetime(record, stream);
        }
        if (options_.at(option::severity)) {
            severity(record, stream);
        }
        if (options_.at(option::threadid)) {
            threadid(record, stream);
        }
        if (options_.at(option::file)) {
            file(record, stream);
        }
        if (options_.at(option::func)) {
            func(record, stream);
        }
        if (options_.at(option::tagname)) {
            tagname(record, stream);
        }
        stream << record.message();
        return stream.str();
    }

    formatter& set_option(option option_, bool enable)
    {
        options_[option_] = enable;
        return *this;
    }

    static constexpr const char* severity_to_str(logu::severity severity)
    {
        return (severity == logu::severity::debug) ? "DEBUG" :
            (severity == logu::severity::info)     ? "INFO " :
            (severity == logu::severity::warn)     ? "WARN " :
            (severity == logu::severity::error)    ? "ERROR" :
                                                     "-----";
    }

private:
    std::unordered_map<option, bool> options_ = {
        { option::datetime, true },
        { option::datetime_year, true },
        { option::datetime_microsecond, false },
        { option::severity, true },
        { option::threadid, true },
        { option::file, false },
        { option::func, true },
        { option::line, true },
        { option::tagname, true }
    };

    void datetime(const logu::record& record, std::ostream& stream) const
    {
        const auto timet = std::chrono::system_clock::to_time_t(record.time());
        struct tm localt = {};
        internal::localtime_s(&localt, &timet);
        char datetime_buf[20];
        const char* datetime_fmt = options_.at(option::datetime_year) ? "%Y-%m-%d %H:%M:%S" : "%m-%d %H:%M:%S";
        std::strftime(datetime_buf, sizeof(datetime_buf), datetime_fmt, &localt);
        stream << "[" << datetime_buf;
        if (options_.at(option::datetime_microsecond)) {
            const auto usec = std::chrono::duration_cast<std::chrono::microseconds>(record.time().time_since_epoch()).count() % 1000000;
            stream << "." << std::setw(6) << std::setfill('0') << usec << "] ";
        } else {
            const auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(record.time().time_since_epoch()).count() % 1000;
            stream << "." << std::setw(3) << std::setfill('0') << msec << "] ";
        }
    }

    void severity(const logu::record& record, std::ostream& stream) const
    {
        stream << "[" << severity_to_str(record.severity()) << "] ";
    }

    void threadid(const logu::record& record, std::ostream& stream) const
    {
        stream << "[" << record.threadid() << "] ";
    }

    void file(const logu::record& record, std::ostream& stream) const
    {
        if (!logu::internal::is_null_or_empty(record.file())) {
            if (options_.at(option::line)) {
                stream << "[" << record.file() << "@" << record.line() << "] ";
            } else {
                stream << "[" << record.file() << "] ";
            }
        }
    }

    void func(const logu::record& record, std::ostream& stream) const
    {
        if (!logu::internal::is_null_or_empty(record.func())) {
            if (options_.at(option::line)) {
                stream << "[" << record.func() << "@" << record.line() << "] ";
            } else {
                stream << "[" << record.func() << "] ";
            }
        }
    }

    void tagname(const logu::record& record, std::ostream& stream) const
    {
        if (!logu::internal::is_null_or_empty(record.tagname())) {
            stream << "[" << record.tagname() << "] ";
        }
    }
};

class logger : logu::internal::noncopyable {
public:
    logger(const char* tagname)
        : tagname_(tagname)
    {
#if defined(LOGU_ENABLE_PLATFORM_LOGGER_WINDOWS) || defined(LOGU_ENABLE_PLATFORM_LOGGER_ANDROID) || defined(LOGU_ENABLE_PLATFORM_LOGGER_LINUX)
        void platform_logger(const logu::record& record, const char* str);
        set_handler(std::cout, platform_logger);
#else
        set_handler(std::cout);
#endif
    }

    logger() = delete;

    void operator+=(const logu::record& record)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (formatter_) {
            const auto str = formatter_->format(record);
            for (auto& h : handlers_) {
                h.output(record, str.c_str());
            }
        }
    }

    bool should_output(logu::severity severity) const
    {
        return enable_logging_ && (min_severity_ <= severity) && (severity <= max_severity_);
    }

    void copy_from(const logu::logger& rhs)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        handlers_ = rhs.handlers_;
        formatter_ = rhs.formatter_;
        min_severity_ = rhs.min_severity_;
        max_severity_ = rhs.max_severity_;
        enable_logging_ = rhs.enable_logging_;
    }

    template <typename... Args>
    logger& set_handler(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        handlers_.clear();
        set_handler_internal(std::forward<Args>(args)...);
        return *this;
    }

    template <typename FormatterType>
    logger& set_formatter(const FormatterType& formatter)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        formatter_ = std::unique_ptr<logu::formatter_base>(new FormatterType(formatter));
        return *this;
    }

    logger& set_severity(logu::severity min_severity, logu::severity max_severity = logu::severity::none)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        min_severity_ = min_severity;
        max_severity_ = max_severity;
        return *this;
    }

    logger& set_enable(bool enable)
    {
        enable_logging_ = enable;
        return *this;
    }

    const std::string& tagname() const { return tagname_; }

private:
    class handler {
    public:
        using functype_str = std::function<void(const char*)>;
        using functype_record = std::function<void(const logu::record&)>;
        using functype_record_str = std::function<void(const logu::record&, const char*)>;

        // clang-format off
        handler(functype_str func) : output_func_str_(func) { }
        handler(functype_record func) : output_func_record_(func) { }
        handler(functype_record_str func) : output_func_record_str_(func) { }
        handler(std::ostream& stream) : output_stream_(stream) { }
        // clang-format on

        handler(const char* filename)
            : output_filestream_(std::make_shared<std::ofstream>(filename))
            , output_stream_(*output_filestream_)
        {
        }

        void output(const logu::record& record, const char* str)
        {
            if (output_func_str_ != nullptr) {
                output_func_str_(str);
            } else if (output_func_record_ != nullptr) {
                output_func_record_(record);
            } else if (output_func_record_str_ != nullptr) {
                output_func_record_str_(record, str);
            } else {
                output_stream_.get() << str << std::endl;
            }
        }

    private:
        std::shared_ptr<std::ofstream> output_filestream_;
        std::reference_wrapper<std::ostream> output_stream_ = std::ref(std::cout);
        functype_str output_func_str_;
        functype_record output_func_record_;
        functype_record_str output_func_record_str_;
    };

private:
    std::string tagname_;
    std::vector<handler> handlers_;
    std::shared_ptr<logu::formatter_base> formatter_ = std::make_shared<logu::formatter>();
    logu::severity min_severity_ = logu::severity::debug;
    logu::severity max_severity_ = logu::severity::none;
    bool enable_logging_ = true;
    std::mutex mtx_;

    template <typename First, typename... Args>
    void set_handler_internal(First&& first, Args&&... args)
    {
        handlers_.emplace_back(first);
        set_handler_internal(std::forward<Args>(args)...);
    }

    template <typename First>
    void set_handler_internal(First&& first)
    {
        handlers_.emplace_back(first);
    }

    void set_handler_internal() { }
};

namespace internal {
    class logger_holder : logu::internal::noncopyable {
    public:
        static logu::logger& get(const char* tagname, bool with_lock = true)
        {
            static logger_holder instance;
            return with_lock ? instance.find_with_lock(tagname) : instance.find(tagname);
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<logu::logger>> instances_;
        std::mutex mtx_;

        logu::logger& find_with_lock(const char* tagname)
        {
            std::lock_guard<std::mutex> lock(mtx_);
            return find(tagname);
        }

        logu::logger& find(const char* tagname)
        {
            auto itr = instances_.find(tagname);
            if (itr != instances_.end()) {
                return *itr->second;
            } else {
                return *(instances_[tagname] = std::unique_ptr<logu::logger>(new logu::logger(tagname)));
            }
        }
    };

    template <uint32_t InstanceId>
    class static_logger_holder : logu::internal::noncopyable {
    public:
        static logu::logger& get(const char* tagname)
        {
            static static_logger_holder<InstanceId> instance(logger_holder::get(tagname, false));
            return (instance.logger_.tagname() == tagname) ? instance.logger_ : instance.find(tagname);
        }

    private:
        logu::logger& logger_;
        std::unique_ptr<static_logger_holder<InstanceId>> next_;
        std::mutex mtx_;

        static_logger_holder(logu::logger& logger)
            : logger_(logger)
        {
        }

        logu::logger& find(const char* tagname)
        {
            std::lock_guard<std::mutex> lock(mtx_);
            static_logger_holder<InstanceId>* ptr = this;
            bool found = false;
            while (ptr->next_) {
                ptr = ptr->next_.get();
                if (ptr->logger_.tagname() == tagname) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                ptr->next_ = std::unique_ptr<static_logger_holder<InstanceId>>(
                    new static_logger_holder<InstanceId>(logger_holder::get(tagname, false)));
                ptr = ptr->next_.get();
            }
            return ptr->logger_;
        }
    };
} // namespace internal

namespace internal {

#if defined(_MSC_VER) && defined(LOGU_ENABLE_PLATFORM_LOGGER_WINDOWS)
    extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(const char* lpOutputString);
#endif
    inline void windows_debugger_handler(const char* str)
    {
#if defined(_MSC_VER) && defined(LOGU_ENABLE_PLATFORM_LOGGER_WINDOWS)
        OutputDebugStringA(str);
        OutputDebugStringA("\n");
#else
        (void)str;
#endif
    }

    inline void android_debugger_handler(const logu::record& record)
    {
#if defined(__ANDROID__) && defined(LOGU_ENABLE_PLATFORM_LOGGER_ANDROID)
        const android_LogPriority priority = (record.severity() == logu::severity::debug) ? ANDROID_LOG_DEBUG :
            (record.severity() == logu::severity::info)                                   ? ANDROID_LOG_INFO :
            (record.severity() == logu::severity::warn)                                   ? ANDROID_LOG_WARN :
            (record.severity() == logu::severity::error)                                  ? ANDROID_LOG_ERROR :
                                                                                            ANDROID_LOG_FATAL;
        const char* tag = logu::internal::is_null_or_empty(record.tagname()) ? "(default)" : record.tagname();
        std::ostringstream ss;
        if (!logu::internal::is_null_or_empty(record.func())) {
            ss << "[" << record.func() << "@" << record.line() << "] ";
        }
        ss << record.message();
        __android_log_print(priority, tag, "%s", ss.str().c_str());
#else
        (void)record;
#endif
    }

    inline void linux_syslog_handler(const logu::record& record, const char* str)
    {
#if defined(__linux__) && defined(LOGU_ENABLE_PLATFORM_LOGGER_LINUX)
        const int level = (record.severity() == logu::severity::debug) ? LOG_DEBUG :
            (record.severity() == logu::severity::info)                ? LOG_INFO :
            (record.severity() == logu::severity::warn)                ? LOG_WARNING :
            (record.severity() == logu::severity::error)               ? LOG_ERR :
                                                                         LOG_CRIT;
        syslog(LOG_USER | level, "%s", str);
#else
        (void)record;
        (void)str;
#endif
    }

} // namespace internal

inline void platform_logger(const logu::record& record, const char* str)
{
    (void)record;
    (void)str;
#if defined(_MSC_VER)
    logu::internal::windows_debugger_handler(str);
#endif
#if defined(__ANDROID__)
    logu::internal::android_debugger_handler(record);
#endif
#if defined(__linux__)
    logu::internal::linux_syslog_handler(record, str);
#endif
}

} // namespace logu
