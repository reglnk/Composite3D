// Unbans memcpy, strcpy and other stuff from Microsoft Secure Development Lifecycle
// :)

#ifndef CM3D_UTILITY_SECURE_C_STRING_HPP
#define CM3D_UTILITY_SECURE_C_STRING_HPP

#include <climits>
#include <cstring>
#include <cstddef>

namespace cm3d
{
	inline void *memcpy(void *dest, const void *src, size_t n) {
#	ifndef _WIN32
		return ::memcpy(dest, src, n);
#	else
		::memcpy_s(dest, SIZE_MAX - 1, src, n);
		return dest;
#	endif
	}
	inline void *memmove(void *dest, const void *src, size_t n) {
#	ifndef _WIN32
		return ::memmove(dest, src, n);
#	else
		::memmove_s(dest, SIZE_MAX - 1, src, n);
		return dest;
#	endif
	}
	inline char *strcpy(char *dest, const char *src) {
#	ifndef _WIN32
		return ::strcpy(dest, src);
#	else
		::strcpy_s(dest, SIZE_MAX - 1, src);
		return dest;
#	endif
	}
	inline char *strncpy(char *dest, const char *src, size_t n) {
#	ifndef _WIN32
		return ::strncpy(dest, src, n);
#	else
		::strncpy_s(dest, SIZE_MAX - 1, src, n);
		return dest;
#	endif
	}
}

#endif // CM3D_UTILITY_SECURE_C_STRING_HPP