#ifndef CM3D_UTILITY_HASH_HPP
#define CM3D_UTILITY_HASH_HPP

#include <cm3d/Utility/Debug.hpp>
#include <cstdint>
#include <cstddef>

namespace cm3d
{
	namespace Hash
	{
		inline uint32_t u32(const uint32_t seed) noexcept
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
		inline uint32_t u32(const void *_arr, const size_t _len) noexcept
		{
			const size_t times = _len >> 2; // / sizeof(uint32_t)
			auto i_arr = reinterpret_cast<const uint32_t *>(_arr);
			auto c_arr = reinterpret_cast<const uint8_t *>(_arr);

			uint32_t h = 0;
			for (size_t i = 0; i < times; ++i)
				h ^= cm3d::Hash::u32(i_arr[i] + i);

			uint32_t l = 0;
			for (size_t i = times << 2; i < _len; ++i) {
				l ^= c_arr[i] << (8 * (_len - i));
			}
			CM3D_ASSERT((l & 0xFF) == 0);

			// An array of length s=sizeof(uint32_t)*N+2 ended with {a, b}
			// must have different hash than one of s+1 bytes ended with {a, b, 0}
			l ^= _len - (times << 2);
			h ^= cm3d::Hash::u32(l);

			return h;
		}
	}
}

#endif
