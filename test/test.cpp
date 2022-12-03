#include "logu/logu.hpp"

#include "gtest/gtest.h"

#include <regex>
#include <string>
#include <vector>

// #define TEST_ENABLE_OUTPUT_TO_STDOUT

class LoguTest : public ::testing::Test {
protected:
    virtual void SetUp() { }
    virtual void TearDown() { }

    std::string GetPattern(logu::severity severity)
    {
        return GetPattern(severity, "");
    }

    std::string GetPattern(logu::severity severity, std::string name)
    {
        std::string s;
        s += "\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} | ";
        std::string severity_str = (severity == logu::severity::debug) ? "DEBUG" :
            (severity == logu::severity::info)                         ? "INFO " :
            (severity == logu::severity::warn)                         ? "WARN " :
            (severity == logu::severity::error)                        ? "ERROR" :
                                                                         "-----";
        s += "" + severity_str + " | ";
        s += "\\d+ | "; // threadid
        s += "[^@]+@\\d+ | "; // func@line
        if (!name.empty()) {
            s += "[" + name + "] "; // tagname
        }
        return s;
    }
};

TEST_F(LoguTest, Severity)
{
    std::string str;

    testing::internal::CaptureStdout();
    LOGU_DEBUG << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::debug) + "test")));

    testing::internal::CaptureStdout();
    LOGU_INFO << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::info) + "test")));

    testing::internal::CaptureStdout();
    LOGU_WARN << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::warn) + "test")));

    testing::internal::CaptureStdout();
    LOGU_ERROR << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::error) + "test")));

    testing::internal::CaptureStdout();
    LOGU << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::none) + "test")));

    testing::internal::CaptureStdout();
    LOGU_DEBUG_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::debug, "name") + "test")));

    testing::internal::CaptureStdout();
    LOGU_INFO_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::info, "name") + "test")));

    testing::internal::CaptureStdout();
    LOGU_WARN_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::warn, "name") + "test")));

    testing::internal::CaptureStdout();
    LOGU_ERROR_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::error, "name") + "test")));

    testing::internal::CaptureStdout();
    LOGU_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex(GetPattern(logu::severity::none, "name") + "test")));
}

TEST_F(LoguTest, OutputFormat)
{
    constexpr auto name = "OutputFormat";
    std::string str;

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::line, false)
                           .set_option(logu::formatter::option::tagname, false));

    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::line, false)
                           .set_option(logu::formatter::option::tagname, true));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("OutputFormat | test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, true)
                           .set_option(logu::formatter::option::func, true)
                           .set_option(logu::formatter::option::line, false)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("test\\.cpp | \\S+ | test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, true)
                           .set_option(logu::formatter::option::func, true)
                           .set_option(logu::formatter::option::line, true)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("test\\.cpp@\\d+ | [^@]+@\\d+\\ | test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, true)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::line, true)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("test\\.cpp@\\d+ | test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, true)
                           .set_option(logu::formatter::option::line, true)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("[^@]+@\\d+ | test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, true)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::line, false)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\d+ | test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, true)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::line, false)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} | test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, true)
                           .set_option(logu::formatter::option::datetime_year, false)
                           .set_option(logu::formatter::option::datetime_microsecond, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::line, false)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} | test")));

    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, true)
                           .set_option(logu::formatter::option::datetime_year, false)
                           .set_option(logu::formatter::option::datetime_microsecond, true)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::line, false)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_search(str, std::regex("\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{6} | test")));
}

TEST_F(LoguTest, CopyLogger)
{
    // constexpr auto from = "CopyLoggerFrom";
    constexpr auto to = "CopyLoggerTo";
    std::string str;

    LOGU_GET_DEFAULT()
        .set_severity(logu::severity::warn, logu::severity::error)
        .set_enable(false)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::tagname, false));
    LOGU_GET(to).copy_from(LOGU_GET_DEFAULT());

    testing::internal::CaptureStdout();
    LOGU_DEBUG_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    LOGU_INFO_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    LOGU_WARN_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    LOGU_ERROR_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());

    LOGU_GET(to).set_enable(true);

    testing::internal::CaptureStdout();
    LOGU_DEBUG_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    LOGU_INFO_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    LOGU_WARN_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("test\n", str);
    testing::internal::CaptureStdout();
    LOGU_ERROR_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("test\n", str);

    LOGU_GET(to).set_severity(logu::severity::debug, logu::severity::info);

    testing::internal::CaptureStdout();
    LOGU_DEBUG_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("test\n", str);
    testing::internal::CaptureStdout();
    LOGU_INFO_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("test\n", str);
    testing::internal::CaptureStdout();
    LOGU_WARN_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
    testing::internal::CaptureStdout();
    LOGU_ERROR_(to) << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());
}

