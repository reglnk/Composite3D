#include <Core/Malloc.hpp>
#include <new>

#ifndef CM3D_ALLOC_MAX_FAILS
#  define CM3D_ALLOC_MAX_FAILS 8
#endif

namespace cm3d
{
	void *mAlloc(size_t mSize)
	{
		unsigned failCount(0);
		void *newMem;
		while (true)
		{
			newMem = malloc(mSize);
			if (!newMem) {
				++failCount;
				if (failCount > CM3D_ALLOC_MAX_FAILS)
					throw std::bad_alloc();
				continue;
			}
			return newMem;
		}
	}
	void *mRealloc(void *Mem, size_t newSize)
	{
		unsigned failCount(0);
		void *newMem;
		while (true)
		{
			newMem = realloc(Mem, newSize);
			if (!newMem) {
				++failCount;
				if (failCount > CM3D_ALLOC_MAX_FAILS)
					throw std::bad_alloc();
				continue;
			}
			return newMem;
		}
	}
	void *cAlloc(size_t numElem, size_t elemSize)
	{
		unsigned failCount(0);
		void *newMem;
		while (true)
		{
			newMem = calloc(numElem, elemSize);
			if (!newMem) {
				++failCount;
				if (failCount > CM3D_ALLOC_MAX_FAILS)
					throw std::bad_alloc();
				continue;
			}
			return newMem;
		}
	}
}
