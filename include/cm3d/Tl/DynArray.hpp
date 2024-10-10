#ifndef CM3D_DYN_ARRAY_HPP
#define CM3D_DYN_ARRAY_HPP

#include <cm3d/Tl/DynArrayIterators.hpp>
#include <cm3d/Core/Malloc.hpp>
#include <cm3d/Utility/Std.hpp>
#include <cm3d/Utility/Debug.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <initializer_list>
#include <utility>
#include <type_traits>

/* =======================================================================
 * A dynamic array template
 *
 * @todo deep test
**/

// Whether elements don't need to be constructed/destructed or have noexcept default constructors/destructors
#define CM3D_DYN_ARRAY_NOEXINIT(Type) (std::is_nothrow_default_constructible<Type>::value)
#define CM3D_DYN_ARRAY_NOEXDEST(Type) (std::is_nothrow_destructible<Type>::value)

// Try to guess whether arrays of Type can be compared with memcmp
#define CM3D_DYN_ARRAY_ISCOMP(Type) (std::is_pointer<Type>::value || std::is_scalar<Type>::value)

namespace cm3d
{
	template<typename T>
	class DynArray
	{
	protected:
		T *arr;
		size_t len;
		size_t maxlen;

		static inline T *p_allocRawMem(size_t _size)
		{
			if constexpr (alignof(T) <= sizeof(size_t))
				return reinterpret_cast<T *>(mAlloc(_size));
			else return reinterpret_cast<T *>(mAlignedAlloc(alignof(T), _size));
		}
		static inline void p_freeRawMem(T *_mem)
		{
			if constexpr (alignof(T) <= sizeof(size_t))
				return mFree(_mem);
			else return mAlignedFree(_mem);
		}
		inline T *p_reserveMem(size_t _newcap)
		{
			T *newmem = p_allocRawMem(sizeof(T) * _newcap);
			for (size_t i = 0; i != len; ++i)
				new (&newmem[i]) T(std::move(arr[i]));
			
			p_destructRange(arr, arr + len);
			p_freeRawMem(arr);
			
			return newmem;
		}
		static inline void p_destructRange(T *const _beg, T *const _end) noexcept(CM3D_DYN_ARRAY_NOEXDEST(T))
		{
			for (T *iter = _beg; iter != _end; ++iter)
				iter->~T();
		}
		static inline void p_structDefault(T *const _beg, T *const _end) noexcept(CM3D_DYN_ARRAY_NOEXINIT(T))
		{
			new(_beg) T[_end - _beg];
		}
		
	public:
		typedef dynArrForwardIterator<T> Iterator;
		typedef dynArrReverseIterator<T> rIterator;
		typedef dynArrForwardIterator<const T> cIterator;
		typedef dynArrReverseIterator<const T> crIterator;

		using valueType = T;

		inline DynArray() noexcept: arr(nullptr), len(0), maxlen(0) {}

		inline DynArray(const size_t len)
		: len(len), maxlen(len)
		{
			arr = p_allocRawMem(sizeof(T) * maxlen);
			p_structDefault(arr, arr + len);
		}

		inline DynArray(const DynArray<T> &other)
		: len(other.len), maxlen(other.len)
		{
			arr = p_allocRawMem(sizeof(T) * maxlen);
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				std::copy(other.arr, other.arr + other.len, arr);
			else std::memcpy(arr, other.arr, len * sizeof(T));
		}

		template<typename IterType>
		inline DynArray(IterType beg_, size_t len_): len(len_), maxlen(len_)
		{
			arr = p_allocRawMem(sizeof(T) * len);
			for (Iterator iter = begin(), src = beg_; iter != end(); )
				*iter++ = *src++;
			std::copy(beg_, beg_ + len_, begin());
		}

		inline DynArray(DynArray<T> &&rv) noexcept
		: arr(rv.arr), len(rv.len), maxlen(rv.maxlen) {
			rv.arr = nullptr;
		}

		inline DynArray(std::initializer_list<T> init_): len(0), maxlen(0)
		{
			arr = p_allocRawMem(sizeof(T) * init_.size());
			
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				std::copy(init_.begin(), init_.end(), arr);
			else memcpy(arr, init_.begin(), init_.size() * sizeof(T));

			len = maxlen = init_.size();
		}

		inline ~DynArray() noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			clear();
		}
		
		static constexpr inline size_t growFn(const size_t current) {
			// nearly current * 1.625
			return current + (current >> 1) + (current >> 3) + 1;
		}
		static constexpr inline size_t shrinkFn(const size_t current) {
			// nearly current * 0.625
			return current - ((current >> 2) + (current >> 3));
		}

