#ifndef CM3D_OBJECT_MODULE_POSITION_HPP
#define CM3D_OBJECT_MODULE_POSITION_HPP

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
		class Position: public Base
		{
		public:
			static constexpr uint32_t Id = 0;
			inline uint32_t getId() const override {
				return Id;
			}
			inline size_t binSize() const override {
				return (sizeof(sInteger) + sizeof(sReal)) * 3;
			}

		public:
			Vector3 value;
			
		public:
			inline ~Position() override = default;
		};
	}
}

#endif
