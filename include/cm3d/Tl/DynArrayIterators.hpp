#ifndef CM3D_DYN_ARRAY_ITERATORS_HPP
#define CM3D_DYN_ARRAY_ITERATORS_HPP

#include <cm3d/Utility/Std.hpp>

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

		constexpr inline dynArrIteratorBase<T> operator =(const dynArrIteratorBase<T> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		constexpr inline dynArrIteratorBase<T> operator =(dynArrIteratorBase<T> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}
		constexpr inline dynArrIteratorBase<T> operator =(T *data) {
			this->m_data = data;
			return *this;
		}
		constexpr inline T *data() {
			return m_data;
		}
		constexpr inline const T *data() const {
			return m_data;
		}
		constexpr inline operator T *() {
			return m_data;
		}
		constexpr inline operator const T *() const {
			return m_data;
		}
		constexpr inline T &operator *() {
			return *m_data;
		}
		constexpr inline const T &operator *() const {
			return *m_data;
		}
		constexpr inline T *operator ->() {
			return m_data;
		}
		constexpr inline const T *operator ->() const {
			return m_data;
		}
		constexpr inline bool operator ==(
			const dynArrIteratorBase<T> other
		) const {
			return m_data == other.m_data;
		}
		constexpr inline bool operator !=(
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
		
		constexpr inline dynArrForwardIterator(T *data) noexcept:
			dynArrIteratorBase<T>(data) {}
		
		constexpr inline dynArrForwardIterator<T> operator =(const dynArrForwardIterator<T> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		constexpr inline dynArrForwardIterator<T> operator =(dynArrForwardIterator<T> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}

		template<typename intType>
		constexpr inline T &operator [](const intType i) {
			return this->m_data[i];
		}
		template<typename intType>
		constexpr inline const T &operator [](const intType i) const {
			return this->m_data[i];
		}
		constexpr inline dynArrForwardIterator<T> operator ++() {
			++this->m_data;
			return *this;
		}
		constexpr inline dynArrForwardIterator<T> operator --() {
			--this->m_data;
			return *this;
		}
		constexpr inline dynArrForwardIterator<T> operator ++(int) {
			dynArrForwardIterator<T> th = *this;
			++this->m_data;
			return th;
		}
		constexpr inline dynArrForwardIterator<T> operator --(int) {
			dynArrForwardIterator<T> th = *this;
			--this->m_data;
			return th;
		}
		constexpr inline ssize_t operator -(const dynArrForwardIterator<T> it) {
			return this->m_data - it.m_data;
		}
		template<typename intType>
		constexpr inline dynArrForwardIterator<T> operator +(const intType i) {
			return dynArrForwardIterator<T>(this->m_data + i);
		}
		template<typename intType>
		constexpr inline dynArrForwardIterator<T> operator -(const intType i) {
			return dynArrForwardIterator<T>(this->m_data - i);
		}
		template<typename intType>
		constexpr inline dynArrForwardIterator<T> &operator +=(const intType i) {
			this->m_data += i;
			return *this;
		}
		template<typename intType>
		constexpr inline dynArrForwardIterator<T> &operator -=(const intType i) {
			this->m_data -= i;
			return *this;
		}
		constexpr inline bool operator >(const dynArrForwardIterator<T> other) const {
			return this->m_data > other.m_data;
		}
		constexpr inline bool operator >=(const dynArrForwardIterator<T> other) const {
			return this->m_data >= other.m_data;
		}
		constexpr inline bool operator <(const dynArrForwardIterator<T> other) const {
			return this->m_data < other.m_data;
		}
		constexpr inline bool operator <=(const dynArrForwardIterator<T> other) const {
			return this->m_data <= other.m_data;
		}
		constexpr inline bool operator >(const T *const other) const {
			return this->m_data > other;
		}
		constexpr inline bool operator >=(const T *const other) const {
			return this->m_data >= other;
		}
		constexpr inline bool operator <(const T *const other) const {
			return this->m_data < other;
		}
		constexpr inline bool operator <=(const T *const other) const {
			return this->m_data <= other;
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
		
		constexpr inline dynArrReverseIterator(T *data) noexcept:
			dynArrIteratorBase<T>(data) {}
		
		constexpr inline dynArrReverseIterator<T> operator =(const dynArrReverseIterator<T> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		constexpr inline dynArrReverseIterator<T> operator =(dynArrReverseIterator<T> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}

		template<typename intType>
		constexpr inline T &operator [](const intType i) {
			return this->m_data[-i];
		}
		template<typename intType>
		constexpr inline const T &operator [](const intType i) const {
			return this->m_data[-i];
		}
		constexpr inline dynArrReverseIterator<T> operator ++() {
			--this->m_data;
			return *this;
		}
		constexpr inline dynArrReverseIterator<T> operator --() {
			++this->m_data;
			return *this;
		}
		constexpr inline dynArrReverseIterator<T> operator ++(int) {
			dynArrReverseIterator<T> th = *this;
			--this->m_data;
			return th;
		}
		constexpr inline dynArrReverseIterator<T> operator --(int) {
			dynArrReverseIterator<T> th = *this;
			++this->m_data;
			return th;
		}
		constexpr inline ssize_t operator -(const dynArrReverseIterator<T> it) {
			return it.m_data - this->m_data;
		}
		template<typename intType>
		constexpr inline dynArrReverseIterator<T> operator +(const intType i) {
			return dynArrReverseIterator<T>(this->m_data - i);
		}
		template<typename intType>
		constexpr inline dynArrReverseIterator<T> operator -(const intType i) {
			return dynArrReverseIterator<T>(this->m_data + i);
		}
		template<typename intType>
		constexpr inline dynArrReverseIterator<T> &operator +=(const intType i) {
			this->m_data -= i;
			return *this;
		}
		template<typename intType>
		constexpr inline dynArrReverseIterator<T> &operator -=(const intType i) {
			this->m_data += i;
			return *this;
		}
		constexpr inline bool operator >(const dynArrReverseIterator<T> other) const {
			return this->m_data < other.m_data;
		}
		constexpr inline bool operator >=(const dynArrReverseIterator<T> other) const {
			return this->m_data <= other.m_data;
		}
		constexpr inline bool operator <(const dynArrReverseIterator<T> other) const {
			return this->m_data > other.m_data;
		}
		constexpr inline bool operator <=(const dynArrReverseIterator<T> other) const {
			return this->m_data >= other.m_data;
		}
		template<typename Ty>
		friend class DynArray;
	};
}

// required for bitsery
#include <iterator>
namespace std
{
	template<typename T>
	struct iterator_traits<cm3d::dynArrForwardIterator<T>>
	{
		using value_type = T;
		using difference_type = ptrdiff_t;
		using iterator_category = struct std::random_access_iterator_tag;
	};

	template<typename T>
	struct iterator_traits<cm3d::dynArrReverseIterator<T>>
	{
		using value_type = T;
		using difference_type = ptrdiff_t;
		using iterator_category = struct std::random_access_iterator_tag;
	};

	template<typename T>
	ptrdiff_t distance(cm3d::dynArrForwardIterator<T> const &q, cm3d::dynArrForwardIterator<T> const &w)
	{
		return w - q;
	}
	
	template<typename T>
	ptrdiff_t distance(cm3d::dynArrReverseIterator<T> const &q, cm3d::dynArrReverseIterator<T> const &w)
	{
		return w - q;
	}
}

#endif // CM3D_DYN_ARRAY_ITERATORS_HPP
