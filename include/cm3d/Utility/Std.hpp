#ifndef CM3D_UTILITY_STD_HPP
#define CM3D_UTILITY_STD_HPP

#ifndef CM3D_CXX14_CONSTEXPR_INLINE
#  if __cplusplus >= 201402
#    define CM3D_CXX14_CONSTEXPR_INLINE constexpr inline
#  else
#    define CM3D_CXX14_CONSTEXPR_INLINE inline
#  endif
#endif

#include <cstddef>

// MSVC has no ssize_t
#ifndef __SIZEOF_SSIZE_T__
typedef ptrdiff_t ssize_t;
#endif

#endif // CM3D_UTILITY_STD_HPP
