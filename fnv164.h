#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

typedef uint64_t HashType64;

template <HashType64 V>
constexpr HashType64 calculate_string_hash()
{
    return V;
}

template <HashType64 V, char C, char... Cs>
constexpr HashType64 calculate_string_hash()
{
    return calculate_string_hash < (V * 1099511628211ULL) ^ static_cast<size_t>(C), Cs... > ();
}

template <size_t... N, typename Literal>
constexpr HashType64 templatize_string_to_chars(std::index_sequence<N...>, Literal)
{
    return calculate_string_hash<14695981039346656037ULL, Literal::get(N)...>();
}

template <typename T, T... Cs>
constexpr HashType64 operator""_tstr()
{
    return templatize_string_to_chars(std::make_index_sequence<sizeof...(Cs)>(), []() {
        struct Literal
        {
            static constexpr char get(int N)
            {
                static constexpr char str[] = {Cs..., '\0'};
                return str[N];
            }
        };
        return Literal{};
    }());
}