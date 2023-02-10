#ifndef CM3D_MEMORY_HPP
#define CM3D_MEMORY_HPP

#include <Utility/Std.hpp>

#include <cstddef>
#include <malloc.h>

namespace cm3d
{
	void *mAlloc(size_t mSize);
	void *mRealloc(void *Mem, size_t newSize);
	void *cAlloc(size_t numElem, size_t elemSize);
	
	CM3D_CXX14_CONSTEXPR_INLINE void mFree(void *Mem) noexcept {
		::free(Mem);
	}
}

#endif