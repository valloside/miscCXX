#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <span>
#include <string_view>

namespace constexpr_hash
{
    class MD5
    {
    public:
        template <size_t SIZE>
        static constexpr std::array<uint32_t, 4> md5Hash(const char (&input)[SIZE])
        {
            return md5Hash({input, SIZE - 1});
        }

        static constexpr std::array<uint32_t, 4> md5Hash(std::string_view input)
        {
            std::vector paddedData = paddingData(input);
            return processing(paddedData);
        }

#ifdef __SIZEOF_INT128__
        template <size_t SIZE>
        static constexpr __uint128_t md5HashAsUint128(const char (&input)[SIZE])
        {
            return md5HashAsUint128({input, SIZE - 1});
        }

        static constexpr __uint128_t md5HashAsUint128(std::string_view input)
        {
            auto        md5Val = md5Hash(input);
            __uint128_t result = 0;
            for (int i = 0; i < 4; i++)
            {
                result |= (__uint128_t)(((md5Val[i] & 0x000000FF) << 24)
                                        | ((md5Val[i] & 0x0000FF00) << 8)
                                        | ((md5Val[i] & 0x00FF0000) >> 8)
                                        | ((md5Val[i] & 0xFF000000) >> 24))
                       << ((3 - i) * 32);
            }
            return result;
        }
#endif

    private:
        static constexpr uint32_t leftRotate(uint32_t value, uint32_t shift)
        {
            return (value << shift) | (value >> (32 - shift));
        }

        static constexpr std::vector<uint32_t> paddingData(std::string_view input)
        {
            const size_t         size = input.size();
            const size_t         RET_BYTE_LENGTH = ((size + 8) / 64 + 1) * 64;
            std::vector<uint8_t> buffer(RET_BYTE_LENGTH, 0);
            for (size_t i = 0; i < size + 1; ++i)
            {
                buffer[i] = input.data()[i];
            }
            buffer[size] = 0x80;

            size_t bit_len = size * 8;
            for (size_t i = 0; i < 8; ++i)
            {
                buffer[RET_BYTE_LENGTH - 8 + i] = (bit_len >> (i * 8)) & 0xFF;
            }

            std::vector<uint32_t> ret(RET_BYTE_LENGTH / 4, 0);
            for (size_t i = 0; i < RET_BYTE_LENGTH; i += 4)
            {
                ret[i / 4] = buffer[i] | (buffer[i + 1] << 8) | (buffer[i + 2] << 16) | (buffer[i + 3] << 24);
            }

            return ret;
        }

        static constexpr std::array<uint32_t, 4> processing(std::span<uint32_t> data)
        {
            const size_t SIZE = data.size();

            const std::array<uint32_t, 64> r =
                {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
            const std::array<uint32_t, 64> k =
                {3614090360, 3905402710, 606105819, 3250441966, 4118548399, 1200080426, 2821735955, 4249261313,
                 1770035416, 2336552879, 4294925233, 2304563134, 1804603682, 4254626195, 2792965006, 1236535329,
                 4129170786, 3225465664, 643717713, 3921069994, 3593408605, 38016083, 3634488961, 3889429448,
                 568446438, 3275163606, 4107603335, 1163531501, 2850285829, 4243563512, 1735328473, 2368359562,
                 4294588738, 2272392833, 1839030562, 4259657740, 2763975236, 1272893353, 4139469664, 3200236656,
                 681279174, 3936430074, 3572445317, 76029189, 3654602809, 3873151461, 530742520, 3299628645,
                 4096336452, 1126891415, 2878612391, 4237533241, 1700485571, 2399980690, 4293915773, 2240044497,
                 1873313359, 4264355552, 2734768916, 1309151649, 4149444226, 3174756917, 718787259, 3951481745};
            std::array<uint32_t, 4> initVal = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};
            for (size_t block = 0; block < SIZE / 16; ++block)
            {
                uint32_t a = initVal[0], b = initVal[1], c = initVal[2], d = initVal[3];
                for (size_t i = 0; i < 64; ++i)
                {
                    uint32_t f, g;
                    if (i < 16)
                    {
                        f = (b & c) | (~b & d);
                        g = i;
                    }
                    else if (i < 32)
                    {
                        f = (d & b) | (~d & c);
                        g = (5 * i + 1) % 16;
                    }
                    else if (i < 48)
                    {
                        f = b ^ c ^ d;
                        g = (3 * i + 5) % 16;
                    }
                    else
                    {
                        f = c ^ (b | ~d);
                        g = (7 * i) % 16;
                    }
                    uint32_t temp = d;
                    d = c;
                    c = b;
                    b += leftRotate(a + f + k[i] + data[g + block * 16], r[i]);
                    a = temp;
                }
                initVal[0] += a;
                initVal[1] += b;
                initVal[2] += c;
                initVal[3] += d;
            }
            return {initVal[0], initVal[1], initVal[2], initVal[3]};
        }
    };

    namespace hash_literals
    {
        constexpr std::array<uint32_t, 4> operator""_MD5(const char *str, size_t size)
        {
            return MD5::md5Hash({str, size});
        }

#ifdef __SIZEOF_INT128__
        constexpr __uint128_t operator""_MD5_128(const char *str, size_t size)
        {
            return MD5::md5HashAsUint128({str, size});
        }
#endif

    } // namespace hash_literals

} // namespace constexpr_hash

/*

void usage()
{
    constexpr std::array<uint32_t, 4> md5_result = md5Hash("hello");

    // if your compiler support __uint128_t
    constexpr __uint128_t md5_result_128 = md5HashAsUint128("hello");

    using namespace constexpr_hash::hash_literals;
    constexpr auto md5_result_128_2 = "hello"_MD5_128;

    std::print("{:X}", md5_result_128); // 5D41402ABC4B2A76B9719D911017C592
}

*/
