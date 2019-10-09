#pragma once

#include <algorithm>
#include <iterator>

template<typename It>
auto realign(It first, It last)
{
    auto r_first{ std::make_reverse_iterator(last) };
    auto r_last{ std::make_reverse_iterator(first) };

    auto r_source_first{ std::find_if_not(r_first, r_last, [](wchar_t ch)
    {
        return ' ' == ch;
    }) };

    auto r_moved{ std::move(r_source_first, r_last, r_first) };
    std::fill(r_moved, r_last, ' ');

    return r_moved.base();
}
