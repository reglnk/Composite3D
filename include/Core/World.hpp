#ifndef CM3D_WORLD_HPP
#define CM3D_WORLD_HPP

#include <Types/DynArray.hpp>

// Should represent a data structure containing game objects. @todo

namespace cm3d
{
	class World
	{
	public:
		DynArray<Object> obj;
	}
}

#endif