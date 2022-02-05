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
        return (p == s) ? p : (*p == c) ? (p + 1)
                                        : strrchr_static(s, p - 1, c);
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
}

struct record {
    record(clog::severity severity, const char* file, const char* func, size_t line)
        : severity(severity)
        , file(file)
        , func(func)
        , line(line)
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
    const char* file;
    const char* func;
    const size_t line;
    const std::chrono::system_clock::time_point time;
    std::ostringstream ss;
};

struct writer {
    writer(std::ostream& stream)
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

#if defined(CLOG_OPTION_MIN_SEVERITY_INFO)
#define CLOG_CHECK(severity_) (severity_ <= clog::severity::info)
#elif defined(CLOG_OPTION_MIN_SEVERITY_WARN)
#define CLOG_CHECK(severity_) (severity_ <= clog::severity::warn)
#elif defined(CLOG_OPTION_MIN_SEVERITY_ERROR)
#define CLOG_CHECK(severity_) (severity_ <= clog::severity::error)
#else
#define CLOG_CHECK(s) true
#endif

#ifdef _MSC_VER
#define CLOG_GET_FUNC() __FUNCTION__
#else
#define CLOG_GET_FUNC() __PRETTY_FUNCTION__
#endif

#define CLOG_GET_FILE() clog::util::basename(__FILE__)

#define CLOG_(severity_)                        \
    if (!CLOG_CHECK(severity_)) {               \
    } else                                      \
        clog::writer(CLOG_OPTION_OUTPUT_STREAM) \
            += clog::record(severity_, CLOG_GET_FILE(), CLOG_GET_FUNC(), __LINE__).ref()

#define CLOG_DEBUG CLOG_(clog::severity::debug)
#define CLOG_INFO CLOG_(clog::severity::info)
#define CLOG_WARN CLOG_(clog::severity::warn)
#define CLOG_ERROR CLOG_(clog::severity::error)
#define CLOG CLOG_(clog::severity::none)
