#ifndef CM3D_TYPE_SL_LIST_HPP
#define CM3D_TYPE_SL_LIST_HPP

#include <cm3d/Tl/SLNode.hpp>
#include <cm3d/Tl/SLNodeIterators.hpp>

#include <cm3d/Utility/Std.hpp>

#include <cstddef>
#include <malloc.h>

namespace cm3d
{
	template<typename T>
	class SLList
	{
	protected:
		SLNode<T> *begNode;
		SLNode<T> *endNode;
		size_t len;
		
	public:
		using valueType = T;

		using Iterator = slNodeIterator<T>;
		using cIterator = slNodeConstIterator<T>;
		
		inline SLList() noexcept: begNode(nullptr), endNode(nullptr), len(0) {}
		
		inline SLList(SLList<T> &other): SLList<T>()
		{
			for (T const &s: other)
				pushBack(s);
		}
		
		inline ~SLList() noexcept {
			while (len)
				popFront();
		}

		inline Iterator at(size_t index) noexcept
		{
			if (index + 1 == this->len)
				return this->endNode;
			auto iter = this->begin();
			while (index--)
				++iter;
			return iter;
		}
		inline cIterator at(size_t index) const noexcept
		{
			if (index + 1 == this->len)
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

		inline void pushFront(const T &data)
		{
			auto newNode = new SLNode<T>(data);
			newNode->next = this->begNode;
			this->begNode = newNode;
			++this->len;
		}
		
		inline void pushBack(const T &data)
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
		
		inline SLNode<T> *detach(size_t index) noexcept
		{
			if (!index)
				return detachFront();
				
			SLNode<T> *prev = at(index - 1);
			SLNode<T> *det = prev->next;
			prev->next = det->next;
			
			if (det == this->endNode)
				this->endNode = prev;
			
			--this->len;
			return det;
		}
		inline SLNode<T> *detachFront() noexcept
		{
			SLNode<T> *bnode = begNode;
			begNode = begNode->next;
			
			if (!begNode)
				endNode = nullptr;
			
			--len;
			return bnode;
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
			delete detach(index);
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
		constexpr inline cIterator end() const {
			return cIterator(nullptr);
		}
		constexpr inline Iterator end() {
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
