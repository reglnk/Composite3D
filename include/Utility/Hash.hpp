#ifndef CM3D_UTILITY_HASH_HPP
#define CM3D_UTILITY_HASH_HPP

#include <cstdint>

namespace cm3d
{
    namespace Hash
    {
        inline uint32_t u32(const uint32_t seed)
        {
            uint32_t num = seed;
            num *= 77777777;
            num ^= 0xd;;
            num &= 0x1F;

            uint32_t mun = 32 - num;
            num = ((seed ^ 0xab7bc843) >> (num + 1)) + ((seed ^ 0x348cb7ba) << mun);

            num += 171 * (seed ^ num) + 31836;
            num += 119 * (seed + num) ^ 84792;
            num += 131 * (seed ^ num) + 52048;

            return num;
        }
    }
}

#endif
