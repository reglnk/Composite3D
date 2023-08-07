#ifndef CM3D_WORLD_HPP
#define CM3D_WORLD_HPP

#include <cm3d/Types/UniqueStorage.hpp>
#include <cm3d/Core/Object.hpp>

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