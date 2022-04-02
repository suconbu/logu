#include "clog/clog.hpp"
#include "gtest/gtest.h"

#include <regex>
#include <string>
#include <vector>

class ClogTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        CLOG_GET_DEFAULT().set_handler([&](const clog::record& record, const char* str) {
            output = str;
        });
    }
    virtual void TearDown() { }

    std::string PatternString(clog::severity severity)
    {
        return PatternString(severity, "");
    }

    std::string PatternString(clog::severity severity, std::string name)
    {
        std::string s;
        s += "\\[\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\] ";
        std::string severity_str = (severity == clog::severity::debug) ? "DEBUG" :
            (severity == clog::severity::info)                         ? "INFO " :
            (severity == clog::severity::warn)                         ? "WARN " :
            (severity == clog::severity::error)                        ? "ERROR" :
                                                                         "-----";
        s += "\\[" + severity_str + "\\] ";
        s += "\\[\\d+\\] ";
        s += "\\[\\w+::TestBody@\\d+\\] ";
        if (!name.empty()) {
            s += "\\[" + name + "\\] ";
        }
        return s;
    }

    std::string output;
};

TEST_F(ClogTest, Severity)
{
    output.clear();
    CLOG_DEBUG << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::debug) + "test")));

    output.clear();
    CLOG_INFO << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::info) + "test")));

    output.clear();
    CLOG_WARN << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::warn) + "test")));

    output.clear();
    CLOG_ERROR << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::error) + "test")));

    output.clear();
    CLOG << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::none) + "test")));

    CLOG_GET("name").set_handler([&](const clog::record& record, const char* str) {
        output = str;
    });

    output.clear();
    CLOG_DEBUG_("name") << "test";
    auto ss = PatternString(clog::severity::debug, "name") + "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::debug, "name") + "test")));

    output.clear();
    CLOG_INFO_("name") << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::info, "name") + "test")));

    output.clear();
    CLOG_WARN_("name") << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::warn, "name") + "test")));

    output.clear();
    CLOG_ERROR_("name") << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::error, "name") + "test")));

    output.clear();
    CLOG_("name") << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::none, "name") + "test")));
}

TEST_F(ClogTest, Condition)
{
    constexpr bool bfalse = false;
    constexpr bool btrue = true;

    // true

    output.clear();
    CLOG_DEBUG_IF(btrue) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::debug) + "test")));

    output.clear();
    CLOG_INFO_IF(btrue) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::info) + "test")));

    output.clear();
    CLOG_WARN_IF(btrue) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::warn) + "test")));

    output.clear();
    CLOG_ERROR_IF(btrue) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::error) + "test")));

    output.clear();
    CLOG_IF(btrue) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex(PatternString(clog::severity::none) + "test")));

    // false

    output.clear();
    CLOG_DEBUG_IF(bfalse) << "test";
    EXPECT_EQ(0, output.length());

    output.clear();
    CLOG_INFO_IF(bfalse) << "test";
    EXPECT_EQ(0, output.length());

    output.clear();
    CLOG_WARN_IF(bfalse) << "test";
    EXPECT_EQ(0, output.length());

    output.clear();
    CLOG_ERROR_IF(bfalse) << "test";
    EXPECT_EQ(0, output.length());

    output.clear();
    CLOG_IF(bfalse) << "test";
    EXPECT_EQ(0, output.length());
}

TEST_F(ClogTest, OutputFormat)
{
    constexpr auto name = "OutputFormat";
    CLOG_GET(name)
        .set_handler([&](const clog::record& record, const char* str) {
            output = str;
        });

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, true));
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[OutputFormat\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, true)
                           .set_option(clog::formatter::option::func, true)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[test\\.cpp\\] \\[\\w+::TestBody\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, true)
                           .set_option(clog::formatter::option::func, true)
                           .set_option(clog::formatter::option::line, true)
                           .set_option(clog::formatter::option::tagname, false));
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[test\\.cpp@\\d+\\] \\[\\w+::TestBody@\\d+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, true)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, true)
                           .set_option(clog::formatter::option::tagname, false));
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[test\\.cpp@\\d+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, true)
                           .set_option(clog::formatter::option::line, true)
                           .set_option(clog::formatter::option::tagname, false));
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[\\w+::TestBody@\\d+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, false)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, true)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[\\d+\\] test")));

    CLOG_GET(name)
        .set_formatter(clog::formatter()
                           .set_option(clog::formatter::option::datetime, true)
                           .set_option(clog::formatter::option::severity, false)
                           .set_option(clog::formatter::option::threadid, false)
                           .set_option(clog::formatter::option::file, false)
                           .set_option(clog::formatter::option::func, false)
                           .set_option(clog::formatter::option::line, false)
                           .set_option(clog::formatter::option::tagname, false));
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\] test")));

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
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\] test")));

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
    output.clear();
    CLOG_(name) << "test";
    EXPECT_TRUE(std::regex_search(output, std::regex("\\[\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{6}\\] test")));
}

TEST_F(ClogTest, MeasureOverheadOutput1000times)
{
    constexpr auto name = "MeasureOverhead";
    CLOG_GET(name)
        .set_handler([&](const clog::record& record, const char* str) {
            output = str;
        })
        .set_severity(clog::severity::error);
    for (int i = 0; i < 1000; ++i) {
        CLOG_ERROR_(name) << "test";
    }
}

TEST_F(ClogTest, MeasureOverheadSkip1000000times)
{
    constexpr auto name = "MeasureOverhead";
    CLOG_GET(name)
        .set_handler([&](const clog::record& record, const char* str) {
            output = str;
        })
        .set_severity(clog::severity::error);
    for (int i = 0; i < 1000000; ++i) {
        CLOG_DEBUG_(name) << "test";
    }
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
    CLOG_GET(name)
        .set_handler([&](const clog::record& record, const char* str) {
            output = str;
        })
        .set_formatter(test_formatter());
    CLOG_(name) << "test";
    EXPECT_EQ("testtest", output);
}
