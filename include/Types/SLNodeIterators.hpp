#ifndef CM3D_SL_NODE_ITERATORS_HPP
#define CM3D_SL_NODE_ITERATORS_HPP

#include <Types/SLNode.hpp>
#include <Utility/Std.hpp>
#include <Utility/Debug.hpp>


namespace cm3d
{
	template<typename T>
	class slNodeIteratorBase
	{
	protected:
		SLNode<T> *m_data;
	public:
		using valueType = T;
		
		inline slNodeIteratorBase() noexcept = default;
		inline slNodeIteratorBase(const slNodeIteratorBase<T> &it) noexcept = default;
		inline slNodeIteratorBase(slNodeIteratorBase<T> &&rv) noexcept = default;
		
		inline slNodeIteratorBase(SLNode<T> *data) noexcept: m_data(data) {}

		CM3D_CXX14_CONSTEXPR_INLINE slNodeIteratorBase<T> operator =(const slNodeIteratorBase<T> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIteratorBase<T> operator =(slNodeIteratorBase<T> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIteratorBase<T> operator =(SLNode<T> *data) {
			this->m_data = data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE SLNode<T> *data() {
			return m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const SLNode<T> *data() const {
			return m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE operator SLNode<T> *() {
			return m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE operator const SLNode<T> *() const {
			return m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE T &operator *() {
			return m_data->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const T &operator *() const {
			return m_data->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE T *operator ->() {
			return &m_data->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const T *operator ->() const {
			return &m_data->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator ==(
			const slNodeIteratorBase<T> other
		) const {
			return m_data == other.m_data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator !=(
			const slNodeIteratorBase<T> other
		) const {
			return m_data != other.m_data;
		}
	};
	
	template<typename T>
	class slNodeIterator: public slNodeIteratorBase<T>
	{
	public:
		inline slNodeIterator() noexcept = default;
		inline slNodeIterator(const slNodeIterator<T> &it) noexcept = default;
		inline slNodeIterator(slNodeIterator<T> &&rv) = default;
		
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator(SLNode<T> *data) noexcept:
			slNodeIteratorBase<T>(data) {}
		
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> operator =(const slNodeIterator<T> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> operator =(slNodeIterator<T> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> &operator ++() {
			this->m_data = this->m_data->next;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> operator ++(int) {
			slNodeIterator<T> th = *this;
			this->m_data = this->m_data->next;
			return th;
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> operator +(intType i) {
			CM3D_ASSERT(i >= 0);
			auto dat1 = this->m_data;
			while (i--)
				dat1 = dat1->next;
			return slNodeIterator(dat1);
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> operator -(intType i) {
			CM3D_ASSERT(i <= 0);
			auto dat1 = this->m_data;
			while (i++)
				dat1 = dat1->next;
			return slNodeIterator(dat1);
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> &operator +=(intType i) {
			CM3D_ASSERT(i >= 0);
			while (i--)
				this->m_data = this->m_data->next;
			return *this;
		}
		template<typename intType>
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> &operator -=(intType i) {
			CM3D_ASSERT(i <= 0);
			while (i++)
				this->m_data = this->m_data->next;
			return *this;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator >(const slNodeIterator<T> other) const {
			auto dat1 = other.m_data;
			while ((dat1 = dat1->next))
				if (dat1 == this->m_data)
					return true;
			
			return false;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator >=(const slNodeIterator<T> other) const {
			return this->m_data == other.m_data || *this > other;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator <(const slNodeIterator<T> other) const {
			auto dat1 = this->m_data;
			while ((dat1 = dat1->next))
				if (dat1 == other.m_data)
					return true;
			
			return false;
		}
		CM3D_CXX14_CONSTEXPR_INLINE bool operator <=(const slNodeIterator<T> other) const {
			return this->m_data == other.m_data || *this < other;
		}
		template<typename Ty>
		friend class Queue;
	};
}

#endif // CM3D_BASIC_QUEUE_ITERATORS_HPP
