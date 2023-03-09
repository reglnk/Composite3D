#ifndef CM3D_OBJECT_MODULE_NONRIGID_PHYSICS_HPP
#define CM3D_OBJECT_MODULE_NONRIGID_PHYSICS_HPP

#include <Types/Aliases.hpp>
#include <Types/DynArray.hpp>
#include <Types/Vector3.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <map>

namespace cm3d
{
	namespace Modules
	{
		class NRPhysics
		{
		protected:
			Vector3 position;
			DynArray<glm::vec<3, sReal>> vertices;
			DynArray<glm::uvec2> struts;
			
		public:
			const uint32_t id = 2;
		};
	}
}

#endif