		// Get forward offset of iterator of any type that belongs to this object
		template<typename IterType>
		constexpr inline ssize_t getOffset(IterType iter) const {
			return (const T *)(iter - arr);
		}
		template<typename IterType>
		constexpr inline ssize_t getOffset(IterType iter) {
			return (T *)(iter - arr);
		}

		constexpr inline T *data() { return arr; }
		constexpr inline const T *data() const { return arr; }
		
		constexpr inline size_t length() const { return len; }
		constexpr inline size_t size() const { return len; }
		constexpr inline size_t capacity() const { return maxlen; }
		constexpr inline size_t objsize() const { return sizeof(T); }
		constexpr inline size_t bytecount() const { return len * sizeof(T); }

		constexpr inline T &operator[](const size_t ind) { return arr[ind]; }
		constexpr inline T &back() { return arr[len - 1]; }
		constexpr inline T &front() { return arr[0]; }
		constexpr inline const T &operator[](const size_t ind) const { return arr[ind]; }
		constexpr inline const T &back() const { return arr[len - 1]; }
		constexpr inline const T &front() const { return arr[0]; }

		constexpr inline Iterator begin() { return Iterator(arr); }
		constexpr inline Iterator end() { return Iterator(arr + len); }
		constexpr inline cIterator begin() const { return cIterator(arr); }
		constexpr inline cIterator end() const { return cIterator(arr + len); }

		constexpr inline cIterator cBegin() const { return cIterator(arr); }
		constexpr inline cIterator cEnd() const { return cIterator(arr + len); }

		constexpr inline rIterator rBegin() { return rIterator(arr + len - 1); }
		constexpr inline rIterator rEnd() { return rIterator(arr - 1); }
		constexpr inline crIterator crBegin() const { return crIterator(arr + len - 1); }
		constexpr inline crIterator crEnd() const { return crIterator(arr - 1); }

