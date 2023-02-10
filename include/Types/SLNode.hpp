#ifndef CM3D_SL_NODE_HPP
#define CM3D_SL_NODE_HPP

// A template for singly-linked node.

#include <malloc.h>

namespace cm3d
{
	template<typename T>
	struct SLNode
	{
		T data;
		SLNode<T> *next;

		inline SLNode(): next(nullptr) {}
		inline SLNode(SLNode<T> const &other) = default;
		inline SLNode(SLNode<T> &&rv) = default;
		
		inline SLNode(T data): data(data), next(nullptr) {}
		inline SLNode(SLNode<T> *other):
			data(other->data), next(other) {}
	};
}

#endif