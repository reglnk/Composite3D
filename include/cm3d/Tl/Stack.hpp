#ifndef CM3D_BASIC_STACK_HPP
#define CM3D_BASIC_STACK_HPP

#include <cm3d/Tl/SLNode.hpp>
#include <cm3d/Tl/SLNodeIterators.hpp>
#include <cm3d/Utility/Std.hpp>

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
		constexpr inline size_t length() const {
			return mLen;
		}
		constexpr inline size_t size() const {
			return mLen;
		}
		constexpr inline cIterator begin() const {
			return cIterator(topNode);
		}
		constexpr inline Iterator begin() {
			return Iterator(topNode);
		}
		constexpr inline Iterator end() const {
			return Iterator(nullptr);
		}
		constexpr inline const T &back() const {
			return topNode->data;
		}
		constexpr inline T &back() {
			return topNode->data;
		}
		constexpr inline operator bool() {
			return topNode;
		}
	};
}

#endif
