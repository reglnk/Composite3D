#ifndef CM3D_UTILITY_DEBUG_HPP
#define CM3D_UTILITY_DEBUG_HPP

#ifdef _DEBUG
#  define CM3D_DEBUG 1
#else
#  undef CM3D_DEBUG
#endif

#ifdef CM3D_DEBUG
#  include <cassert>
#  define CM3D_ASSERT(...) assert(__VA_ARGS__)
#else
#  define CM3D_ASSERT(...)
#endif

#endif