		constexpr inline void clear() noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			if (arr) {
				p_destructRange(arr, arr + len);
				p_freeRawMem(arr);
			}
			arr = nullptr;
			len = maxlen = 0;
		}
		constexpr inline void autoGrow(const size_t required) {
			size_t targetSize = maxlen;
			while (targetSize < required)
				targetSize = growFn(targetSize);
			if (targetSize > maxlen)
				reserve(targetSize);
		}
		constexpr inline void autoShrink(const size_t required) {
			// Leave extra (maxlen - shrinkFn(maxlen)) elements to delete after growing to shrink again
			const size_t shrmax = shrinkFn(maxlen);
			if (growFn(required) < shrmax)
				reserve(shrmax);
		}
		constexpr inline void autoShrink() {
			return autoShrink(len);
		}
		inline void reserve(const size_t newlen) {
			if (newlen < len)
				p_destructRange(arr + newlen, arr + len);
			
			arr = p_reserveMem(sizeof(T) * newlen);
			maxlen = newlen;
		}
		inline void resize(const size_t newlen) {
			if (len < newlen) {
				autoGrow(newlen);
				p_structDefault(arr + len, arr + newlen);
				len = newlen;
			}
			else {
				p_destructRange(arr + newlen, arr + len);
				len = newlen;
				autoShrink();
			}
		}
		inline void pushBack(const T &elem) {
			autoGrow(len + 1);
			new(&arr[len++]) T(elem);
		}
		inline void pushBack(T &&elem) {
			autoGrow(len + 1);
			new(&arr[len++]) T(std::move(elem));
		}
		inline void popBack() {
			arr[--len].~T();
			autoShrink();
		}

	protected:
		constexpr inline void nrInsertEmptyRange(
			const size_t pos, const size_t len_
		) noexcept {
			CM3D_ASSERT(len + len_ <= maxlen);
			T *src = arr + pos;
			memmove(src + len_, src, sizeof(T) * (len - pos));
			p_structDefault(src, src + len_);
			len += len_;
		}
		constexpr inline void insertEmptyRange(
			const size_t pos, const size_t len_
		) {
			autoGrow(len + len_);
			nrInsertEmptyRange(pos, len_);
		}

	public:
		// Check that the iterator belongs to this array
		template<typename IterType>
		constexpr inline bool validateIterator(const IterType p)
		const noexcept {
			return (T *)p >= arr && (T *)p < arr + len;
		}
		constexpr inline bool validateIterators(
			const cIterator _beg, const cIterator _end
		) const noexcept {
			return validateIterator(_beg) &&
				((const T *)_end >= arr && (const T *)_end <= arr + len);
		}
		constexpr inline bool validateIterators(
			const Iterator _beg, const Iterator _end
		) const noexcept {
			return validateIterator(_beg) &&
				((T *)_end >= arr && (T *)_end <= arr + len);
		}
		constexpr inline bool validateIterators(
			const crIterator _beg, const crIterator _end
		) const noexcept {
			return validateIterator(_beg) &&
				((const T *)_end - arr >= -1 && (const T *)_end < arr + len);
		}
		constexpr inline bool validateIterators(
			const rIterator _beg, const rIterator _end
		) const noexcept {
			return validateIterator(_beg) &&
				((T *)_end - arr >= -1 && (T *)_end < arr + len);
		}

	/* =========================================================================
	 * Methods for inserting elements ranges
	**/
		constexpr inline void insert(const size_t pos, const T &source) {
			insertEmptyRange(pos, 1);
			arr[pos] = source;
		}
		constexpr inline void insert(const size_t pos, T &&source) {
			insertEmptyRange(pos, 1);
			arr[pos] = std::move(source);
		}
		constexpr inline void insert(
			const size_t pos, cIterator srcbeg, const size_t srclen
		) {
			insertEmptyRange(pos, srclen);
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				memcpy(arr + pos, srcbeg, sizeof(T) * srclen);
			else std::copy(srcbeg.data(), srcbeg.data() + srclen, arr + pos);
		}

		constexpr inline void insert(
			const size_t pos, Iterator srcbeg, const size_t srclen
		) { this->insert(pos, (cIterator)srcbeg, srclen); }

		constexpr inline void insert(
			const size_t pos, const T *srcbeg, const size_t srclen
		) { this->insert(pos, cIterator(srcbeg), srclen); }

		constexpr inline void insert(
			const size_t pos, T *srcbeg, const size_t srclen
		) { this->insert(pos, Iterator(srcbeg), srclen); }

		constexpr inline void insert(
			const size_t pos, crIterator srcbeg, const size_t srclen
		) {
			cIterator fwEnd = srcbeg - 1;
			cIterator fwBeg = fwEnd - srclen;

			CM3D_ASSERT(validateIterator(fwBeg));
			insertEmptyRange(pos, srclen);

			if (CM3D_DYN_ARRAY_ISCOMP(T))
				memcpy(arr + pos, fwBeg.data(), sizeof(T) * srclen);
			else std::copy(fwBeg.data(), fwBeg.data() + srclen, arr + pos);
		}

		constexpr inline void insert(
			const size_t pos, rIterator srcbeg, const size_t srclen
		) { this->insert(pos, (crIterator)srcbeg, srclen); }

		// This function shouldn't be used for inserting DynArray
		// or other linear arrays' ranges.
		template<typename IterType>
		constexpr inline void insert(
			const size_t pos, IterType srcbeg, size_t srclen
		) {
			insertEmptyRange(pos, srclen);
			std::copy(srcbeg, srcbeg + srclen, arr + pos);
		}

	/* =============================================================================================
	 * Methods for erasing certain elements
	**/
		constexpr inline void erase(Iterator beg, const size_t len_ = 1)
		noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			ssize_t movsize = (arr + len) - (beg + len_);
			CM3D_ASSERT(movsize >= 0);
			p_destructRange(beg, beg + len_);
			memmove(beg, beg + len_, sizeof(T) * movsize);
			len -= len_;
			autoShrink();
		}
		constexpr inline void erase(rIterator beg, const size_t len_ = 1)
		noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			Iterator fw_beg = (beg + len_) - 1;
			this->erase(fw_beg, len_);
		}
	/* =============================================================================================
	 * Operators
	**/
		inline DynArray<T> &operator=(const DynArray<T> &other)
		{
			if (arr)
				p_freeRawMem(arr);
			len = maxlen = other.len;

			arr = (T *)p_allocRawMem(sizeof(T) * maxlen);
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				memcpy(arr, other.arr, len * sizeof(T));
			else std::copy(other.arr, other.arr + len, arr);

			return *this;
		}		
		inline DynArray<T> &operator=(DynArray<T> &&rv) noexcept
		{
			if (arr)
				p_freeRawMem(arr);
			
			this->arr = rv.arr;
			this->len = rv.len;
			this->maxlen = rv.maxlen;
			
			rv.arr = nullptr;

			return *this;
		}
		
		inline bool operator==(const DynArray<T> &other) const noexcept
		{
			if (len != other.len)
				return false;
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				return !memcmp(arr, other.arr, sizeof(T) * len);
			
			auto iter = begin();
			auto oiter = other.begin();

			while (iter != end())
				if (*iter++ != *oiter++)
					return false;
			
			return true;
		}
	};
}

#endif
