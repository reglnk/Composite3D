#ifndef CM3D_TYPE_SL_LIST_HPP
#define CM3D_TYPE_SL_LIST_HPP

#include <cm3d/Types/Queue.hpp>
#include <cm3d/Types/SLNode.hpp>
#include <cm3d/Types/SLNodeIterators.hpp>

#include <cm3d/Utility/Std.hpp>

#include <cstddef>
#include <malloc.h>

namespace cm3d
{
	template<typename T>
	class SLList: public Queue<T>
	{
	public:
		using valueType = T;

		using Iterator = slNodeIterator<T>;
		using cIterator = slNodeIterator<const T>;

		inline void push(const T &data) = delete;
		inline SLNode<T> *detach() noexcept = delete;
		inline void pop() noexcept = delete;

		inline Iterator at(size_t index) noexcept
		{
			if (index == this->len - 1)
				return this->endNode;
			auto iter = this->begin();
			while (index--)
				++iter;
			return iter;
		}
		inline cIterator at(size_t index) const noexcept
		{
			if (index == this->len - 1)
				return this->endNode;
			auto iter = this->begin();
			while (index--)
				++iter;
			return iter;
		}
		inline T &operator [](size_t index) noexcept {
			return *at(index);
		}
		inline const T &operator [](size_t index) const noexcept {
			return *at(index);
		}

		inline void pushFront(const T &data) {
			auto newNode = new SLNode<T>(data);
			newNode->next = this->begNode;
			this->begNode = newNode;
			++this->len;
		}
		inline void pushBack(const T &data) {
			Queue<T>::push(data);
		}
		inline SLNode<T> *insert(size_t index, const T &data)
		{
			if (index == this->len) {
				pushBack(data);
				return this->endNode;
			}
			if (index == 0) {
				pushFront(data);
				return this->begNode;
			}
			SLNode<T> *ins = new SLNode<T>(data);
			SLNode<T> *rep = at(index - 1);
			ins->next = rep->next;
			rep->next = ins;

			++this->len;
			return ins;
		}
		inline Iterator detach(size_t index) noexcept
		{
			if (!index)
				return Queue<T>::detach();
			SLNode<T> *prev = at(index - 1);
			SLNode<T> *det = prev->next;
			prev->next = det->next;
			if (det == this->endNode)
				this->endNode = prev;
			--this->len;
			return det;
		}
		inline SLNode<T> *detachFront() noexcept {
			return Queue<T>::detach();
		}
		inline SLNode<T> *detachBack() noexcept {
			return detach(this->len - 1);
		}

		inline void popFront() noexcept {
			delete detachFront();
		}
		inline void popBack() noexcept {
			delete detachBack();
		}
		inline void erase(size_t index) noexcept {
			delete detach(index).data();
		}
	};
}

#endif
