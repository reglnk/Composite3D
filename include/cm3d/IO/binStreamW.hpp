#ifndef CM3D_IO_BINWSTREAM_HPP
#define CM3D_IO_BINWSTREAM_HPP

#include <cm3d/IO/common.hpp>

#include <cstddef>
#include <cstring>
#include <cstdint>

namespace cm3d::io
{
	class binStreamW
	{
	public:
		inline virtual void write(void const *src, size_t count) { throw nullptr; }

		inline virtual void seekp(int64_t off, Way way = Way::set) { throw nullptr; }

		inline virtual int64_t tellp() { throw nullptr; }

		template<typename T>
		inline binStreamW &operator <<(T const &src) {
			write(&src, sizeof(T));
			return *this;
		}

		inline binStreamW &operator <<(const char *str) {
			write(str, strlen(str));
			return *this;
		}
		inline binStreamW &operator <<(char *str) {
			return operator <<((const char *)str);
		}

		inline virtual ~binStreamW() {}
	};
}

#endif
