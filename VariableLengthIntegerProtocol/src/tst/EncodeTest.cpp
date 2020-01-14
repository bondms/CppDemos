#include "../app/Encode.h"

#include <gtest/gtest.h>

namespace
{
    struct Parameters
    {
        size_t input;
        std::vector<char> expected;
    };

    class EncodeTestFixture :
        public testing::TestWithParam<Parameters>
    {
    };
} // namespace

TEST_P(EncodeTestFixture, Simple)
{
    auto param{ GetParam() };
    EXPECT_EQ(param.expected, Encode(param.input));
}

Parameters parameterValues[] = {
    Parameters(size_t{0}, std::vector<char>{}),
    Parameters(size_t{1}, std::vector<char>{1}),
    Parameters(size_t{2}, std::vector<char>{2}),
    Parameters(size_t{0xFF}, std::vector<char>{0xFF}),
    Parameters(size_t{0x100}, std::vector<char>{0x1, 0x0}),
    Parameters(size_t{0x101}, std::vector<char>{0x1, 0x1}),
    Parameters(size_t{0xFE0087}, std::vector<char>{0xFE, 0x00, 0x87})
};

INSTANTIATE_TEST_SUITE_P(EncodeTestInstantiation,
                         EncodeTestFixture,
                         testing::ValuesIn(parameterValues));
