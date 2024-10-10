#ifndef CM3D_BASIC_QUEUE_HPP
#define CM3D_BASIC_QUEUE_HPP

/* ==================================================
 * A minimalistic singly-linked queue without checks.
**/

#include <cm3d/Types/SLNode.hpp>
#include <cm3d/Types/SLNodeIterators.hpp>
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
		CM3D_CXX14_CONSTEXPR_INLINE cIterator begin() const {
			return cIterator(begNode);
		}
		CM3D_CXX14_CONSTEXPR_INLINE Iterator begin() {
			return Iterator(begNode);
		}
		CM3D_CXX14_CONSTEXPR_INLINE cIterator last() const {
			return cIterator(endNode);
		}
		CM3D_CXX14_CONSTEXPR_INLINE Iterator last() {
			return Iterator(endNode);
		}
		CM3D_CXX14_CONSTEXPR_INLINE Iterator end() const {
			return Iterator(nullptr);
		}
		CM3D_CXX14_CONSTEXPR_INLINE size_t length() const {
			return len;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const T &front() const {
			return begNode->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE const T &back() const {
			return endNode->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE T &front() {
			return begNode->data;
		}
		CM3D_CXX14_CONSTEXPR_INLINE T &back() {
			return endNode->data;
		}
	};
}

#endif
