#include "clog/clog.hpp"

#include "gtest/gtest.h"

#include <regex>
#include <string>
#include <vector>

//#define TEST_ENABLE_OUTPUT_TO_STDOUT

class ClogTest : public ::testing::Test {
protected:
    virtual void SetUp() { }
    virtual void TearDown() { }

    std::string GetPattern(clog::severity severity)
    {
        return GetPattern(severity, "");
    }

    std::string GetPattern(clog::severity severity, std::string name)
    {
        std::string s;
        s += "\\[\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\] ";
        std::string severity_str = (severity == clog::severity::debug) ? "DEBUG" :
            (severity == clog::severity::info)                         ? "INFO " :
            (severity == clog::severity::warn)                         ? "WARN " :
            (severity == clog::severity::error)                        ? "ERROR" :
                                                                         "-----";
        s += "\\[" + severity_str + "\\] ";
        s += "\\[\\d+\\] "; // threadid
        s += "\\[[^@]+@\\d+\\] "; // func@line
        if (!name.empty()) {
            s += "\\[" + name + "\\] "; // tagname
        }
        return s;
    }
};

TEST_F(ClogTest, Severity)
{
    std::string str;

    testing::internal::CaptureStdout();
    CLOG_DEBUG << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::debug) + "test")));

    testing::internal::CaptureStdout();
    CLOG_INFO << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::info) + "test")));

    testing::internal::CaptureStdout();
    CLOG_WARN << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::warn) + "test")));

    testing::internal::CaptureStdout();
    CLOG_ERROR << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::error) + "test")));

    testing::internal::CaptureStdout();
    CLOG << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::none) + "test")));

    testing::internal::CaptureStdout();
    CLOG_DEBUG_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::debug, "name") + "test")));

    testing::internal::CaptureStdout();
    CLOG_INFO_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::info, "name") + "test")));

    testing::internal::CaptureStdout();
    CLOG_WARN_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::warn, "name") + "test")));

    testing::internal::CaptureStdout();
    CLOG_ERROR_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::error, "name") + "test")));

    testing::internal::CaptureStdout();
    CLOG_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(clog::severity::none, "name") + "test")));
}

TEST_F(ClogTest, OutputFormat)
{
    constexpr auto name = "OutputFormat";
    std::string str;

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));

    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, true));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[OutputFormat\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, true)
                           .set_option(clog::formatter::option::func, true)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[test\\.cpp\\] \\[[^\\]]+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, true)
                           .set_option(clog::formatter::option::func, true)
                           .set_option(clog::formatter::option::line, true)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[test\\.cpp@\\d+\\] \\[[^@]+@\\d+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, true)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, true)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[test\\.cpp@\\d+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, true)
                           .set_option(clog::formatter::option::line, true)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[[^@]+@\\d+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, true)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[\\d+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, true)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, true)
                           .set_option(clog::formatter::option::datetime_year, false)
                           .set_option(clog::formatter::option::datetime_microsecond, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, true)
                           .set_option(clog::formatter::option::datetime_year, false)
                           .set_option(clog::formatter::option::datetime_microsecond, true)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\[\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{6}\\] test")));
}

TEST_F(ClogTest, CopyLogger)
{
    constexpr auto from = "CopyLoggerFrom";
    constexpr auto to = "CopyLoggerTo";
    std::string str;

    CLOG_GET_DEFAULT()
        .set_severity(clog::severity::warn, clog::severity::error)
        .set_enable(false)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::tagname, false));
    CLOG_GET(to).copy_from(CLOG_GET_DEFAULT());

    testing::internal::CaptureStdout();
    CLOG_DEBUG_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    CLOG_INFO_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    CLOG_WARN_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    CLOG_ERROR_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());

    CLOG_GET(to).set_enable(true);

    testing::internal::CaptureStdout();
    CLOG_DEBUG_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    CLOG_INFO_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    CLOG_WARN_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("test\n", str);
    testing::internal::CaptureStdout();
    CLOG_ERROR_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("test\n", str);

    CLOG_GET(to).set_severity(clog::severity::debug, clog::severity::info);

    testing::internal::CaptureStdout();
    CLOG_DEBUG_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("test\n", str);
    testing::internal::CaptureStdout();
    CLOG_INFO_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("test\n", str);
    testing::internal::CaptureStdout();
    CLOG_WARN_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    CLOG_ERROR_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
}

TEST_F(ClogTest, Format)
{
    constexpr auto name = "Format";
    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    CLOG_(name).format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "none none none");
    std::string str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("1 0xFFFF 3.142 none none none\n", str);
}

TEST_F(ClogTest, MeasureOverheadOutput)
{
    constexpr auto name = "MeasureOverhead";
    CLOG_GET(name).set_severity(clog::severity::error);
    testing::internal::CaptureStdout();
    for (int i = 0; i < 1000; ++i) {
        CLOG_ERROR_(name) << "test";
    }
    testing::internal::GetCapturedStdout();
}

TEST_F(ClogTest, MeasureOverheadSkip)
{
    constexpr auto name = "MeasureOverhead";
    CLOG_GET(name).set_severity(clog::severity::error);
    testing::internal::CaptureStdout();
    for (int i = 0; i < 1000000; ++i) {
        CLOG_DEBUG_(name) << "test";
    }
    testing::internal::GetCapturedStdout();
}

struct test_formatter : public clog::formatter_base {
    std::string format(const clog::record& record) override
    {
        return record.message() + record.message();
    }
};

TEST_F(ClogTest, CustomFormatter)
{
    constexpr auto name = "CustomFormatter";
    CLOG_GET(name).set_formatter(test_formatter());
    testing::internal::CaptureStdout();
    CLOG_(name) << "test";
    std::string str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("testtest\n", str);
}

TEST_F(ClogTest, GetInstance)
{
    std::string str;
    std::string name = "GetInstance";
    CLOG_GET(name.c_str()).set_severity(clog::severity::warn);

    testing::internal::CaptureStdout();
    CLOG_INFO_("GetInstance") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());

    testing::internal::CaptureStdout();
    CLOG_WARN_("GetInstance") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_NE(0, str.length());
}
