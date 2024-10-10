#ifndef CM3D_UTILITY_STD_HPP
#define CM3D_UTILITY_STD_HPP

#include <cstddef>

// MSVC has no ssize_t
#ifndef __SIZEOF_SSIZE_T__
typedef ptrdiff_t ssize_t;
#endif

#endif // CM3D_UTILITY_STD_HPP