TEST_F(LoguTest, Format)
{
    constexpr auto name = "Format";
    LOGU_GET(name)
        .set_formatter(logu::formatter()
                           .set_option(logu::formatter::option::datetime, false)
                           .set_option(logu::formatter::option::severity, false)
                           .set_option(logu::formatter::option::threadid, false)
                           .set_option(logu::formatter::option::file, false)
                           .set_option(logu::formatter::option::func, false)
                           .set_option(logu::formatter::option::tagname, false));
    testing::internal::CaptureStdout();
    LOGU_(name).format("%d 0x%04X %.3f %s", 1, 0xFFFFu, 3.141592653589793, "none none none");
    std::string str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("1 0xFFFF 3.142 none none none\n", str);
}

TEST_F(LoguTest, MeasureOverheadOutput)
{
    constexpr auto name = "MeasureOverhead";
    LOGU_GET(name).set_severity(logu::severity::error);
    testing::internal::CaptureStdout();
    for (int i = 0; i < 1000; ++i) {
        LOGU_ERROR_(name) << "test";
    }
    testing::internal::GetCapturedStdout();
}

TEST_F(LoguTest, MeasureOverheadSkip)
{
    constexpr auto name = "MeasureOverhead";
    LOGU_GET(name).set_severity(logu::severity::error);
    testing::internal::CaptureStdout();
    for (int i = 0; i < 1000000; ++i) {
        LOGU_DEBUG_(name) << "test";
    }
    testing::internal::GetCapturedStdout();
}

struct test_formatter : public logu::formatter_base {
    std::string format(const logu::record& record) override
    {
        return record.message() + record.message();
    }
};

TEST_F(LoguTest, CustomFormatter)
{
    constexpr auto name = "CustomFormatter";
    LOGU_GET(name).set_formatter(test_formatter());
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    std::string str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("testtest\n", str);
}

TEST_F(LoguTest, GetInstance)
{
    std::string str;
    std::string name = "GetInstance";
    LOGU_GET(name.c_str()).set_severity(logu::severity::warn);

    testing::internal::CaptureStdout();
    LOGU_INFO_("GetInstance") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());

    testing::internal::CaptureStdout();
    LOGU_WARN_("GetInstance") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_NE(0, str.length());
}

TEST_F(LoguTest, Print)
{
    LOGU_GET_DEFAULT()
        .set_enable(true)
        .set_severity(logu::severity::debug)
        .set_formatter(
            logu::formatter()
                .set_option(logu::formatter::option::datetime, false)
                .set_option(logu::formatter::option::severity, true)
                .set_option(logu::formatter::option::threadid, false)
                .set_option(logu::formatter::option::file, false)
                .set_option(logu::formatter::option::func, false)
                .set_option(logu::formatter::option::line, false)
                .set_option(logu::formatter::option::tagname, false));
    std::string expect_base = "(v1, v2) -> (123, abc) \n";
    std::string expect;
    std::string str;
    int32_t v1 = 123;
    std::string v2 = "abc";

    testing::internal::CaptureStdout();
    LOGU_PRINT(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "----- | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());

    testing::internal::CaptureStdout();
    LOGU_PRINT_DEBUG(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "DEBUG | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());

    testing::internal::CaptureStdout();
    LOGU_PRINT_INFO(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "INFO  | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());

    testing::internal::CaptureStdout();
    LOGU_PRINT_WARN(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "WARN  | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());

    testing::internal::CaptureStdout();
    LOGU_PRINT_ERROR(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "ERROR | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());
}
