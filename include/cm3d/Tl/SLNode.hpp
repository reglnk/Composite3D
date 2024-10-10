#ifndef CM3D_SL_NODE_HPP
#define CM3D_SL_NODE_HPP

// A template for singly-linked node.

#include <malloc.h>

namespace cm3d
{
	template<typename T>
	struct SLNode
	{
		using valueType = T;
		
		T data;
		SLNode<T> *next;

		inline SLNode(): next(nullptr) {}
		inline SLNode(SLNode<T> const &other): data(other.data), next(other.next) {}
		inline SLNode(SLNode<T> &&rv): data(rv.data), next(rv.next) {}
		
		inline SLNode(const T &data): data(data), next(nullptr) {}
	};
}

#endif
