#ifndef CM3D_BASIC_QUEUE_HPP
#define CM3D_BASIC_QUEUE_HPP

/* ===================================================
 * A minimalistic singly-linked queue, poor on checks.
**/

#include <cm3d/Tl/SLNode.hpp>
#include <cm3d/Tl/SLNodeIterators.hpp>
#include <cm3d/Utility/Std.hpp>

#include <cstddef>
#include <malloc.h>

namespace cm3d
{
	template<typename T>
	class Queue
	{
	protected:
		SLNode<T> *begNode;
		SLNode<T> *endNode;
		size_t len;

	public:
		using valueType = T;

		using Iterator = slNodeIterator<T>;
		using cIterator = slNodeIterator<const T>;
		
		inline Queue() noexcept: begNode(nullptr), endNode(nullptr), len(0) {}
		inline Queue(Queue<T> &other): begNode(nullptr), endNode(nullptr), len(other.len) {
			if (!len)
				return;
			for (auto iter: other)
				push(iter->data);
		}
		inline ~Queue() noexcept {
			while (len)
				pop();
		}

		inline void push(const T &data)
		{
			if (endNode) {
				endNode->next = new SLNode<T>(data);
				endNode = endNode->next;
			}
			else {
				endNode = new SLNode<T>(data);
				begNode = endNode;
			}
			++len;
		}
		inline SLNode<T> *detach() noexcept
		{
			SLNode<T> *bnode = begNode;
			begNode = begNode->next;
			if (!begNode)
				endNode = nullptr;
			--len;
			return bnode;
		}
		inline void pop() noexcept {
			delete detach();
		}
		constexpr inline cIterator begin() const {
			return cIterator(begNode);
		}
		constexpr inline Iterator begin() {
			return Iterator(begNode);
		}
		constexpr inline cIterator last() const {
			return cIterator(endNode);
		}
		constexpr inline Iterator last() {
			return Iterator(endNode);
		}
		constexpr inline Iterator end() const {
			return Iterator(nullptr);
		}
		constexpr inline size_t length() const {
			return len;
		}
		constexpr inline const T &front() const {
			return begNode->data;
		}
		constexpr inline const T &back() const {
			return endNode->data;
		}
		constexpr inline T &front() {
			return begNode->data;
		}
		constexpr inline T &back() {
			return endNode->data;
		}
	};
}

#endif
