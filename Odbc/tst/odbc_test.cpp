#include "lib/odbc.h"

#include <gmock/gmock.h>

#include <exception>

using namespace Odbc;

TEST(OdbcTest, OdbcExceptionIsStdException)
{
    try
    {
        throw Exception(SQL_ERROR, "TestContext");
    }
    catch ( const std::exception & )
    {
    }
}

TEST(OdbcTest, ExceptionDetails)
{
    try
    {
        throw Exception(SQL_ERROR, "TestContext");
    }
    catch ( const Exception & e )
    {
        EXPECT_STREQ("TestContext: -1", e.what());
        EXPECT_EQ(SQL_ERROR, e.SqlReturn());
    }
}

TEST(OdbcTest, allocEnvHandle_ErrorCodeVersion)
{
    SQLRETURN sqlReturn{};
    auto h{ allocEnvHandle(sqlReturn) };
    EXPECT_TRUE(SQL_SUCCEEDED(sqlReturn));
    EXPECT_TRUE(h.isValid());
}

TEST(OdbcTest, allocEnvHandle_ExceptionVersion)
{
    auto h{ allocEnvHandle() };
    EXPECT_TRUE(h.isValid());
}
