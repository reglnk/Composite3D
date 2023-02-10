#ifndef CM3D_DYN_ARRAY_HPP
#define CM3D_DYN_ARRAY_HPP

#include <Types/DynArrayIterators.hpp>
#include <Core/Malloc.hpp>
#include <Utility/Std.hpp>
#include <Utility/Debug.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <initializer_list>
#include <utility>
#include <type_traits>

/* =======================================================================
 * A dynamic array class template. @todo move some code to .inl
**/

// Define if the elements need to be default constructed/destructed
#define CM3D_DYN_ARRAY_SHDEFCONS(Type) (!std::is_scalar<Type>::value && std::is_default_constructible<Type>::value)
#define CM3D_DYN_ARRAY_SHDESTROY(Type) (!std::is_scalar<Type>::value && std::is_destructible<Type>::value)

// Whether elements don't need to be constructed/destructed or have noexcept default constructors/destructors
#define CM3D_DYN_ARRAY_NOEXINIT(Type) (!CM3D_DYN_ARRAY_SHDEFCONS(Type) || std::is_nothrow_default_constructible<Type>::value)
#define CM3D_DYN_ARRAY_NOEXDEST(Type) (!CM3D_DYN_ARRAY_SHDESTROY(Type) || std::is_nothrow_destructible<Type>::value)

// Try to guess whether arrays of Type can be compared with memcmp
#define CM3D_DYN_ARRAY_ISCOMP(Type) (std::is_pointer<Type>::value || std::is_scalar<Type>::value)

// @todo more asserts

namespace cm3d
{
	template<typename T>
	class DynArray
	{
	protected:
		T *arr;
		size_t len;
		size_t maxlen;

		static CM3D_CXX14_CONSTEXPR_INLINE size_t growFn(const size_t current) {
			// nearly current * 1.625
			return current + (current >> 1) + (current >> 3) + 1;
		}
		static CM3D_CXX14_CONSTEXPR_INLINE size_t shrinkFn(const size_t current) {
			// nearly current * 0.625
			return current - ((current >> 2) + (current >> 3));
		}
		inline void p_destructRange(T *const beg_, T *const end_) noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			if (CM3D_DYN_ARRAY_SHDESTROY(T))
				for (T *iter = beg_; iter != end_; ++iter)
					iter->~T();
		}
		inline void p_structDefault(T *const beg_, T *const end_) noexcept(CM3D_DYN_ARRAY_NOEXINIT(T)) {
			if (CM3D_DYN_ARRAY_SHDEFCONS(T))
				for (T *iter = beg_; iter != end_; ++iter)
					*iter = T();
			// Init with zeros for elements to destruct correctly later
			else if (CM3D_DYN_ARRAY_SHDESTROY(T))
				memset(beg_, 0, end_ - beg_);
		}
		
	public:
		typedef dynArrForwardIterator<T> Iterator;
		typedef dynArrReverseIterator<T> rIterator;
		typedef dynArrForwardIterator<const T> cIterator;
		typedef dynArrReverseIterator<const T> crIterator;

		using valueType = T;

		inline DynArray() noexcept: arr(NULL), len(0), maxlen(0) {}

		inline DynArray(const size_t len) noexcept(CM3D_DYN_ARRAY_NOEXINIT(T))
		: len(len), maxlen(len)
		{
			arr = (T *)mAlloc(sizeof(T) * maxlen);
			p_structDefault(arr, arr + len);
		}

		// @todo test
		inline DynArray(const DynArray<T> &other) noexcept(std::is_nothrow_copy_assignable<T>::value)
		: len(other.len), maxlen(other.len)
		{
			arr = (T *)mAlloc(sizeof(T) * maxlen);
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				std::copy(other.arr, other.arr + other.len, arr);
			else std::memcpy(arr, other.arr, len * sizeof(T));
		}

		// @todo test
		template<typename IterType>
		inline DynArray(IterType beg_, size_t len_)
		noexcept(std::is_nothrow_copy_assignable<T>::value): len(0), maxlen(0)
		{
			arr = (T *)mAlloc(sizeof(T) * len_);
			len = maxlen = len_;
			for (Iterator iter = begin(); len_; len--)
				*iter++ = *beg_++;
			std::copy(beg_, beg_ + len_, this->begin());
		}

