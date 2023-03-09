#ifndef CM3D_BASIC_STACK_HPP
#define CM3D_BASIC_STACK_HPP

#include <Types/SLNode.hpp>
#include <Types/SLNodeIterators.hpp>
#include <Utility/Std.hpp>

#include <cstddef>
#include <malloc.h>

namespace cm3d
{
	template<typename T>
	class Stack
	{
	protected:
		SLNode<T> *topNode;
		size_t mLen;

	public:
		using valueType = T;

		using Iterator = slNodeIterator<T>;
		using cIterator = slNodeIterator<const T>;
		
		inline Stack() noexcept: topNode(nullptr), mLen(0) {}
		inline Stack(Stack<T> &&other) noexcept: topNode(other.topNode), mLen(other.mLen) {
			other.topNode = nullptr;
		}
		inline Stack(const Stack<T> &other) = delete;
		
		inline ~Stack() noexcept {
			while (topNode)
				pop();
		}

		inline void push(const T &data)
		{
			auto node = new SLNode<T>(data);
			node->next = topNode;
			topNode = node;
			++mLen;
		}
		inline SLNode<T> *detach() noexcept
		{
			SLNode<T> *node = topNode;
			topNode = topNode->next;
			--mLen;
			return node;
		}
		inline void pop() noexcept {
			delete detach();
		}
		inline void clear() noexcept {
			while (topNode)
				pop();
		}
		CM3D_CXX14_CONSTEXPR_INLINE size_t length() const {
			return mLen;
		}
		CM3D_CXX14_CONSTEXPR_INLINE size_t size() const {
			return mLen;
		}
		CM3D_CXX14_CONSTEXPR_INLINE cIterator begin() const {
			return cIterator(topNode);
		}
		CM3D_CXX14_CONSTEXPR_INLINE Iterator begin() {
			return Iterator(topNode);
		}
		CM3D_CXX14_CONSTEXPR_INLINE Iterator end() const {
			return Iterator(nullptr);
		}
		CM3D_CXX14_CONSTEXPR_INLINE const T &back() const {
			return topNode->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE T &back() {
			return topNode->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE operator bool() {
			return topNode;
		}
	};
}

#endif
