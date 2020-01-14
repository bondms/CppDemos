#include "Encode.h"

// TODO(MarkBond): Just for testing.
#include <iomanip>
#include <iostream>

#include <climits>

static_assert(0 == (CHAR_BIT % 8));

namespace
{
    template<typename T>
    auto shift_left_byte(T& v)
    {
        constexpr auto bits_in_t{ sizeof(T) * CHAR_BIT };
        uint8_t result = v >> (bits_in_t - 8);
        v = v << 8;
        return result;
    }
} // namespace

std::vector<uint8_t> Encode(size_t input)
{
    std::vector<uint8_t> result{};

    if ( 0 == input )
    {
        result.push_back(0);
        return result;
    }

    uint8_t currentByte{ 0 };

    while ( (0 == currentByte) && (0 != input) )
    {
        currentByte = shift_left_byte(input);
    }

    while ( (0 != currentByte) || (0 != input) )
    {
        result.push_back(currentByte);
        currentByte = shift_left_byte(input);
    }

    return result;
}
