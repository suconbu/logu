#include "logu/logu.hpp"

#include "gtest/gtest.h"

#include <regex>
#include <string>
#include <vector>

// #define TEST_ENABLE_OUTPUT_TO_STDOUT

logu::logger g_default_logger("");

class LoguTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        LOGU_DEFAULT_LOGGER().copy_from(g_default_logger);
    }
    virtual void TearDown() { }

    std::string GetPattern(logu::severity severity)
    {
        return GetPattern(severity, "");
    }

    std::string GetPattern(logu::severity severity, std::string name)
    {
        std::string s;
        s += "\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\| ";
        std::string severity_str = (severity == logu::severity::debug) ? "DEBUG" :
            (severity == logu::severity::info)                         ? "INFO " :
            (severity == logu::severity::warn)                         ? "WARN " :
            (severity == logu::severity::error)                        ? "ERROR" :
                                                                         "-----";
        s += "" + severity_str + " \\| ";
        s += "\\d+ \\| "; // threadid
        s += "[^@]+@\\d+ \\| "; // func@line
        if (!name.empty()) {
            s += "\\[" + name + "\\] "; // tagname
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
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::debug) + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU_INFO << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::info) + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU_WARN << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::warn) + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU_ERROR << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::error) + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::none) + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU_DEBUG_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::debug, "name") + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU_INFO_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::info, "name") + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU_WARN_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::warn, "name") + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU_ERROR_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::error, "name") + "test\\n")));

    testing::internal::CaptureStdout();
    LOGU_("name") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(GetPattern(logu::severity::none, "name") + "test\\n")));
}

TEST_F(LoguTest, OutputFormat)
{
    constexpr auto name = "OutputFormat";
    std::string str;

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("\\[OutputFormat\\] test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("test\\.cpp \\| \\S+ \\| test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("test\\.cpp@\\d+ \\| [^@]+@\\d+\\ \\| test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("test\\.cpp@\\d+ \\| test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("[^@]+@\\d+ \\| test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("\\d+ \\| test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\| test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3} \\| test\\n")));

    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    EXPECT_TRUE(std::regex_match(str, std::regex("\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{6} \\| test\\n")));
}

TEST_F(LoguTest, CopyLogger)
{
    // constexpr auto from = "CopyLoggerFrom";
    constexpr auto to = "CopyLoggerTo";
    std::string str;

    LOGU_DEFAULT_LOGGER()
        .set_severity(logu::severity::warn, logu::severity::error)
        .set_enable(false)
        .set_formatter(
            logu::formatter()
                .set_option(logu::formatter::option::datetime, false)
                .set_option(logu::formatter::option::severity, false)
                .set_option(logu::formatter::option::threadid, false)
                .set_option(logu::formatter::option::file, false)
                .set_option(logu::formatter::option::func, false)
                .set_option(logu::formatter::option::tagname, false));
    LOGU_LOGGER(to).copy_from(LOGU_DEFAULT_LOGGER());

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

    LOGU_LOGGER(to).set_enable(true);

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

    LOGU_LOGGER(to).set_severity(logu::severity::debug, logu::severity::info);

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
    LOGU_LOGGER(name)
        .set_formatter(
            logu::formatter()
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
    LOGU_LOGGER(name).set_severity(logu::severity::error);
    testing::internal::CaptureStdout();
    for (int i = 0; i < 1000; ++i) {
        LOGU_ERROR_(name) << "test";
    }
    testing::internal::GetCapturedStdout();
}

TEST_F(LoguTest, MeasureOverheadSkip)
{
    constexpr auto name = "MeasureOverhead";
    LOGU_LOGGER(name).set_severity(logu::severity::error);
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
    LOGU_LOGGER(name).set_formatter(test_formatter());
    testing::internal::CaptureStdout();
    LOGU_(name) << "test";
    std::string str = testing::internal::GetCapturedStdout();
    EXPECT_EQ("testtest\n", str);
}

TEST_F(LoguTest, GetInstance)
{
    std::string str;
    std::string name = "GetInstance";
    LOGU_LOGGER(name.c_str()).set_severity(logu::severity::warn);

    testing::internal::CaptureStdout();
    LOGU_INFO_("GetInstance") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_EQ(0, str.length());

    testing::internal::CaptureStdout();
    LOGU_WARN_("GetInstance") << "test";
    str = testing::internal::GetCapturedStdout();
    EXPECT_NE(0, str.length());
}

TEST_F(LoguTest, Var)
{
    LOGU_DEFAULT_LOGGER()
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
    LOGU << LOGU_VARS(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "----- | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());

    testing::internal::CaptureStdout();
    LOGU_DEBUG << LOGU_VARS(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "DEBUG | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());

    testing::internal::CaptureStdout();
    LOGU_INFO << LOGU_VARS(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "INFO  | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());

    testing::internal::CaptureStdout();
    LOGU_WARN << LOGU_VARS(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "WARN  | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());

    testing::internal::CaptureStdout();
    LOGU_ERROR << LOGU_VARS(v1, v2);
    str = testing::internal::GetCapturedStdout();
    expect = "ERROR | " + expect_base;
    EXPECT_STREQ(expect.c_str(), str.c_str());
}

TEST_F(LoguTest, InheritDefault)
{
    std::string str;
    const auto get_pattern = [](logu::severity severity, const char* tagname) {
        std::string pattern;
        std::string severity_str = (severity == logu::severity::debug) ? "DEBUG" :
            (severity == logu::severity::info)                         ? "INFO " :
            (severity == logu::severity::warn)                         ? "WARN " :
            (severity == logu::severity::error)                        ? "ERROR" :
                                                                         "-----";
        pattern += "" + severity_str + " \\| ";
        if (*tagname != 0) {
            pattern += "\\[";
            pattern += tagname;
            pattern += "\\] "; // tagname
        }
        return pattern;
    };

    LOGU_DEFAULT_LOGGER()
        .set_enable(true)
        .set_severity(logu::severity::info)
        .set_formatter(
            logu::formatter()
                .set_option(logu::formatter::option::datetime, false)
                .set_option(logu::formatter::option::severity, true)
                .set_option(logu::formatter::option::threadid, false)
                .set_option(logu::formatter::option::file, false)
                .set_option(logu::formatter::option::func, false)
                .set_option(logu::formatter::option::line, false)
                .set_option(logu::formatter::option::tagname, true));

    // Using logger of inherited default logger

    testing::internal::CaptureStdout();
    LOGU_DEBUG_("inherit") << "DEBUG";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(str.empty());

    testing::internal::CaptureStdout();
    LOGU_INFO_("inherit") << "INFO";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(get_pattern(logu::severity::info, "inherit") + "INFO\\n")));

    // Override severity
    LOGU_LOGGER("inherit")
        .set_severity(logu::severity::debug);

    testing::internal::CaptureStdout();
    LOGU_DEBUG_("inherit") << "DEBUG";
    str = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(std::regex_match(str, std::regex(get_pattern(logu::severity::debug, "inherit") + "DEBUG\\n")));
}