		inline DynArray(DynArray<T> &&rv) noexcept
		: arr(rv.arr), len(rv.len), maxlen(rv.maxlen) {
			rv.arr = NULL;
		}

		inline DynArray(std::initializer_list<T> init_)
		noexcept(std::is_nothrow_copy_assignable<T>::value): len(0), maxlen(0)
		{
			arr = (T *)mAlloc(sizeof(T) * init_.size());
			
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				std::copy(init_.begin(), init_.end(), arr);
			else memcpy(arr, init_.begin(), init_.size() * sizeof(T));

			len = maxlen = init_.size();
		}

		inline ~DynArray() noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			if (arr) {
				p_destructRange(arr, arr + len);
				mFree(arr);
			}
		}

		// Get forward offset of iterator of any type that belongs to this object
		template<typename IterType>
		CM3D_CXX14_CONSTEXPR_INLINE ssize_t getOffset(IterType iter) const {
			return (const T *)iter - arr;
		}

		CM3D_CXX14_CONSTEXPR_INLINE T *data() { return arr; }
		CM3D_CXX14_CONSTEXPR_INLINE const T *data() const { return arr; }

		CM3D_CXX14_CONSTEXPR_INLINE T &operator[](const size_t ind) { return arr[ind]; }
		CM3D_CXX14_CONSTEXPR_INLINE T &back() { return arr[len - 1]; }
		CM3D_CXX14_CONSTEXPR_INLINE T &front() { return arr[0]; }
		CM3D_CXX14_CONSTEXPR_INLINE const T &operator[](const size_t ind) const { return arr[ind]; }
		CM3D_CXX14_CONSTEXPR_INLINE const T &back() const { return arr[len - 1]; }
		CM3D_CXX14_CONSTEXPR_INLINE const T &front() const { return arr[0]; }

		CM3D_CXX14_CONSTEXPR_INLINE Iterator begin() { return Iterator(arr); }
		CM3D_CXX14_CONSTEXPR_INLINE Iterator end() { return Iterator(arr + len); }
		CM3D_CXX14_CONSTEXPR_INLINE cIterator begin() const { return cIterator(arr); }
		CM3D_CXX14_CONSTEXPR_INLINE cIterator end() const { return cIterator(arr + len); }

		CM3D_CXX14_CONSTEXPR_INLINE cIterator cBegin() const { return cIterator(arr); }
		CM3D_CXX14_CONSTEXPR_INLINE cIterator cEnd() const { return cIterator(arr + len); }

		CM3D_CXX14_CONSTEXPR_INLINE rIterator rBegin() { return rIterator(arr + len - 1); }
		CM3D_CXX14_CONSTEXPR_INLINE rIterator rEnd() { return rIterator(arr - 1); }
		CM3D_CXX14_CONSTEXPR_INLINE crIterator crBegin() const { return crIterator(arr + len - 1); }
		CM3D_CXX14_CONSTEXPR_INLINE crIterator crEnd() const { return crIterator(arr - 1); }

		CM3D_CXX14_CONSTEXPR_INLINE size_t length() const { return len; }
		CM3D_CXX14_CONSTEXPR_INLINE size_t capacity() const { return maxlen; }

		CM3D_CXX14_CONSTEXPR_INLINE Iterator find(const T &elem, Iterator f = NULL) noexcept {
			if (!f)
				f = this->begin();
			for (; f != this->end(); ++f)
				if (*f == elem)
					break;
	
			return f;
		}
		CM3D_CXX14_CONSTEXPR_INLINE cIterator find(const T &elem, cIterator f = NULL) const noexcept {
			if (!f)
				f = this->begin();
			for (; f != this->end(); ++f)
				if (*f == elem)
					break;
	
			return f;
		}
		CM3D_CXX14_CONSTEXPR_INLINE void clear() noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			if (arr) {
				p_destructRange(arr, arr + len);
				mFree(arr);
			}
			arr = NULL;
			len = maxlen = 0;
		}
		CM3D_CXX14_CONSTEXPR_INLINE void autoGrow(const size_t required) {
			size_t targetSize = maxlen;
			while (targetSize < required)
				targetSize = growFn(targetSize);
			if (targetSize > maxlen)
				reserve(targetSize);
		}
		CM3D_CXX14_CONSTEXPR_INLINE void autoShrink(const size_t required) {
			// Leave extra (maxlen - shrinkFn(maxlen)) elements to delete after growing to shrink again
			const size_t shrmax = shrinkFn(maxlen);
			if (growFn(required) < shrmax)
				reserve(shrmax);
		}
		CM3D_CXX14_CONSTEXPR_INLINE void autoShrink() {
			return autoShrink(len);
		}
		inline void reserve(const size_t newlen) {
			if (newlen < len)
				p_destructRange(arr + newlen, arr + len);
			
			T *newarr = (T *)mRealloc(arr, sizeof(T) * newlen);
			arr = newarr;
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
			resize(len + 1);
			back() = elem;
		}
		inline void pushBack(T &&elem) {
			resize(len + 1);
			back() = elem;
		}
		inline void popBack() {
			--len;
			if (CM3D_DYN_ARRAY_SHDESTROY(T))
				arr[len].~T();
			
			autoShrink();
		}
		// @todo test
		inline void insert(const T &elem, const size_t ind) {
			resize(len + 1);
			if (ind + 1 < len)
				memmove(arr + ind + 1, arr + ind, sizeof(T) * (len - 1 - ind));
			arr[ind] = elem;
		}

	protected:
		CM3D_CXX14_CONSTEXPR_INLINE void nrInsertEmptyRange(
			const size_t pos, const size_t len_
		) noexcept {
			T *src = arr + pos;
			memmove(src + len_, src, sizeof(T) * (len - pos));
			len += len_;
		}
		CM3D_CXX14_CONSTEXPR_INLINE void insertEmptyRange(
			const size_t pos, const size_t len_
		) {
			autoGrow(len + len_);
			nrInsertEmptyRange(pos, len_);
		}
		
		// Check that the iterator pointer belongs to this array
		// (note that end iterators still can point +/- 1 out of array bounds)
		template<typename IterType>
		CM3D_CXX14_CONSTEXPR_INLINE bool validateIterator(const IterType p)
		const noexcept {
			return p >= arr && p <= arr + len;
		}
		template<typename IterType>
		CM3D_CXX14_CONSTEXPR_INLINE bool validateIterators(
			const IterType beg_, const IterType end_
		) const noexcept {
			// Assume that end might equal beg
			// (so that iterator pair selects 0 elements)
			return validateIterator(beg_) &&
				(validateIterator(end_ - 1) || validateIterator(end_));
		}
		

	public:
	/* =============================================================================================
	 * The methods for inserting elements of some data structures. (Not for inserting a
	 * range of DynArray into itself. See nrInsert)
	**/
		// The basic template function shouldn't be used for inserting DynArray
		// or other linear arrays' ranges.
		template<typename IterType>
		CM3D_CXX14_CONSTEXPR_INLINE void insert(
			const size_t pos, IterType srcbeg, size_t srclen
		) {
			insertEmptyRange(pos, srclen);
			T *dest = arr + pos;
			std::copy(srcbeg, srcbeg + srclen, dest);
		}

		CM3D_CXX14_CONSTEXPR_INLINE void insert(
			const size_t pos, cIterator srcbeg, const size_t srclen
		) {
			insertEmptyRange(pos, srclen);
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				memcpy(arr + pos, srcbeg, sizeof(T) * srclen);
			else std::copy(srcbeg.data(), srcbeg.data() + srclen, arr + pos);
		}

		CM3D_CXX14_CONSTEXPR_INLINE void insert(
			const size_t pos, Iterator srcbeg, const size_t srclen
		) { this->insert(pos, (cIterator)srcbeg, srclen); }

		CM3D_CXX14_CONSTEXPR_INLINE void insert(
			const size_t pos, const T *srcbeg, const size_t srclen
		) { this->insert(pos, cIterator(srcbeg), srclen); }

		CM3D_CXX14_CONSTEXPR_INLINE void insert(
			const size_t pos, T *srcbeg, const size_t srclen
		) { this->insert(pos, Iterator(srcbeg), srclen); }
		
		CM3D_CXX14_CONSTEXPR_INLINE void insert(
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

		CM3D_CXX14_CONSTEXPR_INLINE void insert(
			const size_t pos, rIterator srcbeg, const size_t srclen
		) { this->insert(pos, (crIterator)srcbeg, srclen); }
	/* =============================================================================================
	 * These methods guarantee that the array isn't resized within a call (useful for inserting
	 * a range of a DynArray into itself).
	**/
	// @todo @dry
		template<typename IterType>
		CM3D_CXX14_CONSTEXPR_INLINE void nrInsert(
			const size_t pos, IterType srcbeg, size_t srclen
		) noexcept(std::is_nothrow_copy_assignable<T>::value)
		{
			nrInsertEmptyRange(pos, srclen);
			std::copy(srcbeg, srcbeg + srclen, arr + pos);
		}

		CM3D_CXX14_CONSTEXPR_INLINE void nrInsert(
			const size_t pos, cIterator srcbeg, const size_t srclen
		) noexcept(std::is_nothrow_copy_assignable<T>::value)
		{
			nrInsertEmptyRange(pos, srclen);
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				memcpy(arr + pos, srcbeg, srclen);
			else std::copy(srcbeg, srcbeg + srclen, arr + pos);
		}

		CM3D_CXX14_CONSTEXPR_INLINE void nrInsert(
			const size_t pos, Iterator srcbeg, const size_t srclen
		) noexcept(std::is_nothrow_copy_assignable<T>::value) {
			return this->nrInsert(pos, (cIterator)srcbeg, srclen);
		}

		CM3D_CXX14_CONSTEXPR_INLINE void nrInsert(
			const size_t pos, const T *srcbeg, const size_t srclen
		) noexcept(std::is_nothrow_copy_assignable<T>::value) {
			return this->nrInsert(pos, cIterator(srcbeg), srclen);
		}

		CM3D_CXX14_CONSTEXPR_INLINE void nrInsert(
			const size_t pos, T *srcbeg, const size_t srclen
		) noexcept(std::is_nothrow_copy_assignable<T>::value) {
			return this->nrInsert(pos, Iterator(srcbeg), srclen);
		}

		CM3D_CXX14_CONSTEXPR_INLINE void nrInsert(
			const size_t pos, crIterator srcbeg, const size_t srclen
		) noexcept(std::is_nothrow_copy_assignable<T>::value)
		{
			cIterator fwEnd = srcbeg - 1;
			cIterator fwBeg = fwEnd - srclen;

			CM3D_ASSERT(validateIterators(fwBeg, fwEnd));

			nrInsertEmptyRange(pos, srclen);
			memcpy(arr + pos, fwBeg, srclen);
		}

		CM3D_CXX14_CONSTEXPR_INLINE void nrInsert(
			const size_t pos, rIterator srcbeg, const size_t srclen
		) noexcept(std::is_nothrow_copy_assignable<T>::value) {
			return this->nrInsert(pos, (crIterator)srcbeg, srclen);
		}
	/* =============================================================================================
	 * Methods for erasing certain elements
	**/
		CM3D_CXX14_CONSTEXPR_INLINE void erase(Iterator beg, const size_t len_ = 1)
		noexcept(CM3D_DYN_ARRAY_NOEXDEST(T)) {
			ssize_t movsize = (arr + len) - (beg + len_);
			CM3D_ASSERT(movsize >= 0);
			p_destructRange(beg, beg + len_);
			memmove(beg, beg + len_, sizeof(T) * movsize);
			len -= len_;
			autoShrink();
		}
		CM3D_CXX14_CONSTEXPR_INLINE void erase(rIterator beg, const size_t len_ = 1)
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
				mFree(arr);
			len = maxlen = other.len;

			arr = (T *)mAlloc(sizeof(T) * maxlen);
			if (CM3D_DYN_ARRAY_ISCOMP(T))
				memcpy(arr, other.arr, len * sizeof(T));
			else std::copy(other.arr, other.arr + len, arr);

			return *this;
		}		
		inline DynArray<T> &operator=(const DynArray<T> &&rv) noexcept
		{
			if (arr)
				mFree(arr);
			
			this->arr = rv.arr;
			this->len = rv.len;
			this->maxlen = rv.maxlen;
			
			rv.arr = NULL;

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
