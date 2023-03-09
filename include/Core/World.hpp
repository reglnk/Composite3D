#ifndef CM3D_WORLD_HPP
#define CM3D_WORLD_HPP

#include <Types/UniqueStorage.hpp>
#include <Core/Object.hpp>

namespace cm3d
{
	class World
	{
		// @todo widen features
	public:
		UniqueStorage<Object> obj;
	};
}

#endif