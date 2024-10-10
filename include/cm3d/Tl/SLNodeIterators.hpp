#ifndef CM3D_SL_NODE_ITERATORS_HPP
#define CM3D_SL_NODE_ITERATORS_HPP

#include <cm3d/Tl/SLNode.hpp>
#include <cm3d/Utility/Std.hpp>
#include <cm3d/Utility/Debug.hpp>


namespace cm3d
{
	template<typename NodeType>
	class slNodeIteratorBase
	{
	protected:
		NodeType *m_data;
		
	public:
		using nodeType = NodeType;
		using valueType = typename nodeType::valueType;
		
		inline slNodeIteratorBase(const slNodeIteratorBase<NodeType> &it) noexcept = default;
		inline slNodeIteratorBase(slNodeIteratorBase<NodeType> &&rv) noexcept = default;
		
		inline slNodeIteratorBase(NodeType *data) noexcept: m_data(data) {}

		constexpr inline slNodeIteratorBase<NodeType> operator =(const slNodeIteratorBase<NodeType> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		constexpr inline slNodeIteratorBase<NodeType> operator =(slNodeIteratorBase<NodeType> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}
		constexpr inline slNodeIteratorBase<NodeType> operator =(SLNode<NodeType> *data) {
			this->m_data = data;
			return *this;
		}
		constexpr inline NodeType *data() {
			return m_data;
		}
		constexpr inline const NodeType *data() const {
			return m_data;
		}
		constexpr inline operator NodeType *() {
			return m_data;
		}
		constexpr inline operator const NodeType *() const {
			return m_data;
		}
		constexpr inline valueType &operator *() {
			return m_data->data;
		}
		constexpr inline const valueType &operator *() const {
			return m_data->data;
		}
		constexpr inline valueType *operator ->() {
			return &m_data->data;
		}
		constexpr inline const valueType *operator ->() const {
			return &m_data->data;
		}
		constexpr inline bool operator ==(const slNodeIteratorBase<NodeType> other) const {
			return m_data == other.m_data;
		}
		constexpr inline bool operator !=(const slNodeIteratorBase<NodeType> other) const {
			return m_data != other.m_data;
		}
	};
	
	template<typename NodeType>
	class slNodeFwdIteratorBase: public slNodeIteratorBase<NodeType>
	{
	public:
		using nodeType = NodeType;
		using valueType = typename NodeType::valueType;
		
		inline slNodeFwdIteratorBase() noexcept = default;
		inline slNodeFwdIteratorBase(const slNodeFwdIteratorBase<NodeType> &it) noexcept = default;
		inline slNodeFwdIteratorBase(slNodeFwdIteratorBase<NodeType> &&rv) = default;
		
		constexpr inline slNodeFwdIteratorBase(NodeType *data) noexcept:
			slNodeIteratorBase<NodeType>(data) {}
		
		constexpr inline slNodeFwdIteratorBase<NodeType> operator =(const slNodeFwdIteratorBase<NodeType> &it) {
			this->m_data = it.m_data;
			return *this;
		}
		constexpr inline slNodeFwdIteratorBase<NodeType> operator =(slNodeFwdIteratorBase<NodeType> &&rv) {
			this->m_data = rv.m_data;
			return *this;
		}
		constexpr inline slNodeFwdIteratorBase<NodeType> &operator ++() {
			this->m_data = this->m_data->next;
			return *this;
		}
		constexpr inline slNodeFwdIteratorBase<NodeType> operator ++(int) {
			slNodeFwdIteratorBase<NodeType> th = *this;
			this->m_data = this->m_data->next;
			return th;
		}
		template<typename intType>
		constexpr inline slNodeFwdIteratorBase<NodeType> operator +(intType i) {
			CM3D_ASSERT(i >= 0);
			auto dat1 = this->m_data;
			while (i--)
				dat1 = dat1->next;
			return slNodeFwdIteratorBase(dat1);
		}
		template<typename intType>
		constexpr inline slNodeFwdIteratorBase<NodeType> operator -(intType i) {
			CM3D_ASSERT(i <= 0);
			auto dat1 = this->m_data;
			while (i++)
				dat1 = dat1->next;
			return slNodeFwdIteratorBase(dat1);
		}
		template<typename intType>
		constexpr inline slNodeFwdIteratorBase<NodeType> &operator +=(intType i) {
			CM3D_ASSERT(i >= 0);
			while (i--)
				this->m_data = this->m_data->next;
			return *this;
		}
		template<typename intType>
		constexpr inline slNodeFwdIteratorBase<NodeType> &operator -=(intType i) {
			CM3D_ASSERT(i <= 0);
			while (i++)
				this->m_data = this->m_data->next;
			return *this;
		}
		constexpr inline bool operator >(const slNodeFwdIteratorBase<NodeType> other) const {
			auto dat1 = other.m_data;
			while ((dat1 = dat1->next))
				if (dat1 == this->m_data)
					return true;
			
			return false;
		}
		constexpr inline bool operator >=(const slNodeFwdIteratorBase<NodeType> other) const {
			return this->m_data == other.m_data || *this > other;
		}
		constexpr inline bool operator <(const slNodeFwdIteratorBase<NodeType> other) const {
			auto dat1 = this->m_data;
			while ((dat1 = dat1->next))
				if (dat1 == other.m_data)
					return true;
			
			return false;
		}
		constexpr inline bool operator <=(const slNodeFwdIteratorBase<NodeType> other) const {
			return this->m_data == other.m_data || *this < other;
		}
	};
	
	template<typename T>
	class slNodeIterator: public slNodeFwdIteratorBase<SLNode<T>>
	{
	public:
		using nodeType = SLNode<T>;
		using valueType = T;
		
		inline slNodeIterator() noexcept = default;
		inline slNodeIterator(const slNodeIterator<T> &it) noexcept = default;
		inline slNodeIterator(slNodeIterator<T> &&rv) = default;
		
		constexpr inline slNodeIterator(nodeType *data) noexcept:
			slNodeFwdIteratorBase<nodeType>(data) {}
	};
	
	template<typename T>
	class slNodeConstIterator: public slNodeFwdIteratorBase<const SLNode<T>>
	{
	public:
		using nodeType = const SLNode<T>;
		using valueType = T;
		
		inline slNodeConstIterator() noexcept = default;
		inline slNodeConstIterator(const slNodeConstIterator<T> &it) noexcept = default;
		inline slNodeConstIterator(slNodeConstIterator<T> &&rv) = default;
		
		constexpr inline slNodeConstIterator(nodeType *data) noexcept:
			slNodeFwdIteratorBase<nodeType>(data) {}
	};
}

#endif // CM3D_BASIC_QUEUE_ITERATORS_HPP
