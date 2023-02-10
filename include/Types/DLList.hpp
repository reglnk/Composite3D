#warning DLList.hpp isn't currently finished!
// @todo

#ifndef CM3D_DL_LIST_HPP
#define CM3D_DL_LIST_HPP

#include <Types/DLNode.hpp>

#include <cstddef>
#include <malloc.h>

namespace cm3d
{
	template<typename T>
	class DLList
	{
		DLNode<T> *beg;
		DLNode<T> *end;
		size_t len;

	public:
		inline DLList(): len(0), beg(NULL), end(NULL) {}
		inline ~DLList() {
			// DLNode<T> *iter = beg;
			// do {
			// 	DLNode<T> next = iter->next;
			// 	free
			// }
		}

		inline T &getElement(size_t index)
		{
			DLNode<T> iter;
			if (index <= (len >> 1))
			{
				iter = beg;
				while (index--)
					++iter;
				return iter;
			} else {
				iter = end;
				index = len - index;
				while (--index)
					--iter;
			}
			return iter;
		}

		inline void insert(const T &data, size_t pos) {
			DLNode<T> nn = new DLNode<T>(data);
			if (!pos)
				pushFront(data);
			else if (pos == len)
				pushBack(data);
			else {
				DLNode<T> ins = getElement(pos);
				nn >> ins;
				++len;
			}
		}
		constexpr void pushFront(const T &data) {
			// @todo CHECK THIS
			beg = (new DLNode<T>(data))->linkFront(beg);
			++len;
		}
		constexpr void pushBack(const T &data) {
			// @todo CHECK THIS
			end = (new DLNode<T>(data))->linkBack(end);
			++len;
		}
		constexpr DLNode<T> &popFront(const T &data) {
			DLNode<T> newbeg = beg->next;
			// return del; @todo
		}
		constexpr void popBack(const T &data) {
			end = new DLNode<T>(data) << end;
			++len;
		}
		constexpr const size_t length() const {
			return len;
		}
		constexpr const size_t size() const {
			return len;
		}
		constexpr const T &front() const {
			return beg->data;
		}
		constexpr const T &back() const {
			return end->data;
		}
		constexpr T &front() {
			return beg->data;
		}
		constexpr T &back() {
			return end->data;
		}
	};
}

#endif
