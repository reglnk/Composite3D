#ifndef CM3D_OBJECT_MODULE_NONRIGID_PHYSICS_HPP
#define CM3D_OBJECT_MODULE_NONRIGID_PHYSICS_HPP

#include <cm3d/Tl/Aliases.hpp>
#include <cm3d/Tl/DynArray.hpp>
#include <cm3d/Tl/Vector3.hpp>

#include <cm3d/Modules/Base.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <map>

namespace cm3d
{
	namespace Modules
	{
		class NRPhysics: public Base
		{
		public:
			static constexpr uint32_t Id = 2;
			inline uint32_t getId() const override {
				return Id;
			}
			
		protected:
			Vector3 position;
			DynArray<glm::vec<3, sReal>> vertices;
			DynArray<glm::uvec2> struts;
			
		public:
			inline ~NRPhysics() override = default;
		};
	}
}

#endif
