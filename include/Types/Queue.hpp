#ifndef CM3D_BASIC_QUEUE_HPP
#define CM3D_BASIC_QUEUE_HPP

/* ==================================================
 * A minimalistic singly-linked queue without checks.
**/

#include <Types/SLNode.hpp>
#include <Types/SLNodeIterators.hpp>
#include <Utility/Std.hpp>

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

		inline void push(const T &data) {
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
		inline SLNode<T> *detach() noexcept {
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
		CM3D_CXX14_CONSTEXPR_INLINE const slNodeIterator<T> begin() const {
			return slNodeIterator<T>(begNode);
		}
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> begin() {
			return slNodeIterator<T>(begNode);
		}
		CM3D_CXX14_CONSTEXPR_INLINE slNodeIterator<T> end() const {
			return slNodeIterator<T>(nullptr);
		}
		CM3D_CXX14_CONSTEXPR_INLINE const size_t length() const {
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
