#ifndef CM3D_IO_BINRSTREAM_HPP
#define CM3D_IO_BINRSTREAM_HPP

#include <cm3d/IO/common.hpp>

#include <cstddef>
#include <cstdint>

namespace cm3d::io
{
	class binStreamR
	{
	public:
		inline virtual void read(void *dst, size_t count) { throw nullptr; }

		inline virtual void seekg(int64_t off, Way way = Way::set) { throw nullptr; }

		inline virtual int64_t tellg() { throw nullptr; }

		template<typename T>
		inline binStreamR &operator >>(T &dst) {
			read(&dst, sizeof(T));
			return *this;
		}

		inline virtual ~binStreamR() {}
	};
}

#endif
