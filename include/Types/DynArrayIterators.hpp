#ifndef CM3D_DYN_ARRAY_ITERATORS_HPP
#define CM3D_DYN_ARRAY_ITERATORS_HPP

#include <Utility/Std.hpp>

namespace cm3d
{
	template<typename T>
	class dynArrIteratorBase
	{
	protected:
		T *m_data;
	public:
		using valueType = T;
		
		inline dynArrIteratorBase() noexcept = default;
		inline dynArrIteratorBase(const dynArrIteratorBase<T> &it) noexcept = default;
		inline dynArrIteratorBase(dynArrIteratorBase<T> &&rv) noexcept = default;
		
		inline dynArrIteratorBase(T *data) noexcept: m_data(data) {}

		CM3D_CXX14_CONSTEXPR_INLINE dynArrIteratorBase<T> operator =(const dynArrIteratorBase<T> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrIteratorBase<T> operator =(dynArrIteratorBase<T> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrIteratorBase<T> operator =(T *data) {
			this->m_data = data;
			return *this;
		}

		CM3D_CXX14_CONSTEXPR_INLINE T *data() {
			return m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const T *data() const {
			return m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE operator T *() {
			return m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE operator const T *() const {
			return m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE T &operator *() {
			return *m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const T &operator *() const {
			return *m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator ==(
			const dynArrIteratorBase<T> other
		) const {
			return m_data == other.m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator !=(
			const dynArrIteratorBase<T> other
		) const {
			return m_data != other.m_data;
		}
	};
	
	template<typename T>
	class dynArrForwardIterator: public dynArrIteratorBase<T>
	{
	public:
		inline dynArrForwardIterator() noexcept = default;
		inline dynArrForwardIterator(const dynArrForwardIterator<T> &it) noexcept = default;
		inline dynArrForwardIterator(dynArrForwardIterator<T> &&rv) = default;
		
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator(T *data) noexcept:
			dynArrIteratorBase<T>(data) {}
		
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> operator =(const dynArrForwardIterator<T> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> operator =(dynArrForwardIterator<T> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}

		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE T &operator [](const intType i) {
			return this->m_data[i];
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE const T &operator [](const intType i) const {
			return this->m_data[i];
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> operator ++() {
			++this->m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> operator --() {
			--this->m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> operator ++(int) {
			dynArrForwardIterator<T> th = *this;
			++this->m_data;
			return th;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> operator --(int) {
			dynArrForwardIterator<T> th = *this;
			--this->m_data;
			return th;
		}
		CM3D_CXX14_CONSTEXPR_INLINE ssize_t operator -(const dynArrForwardIterator<T> it) {
			return this->m_data - it.m_data;
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> operator +(const intType i) {
			return dynArrForwardIterator<T>(this->m_data + i);
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> operator -(const intType i) {
			return dynArrForwardIterator<T>(this->m_data - i);
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> &operator +=(const intType i) {
			this->m_data += i;
			return *this;
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE dynArrForwardIterator<T> &operator -=(const intType i) {
			this->m_data -= i;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator >(const dynArrForwardIterator<T> other) const {
			return this->m_data > other.m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator >=(const dynArrForwardIterator<T> other) const {
			return this->m_data >= other.m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator <(const dynArrForwardIterator<T> other) const {
			return this->m_data < other.m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator <=(const dynArrForwardIterator<T> other) const {
			return this->m_data <= other.m_data;
		}
		template<typename Ty>
		friend class DynArray;
	};
	template<typename T>
	class dynArrReverseIterator: public dynArrIteratorBase<T>
	{
	public:
		inline dynArrReverseIterator() noexcept = default;
		inline dynArrReverseIterator(const dynArrReverseIterator<T> &it) = default;
		inline dynArrReverseIterator(dynArrReverseIterator<T> &&rv) = default;
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator(T *data) noexcept:
			dynArrIteratorBase<T>(data) {}

		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE T &operator [](const intType i) {
			return this->m_data[-i];
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE const T &operator [](const intType i) const {
			return this->m_data[-i];
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator<T> operator ++() {
			--this->m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator<T> operator --() {
			++this->m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator<T> operator ++(int) {
			dynArrReverseIterator<T> th = *this;
			--this->m_data;
			return th;
		}
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator<T> operator --(int) {
			dynArrReverseIterator<T> th = *this;
			++this->m_data;
			return th;
		}
		CM3D_CXX14_CONSTEXPR_INLINE ssize_t operator -(const dynArrReverseIterator<T> it) {
			return it.m_data - this->m_data;
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator<T> operator +(const intType i) {
			return dynArrReverseIterator<T>(this->m_data - i);
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator<T> operator -(const intType i) {
			return dynArrReverseIterator<T>(this->m_data + i);
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator<T> &operator +=(const intType i) {
			this->m_data -= i;
			return *this;
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE dynArrReverseIterator<T> &operator -=(const intType i) {
			this->m_data += i;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator >(const dynArrReverseIterator<T> other) const {
			return this->m_data < other.m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator >=(const dynArrReverseIterator<T> other) const {
			return this->m_data <= other.m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator <(const dynArrReverseIterator<T> other) const {
			return this->m_data > other.m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator <=(const dynArrReverseIterator<T> other) const {
			return this->m_data >= other.m_data;
		}
		template<typename Ty>
		friend class DynArray;
	};
}

#endif // CM3D_DYN_ARRAY_ITERATORS_HPP