#ifndef CM3D_MALLOC_HPP
#define CM3D_MALLOC_HPP

#include <cm3d/Utility/Debug.hpp>
#include <cm3d/Utility/SCstring.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>

static_assert(__SIZEOF_SIZE_T__ >= sizeof(uint32_t), "Wrong architecture!");

namespace cm3d
{
	inline void *mAlloc(size_t mSize)
	{
		if (!mSize)
			return nullptr;
		
		void *newMem = ::malloc(mSize);
		if (!newMem)
			throw std::bad_alloc();
		return newMem;
	}
	inline void *mRealloc(void *Mem, size_t newSize)
	{
		if (!newSize)
			return Mem;
		
		void *newMem = ::realloc(Mem, newSize);
		if (!newMem)
			throw std::bad_alloc();
		return newMem;
	}
	inline void *cAlloc(size_t numElem, size_t elemSize)
	{
		if (!numElem || !elemSize)
			return nullptr;
		
		void *newMem = ::calloc(numElem, elemSize);
		if (!newMem)
			throw std::bad_alloc();
		return newMem;
	}
	constexpr inline void mFree(void *Mem) noexcept {
		::free(Mem);
	}

// MSVC, LLVM for Windows, MinGW compilers
#if defined(_MSC_VER) || defined(__MINGW32__)
	inline void *mAlignedAlloc(size_t _align, size_t _size) {
		if (!_size)
			return nullptr;
		
		void *newMem = _aligned_malloc(_align, _size);
		if (!newMem)
			throw std::bad_alloc();
		return newMem;
	}
	inline void *mAlignedRealloc(void *_mem, size_t _align, size_t _newsize, size_t) {
		if (!_newsize)
			return _mem;
		
		void *newMem = _aligned_realloc(_mem, _newsize, _align);
		if (!newMem)
			throw std::bad_alloc();
		return newMem;
	}
	inline void mAlignedFree(void *_mem) noexcept {
		_aligned_free(_mem);
	}
#else
	inline void *mAlignedAlloc(size_t _align, size_t _size) {
		void *newMem = aligned_alloc(_align, _size);
		if (!newMem)
			throw std::bad_alloc();
		return newMem;
	}
	inline void *mAlignedRealloc(void *_mem, size_t _align, size_t _newsize, size_t _prevsize)
	{
		void *newMem;
		if (_newsize)
		{
			newMem = aligned_alloc(_align, _newsize);
			if (!newMem)
				throw std::bad_alloc();
			memcpy(newMem, _mem, _prevsize);
		}
		else newMem = nullptr;
		::free(_mem);
		return newMem;
	}
	inline void mAlignedFree(void *_mem) noexcept {
		::free(_mem);
	}
#endif
}

#endif
