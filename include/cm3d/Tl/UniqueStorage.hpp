#ifndef CM3D_TYPE_UNIQUE_STORAGE_HPP
#define CM3D_TYPE_UNIQUE_STORAGE_HPP

#include <cm3d/Types/DynArray.hpp>
#include <cm3d/Types/Stack.hpp>
#include <cm3d/Core/Object.hpp>

#include <cstdint>
#include <cstring>

// The container that maps items to IDs. Intended for usage with cm3d::Object
// @todo iterators
// @todo specialize by T = Object and sort objects by position in their physics modules (if any)

namespace cm3d
{
	// Stores elements by IDs
	template<typename T>
	class UniqueStorage
	{
	protected:
		DynArray<T> mArr;
		// contains info about existence of each element of previous array
		DynArray<bool> mActive;
		// IDs of elements being deleted
		Stack<uint32_t> mFreeIds;
		
	public:
		using valueType = T;

		inline UniqueStorage() noexcept: mArr() {}
		
		// @todo test
		inline UniqueStorage(const UniqueStorage<T> &other): mFreeIds()
		{
			mArr = other.mArr;
			mActive = other.mActive;
			
			// nothing happens if the stack turns upside down,
			// the same IDs should be reused
			for (const auto &el: other.mFreeIds)
				mFreeIds.push(el);
		}

		// @todo test
		inline UniqueStorage(UniqueStorage<T> &&rv) noexcept {
			mArr = std::move(rv.mArr);
			mActive = std::move(rv.mActive);
			mFreeIds = std::move(rv.mFreeIds);
		}

		// @todo test
		inline UniqueStorage(std::initializer_list<T> _lst):
			mArr(_lst), mActive(mArr.length()), mFreeIds()
		{ std::memset(mActive.data(), 1, mArr.size()); }
		
		inline UniqueStorage<T> &operator=(const UniqueStorage<T> &other)
		{
			clear();
			mArr = other.mArr;
			mActive = other.mActive;
			
			for (const auto &el: other.mFreeIds)
				mFreeIds.push(el);
		}
		inline UniqueStorage<T> &operator=(UniqueStorage<T> &&rv) noexcept
		{
			mArr = std::move(rv.mArr);
			mActive = std::move(rv.mActive);
			mFreeIds = std::move(rv.mFreeIds);
		}

		CM3D_CXX14_CONSTEXPR_INLINE const T &operator[](const size_t id) const
		{
			CM3D_ASSERT(mActive[id]);
			return mArr[id];
		}

		// current array size, including free space after deleted objects
		inline size_t size() const { return mArr.length(); }
		
		// the quantity of actual stored objects
		inline size_t pureSize() const { return mArr.length() - mFreeIds.length(); }

		inline void clear() noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			mArr.clear();
			mActive.clear();
			mFreeIds.clear();
		}
		
		inline void push(T &&elem)
		{
			while (mFreeIds)
			{
				uint32_t id = mFreeIds.back();
				mFreeIds.pop();
				
				// erase() may have invalidated some freed IDs. Check them here
				if (id >= mActive.size() || mActive[id])
					continue;
				
				mArr[id] = elem;
				mActive[id] = true;
				return;
			}
			mArr.pushBack(elem);
			mActive.pushBack(true);
		}
		inline void push(const T &elem)
		{
			push(std::move(elem));
		}
		inline void erase(const uint32_t id)
		{
			CM3D_ASSERT(mActive[id]);
			mActive[id] = false;
			
			// Check if some memory is extra
			DynArray<bool>::rIterator iter;
			for (iter = mActive.rBegin(); iter != mActive.rEnd(); ++iter)
				if (*iter) break;
			
			auto lastFreed = DynArray<bool>::Iterator(iter.data());
			if (mActive.end() - lastFreed) {
				size_t newSize = lastFreed - mActive.begin();
				mActive.resize(newSize);
				mArr.resize(newSize);
			}
			
			// incase T = Object, it's empty at begin of life, so
			// that should bring some perf
			mArr[id] = T();
		}

		inline bool operator==(const UniqueStorage<T> &other) const noexcept {
			return mActive == other.mActive && mArr == other.mArr;
		}
	};
}

#endif
