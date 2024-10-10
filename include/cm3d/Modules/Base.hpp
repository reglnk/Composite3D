#ifndef CM3D_OBJECT_MODULE_BASE_HPP
#define CM3D_OBJECT_MODULE_BASE_HPP

// The base module to be inherited by other Object modules

#include <cm3d/IO/binStream.hpp>

#include <cstdint>
#include <cstddef>

namespace cm3d::Modules
{
	class Base
	{
	public:
		inline virtual uint32_t getId() const { return -1; }

		inline virtual size_t binSize() const { return 0; }

		inline virtual void binSerialize(io::binStreamW &s) const {}

		inline virtual void binDeserialize(io::binStreamR &s) {}

		inline virtual ~Base() {}
	};
}

#endif
