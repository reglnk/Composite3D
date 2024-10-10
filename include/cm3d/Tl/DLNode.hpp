#ifndef CM3D_DL_NODE_HPP
#define CM3D_DL_NODE_HPP

// A template for doubly-linked node.
// @todo operators

#include <malloc.h>

namespace cm3d
{
	template<typename T>
	struct DLNode
	{
		T data;
		DLNode<T> *next;
		DLNode<T> *prev;

		DLNode() = default;
		DLNode(const T &data): data(data) {}

		DLNode(const DLNode<T> &other): data(other.data) {}
		DLNode(DLNode<T> &&other) = default;

		// unlink from both sides (seamlessly)
		constexpr inline DLNode<T> &unlink() {
			next->prev = this->prev;
			prev->next = this->next;
			next = NULL;
			prev = NULL;
			return *this;
		}
		// detach next node
		constexpr inline DLNode<T> &detachNext(int) {
			next->prev = NULL;
			this->next = NULL;
			return *this;
		}
		// detach previous node
		constexpr inline DLNode<T> &detachPrev() {
			prev->next = NULL;
			this->prev = NULL;
			return *this;
		}
		// insert to the front side of other (must have already linked prev node)
		constexpr inline DLNode<T> &insFront(DLNode<T> *other) {
			this->prev = other->prev;
			this->next = other;
			prev->next = this;
			other->prev = this;
			return *this;
		}
		constexpr inline DLNode<T> &insFront(DLNode<T> &other) {
			return this->insFront(&other);
		}
		// insert to the front side of other (assume it hasn't prev node)
		constexpr inline DLNode<T> &linkFront(DLNode<T> *other) {
			this->next = other;
			other->prev = this;
			return *this;
		}
		constexpr inline DLNode<T> &linkFront(DLNode<T> &other) {
			return this->linkFront(&other);
		}
		// insert to the back side of other (must have already linked next node)
		constexpr inline DLNode<T> &insBack(DLNode<T> *other) {
			this->next = other->next;
			this->prev = other;
			next->prev = this;
			other->next = this;
			return *this;
		}
		constexpr inline DLNode<T> &insBack(DLNode<T> &other) {
			return this->insBack(&other);
		}
		// insert to the back side of other (assume it hasn't next node)
		constexpr inline DLNode<T> &linkBack(DLNode<T> *other) {
			this->prev = other;
			other->next = this;
			return *this;
		}
		constexpr inline DLNode<T> &linkBack(DLNode<T> &other) {
			return this->linkBack(&other);
		}
	}; 
}

#endif