#ifndef CM3D_OBJECT_MODULE_BASE_HPP
#define CM3D_OBJECT_MODULE_BASE_HPP

// The base module to be inherited by other Object modules

#include <cstdint>

namespace cm3d
{
	namespace Modules
	{
		class Base
		{
		protected:
			uint32_t id;
		public:
			inline uint32_t getId() { return id; }
			inline Base(uint32_t id): id(id) {}
		};
	}
}

#endif
