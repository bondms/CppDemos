#include "../app/shuffle.h"

#include <gtest/gtest.h>

#include <string>

namespace
{
    class ShuffleTestFixture :
        public ::testing::Test
    {
    };

    template<typename Shuffler>
    void test(Shuffler & shuffler)
    {
        auto max = shuffler.max();
        std::set<int> s{};
        for(auto i = 0 ; i < (max * 2 + max / 2 + 1) ; ++i)
        {
            if ( 0 == i % (max + 1) )
            {
                s.clear();
            }
            EXPECT_TRUE(s.insert(shuffler()).second);
        }
    }
} // namespace

TEST_F(ShuffleTestFixture, Simple_ShuffleMillion)
{
    Shuffle::Simple<int> simple{999'999};
    test(simple);
}

TEST_F(ShuffleTestFixture, LowMem_ShuffleMillion)
{
    Shuffle::LowMem<int, 999'999> lowMem{};
    test(lowMem);
}

TEST_F(ShuffleTestFixture, LowMem_FullRangeOfType)
{
    Shuffle::LowMem<std::uint8_t, std::numeric_limits<std::uint8_t>::max()> lowMem{};
    test(lowMem);
}
