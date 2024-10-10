#ifndef CM3D_UTILITY_DEBUG_HPP
#define CM3D_UTILITY_DEBUG_HPP

#ifdef CM3D_DEBUG
#  include <cassert>
#  ifdef NDEBUG
#    error assertions cannot be enabled
#  endif
#  define CM3D_ASSERT(...) assert(__VA_ARGS__)
#else
#  define CM3D_ASSERT(...)
#endif

#endif
