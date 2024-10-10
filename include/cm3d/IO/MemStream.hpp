#ifndef CM3D_IO_MEMSTREAM_HPP
#define CM3D_IO_MEMSTREAM_HPP

#include <cm3d/IO/binStream.hpp>
#include <cm3d/Utility/Debug.hpp>

#include <cstdio>
#include <limits>

// @todo: remove WET

namespace cm3d::io
{
	class MemStream: public binStreamP
	{
	protected:
		uint8_t *st_in;
		uint8_t *st_out;

		size_t in_off;
		size_t out_off;

		size_t st_size;
		// @todo flags

	public:
		MemStream(uint8_t *mem, size_t size = SIZE_MAX):
			st_in(mem), st_out(mem), in_off(0), out_off(0), st_size(size)
		{}

		inline void read(void *dst, size_t count) override
		{
			if (in_off + count > st_size)
				count = st_size - in_off; // @todo set EOF flag

			CM3D_ASSERT(in_off <= st_size);
			memcpy(dst, st_in + in_off, count);
			in_off += count;
		}

		inline void write(void const *src, size_t count) override
		{
			if (out_off + count > st_size)
				count = st_size - out_off; // @todo set EOF flag

			CM3D_ASSERT(out_off <= st_size);
			memcpy(st_out + out_off, src, count);
			out_off += count;
		}

		inline int64_t tellg() override {
			return in_off;
		}

		inline int64_t tellp() override {
			return out_off;
		}
	};

	class MemStreamR: public binStreamR
	{
	protected:
		uint8_t const *st_in;
		size_t st_size;
		size_t in_off;
		// @todo flags

	public:
		MemStreamR(uint8_t const *mem, size_t size = SIZE_MAX):
			st_in(mem), st_size(size), in_off(0) {}

		inline void read(void *dst, size_t count) override
		{
			if (in_off + count > st_size)
				count = st_size - in_off; // @todo set EOF flag

			CM3D_ASSERT(in_off <= st_size);
			memcpy(dst, st_in + in_off, count);
			in_off += count;
		}

		inline int64_t tellg() override {
			return in_off;
		}
	};

	class MemStreamW: public binStreamW
	{
	protected:
		uint8_t *st_out;
		size_t st_size;
		size_t out_off {0};
		// @todo flags

	public:
		MemStreamW(uint8_t *mem, size_t size = SIZE_MAX):
			st_out(mem), st_size(size), out_off(0) {}

		inline void write(void const *src, size_t count) override
		{
			if (out_off + count > st_size)
				count = st_size - out_off; // @todo set EOF flag

			CM3D_ASSERT(out_off <= st_size);
			memcpy(st_out + out_off, src, count);
			out_off += count;
		}

		inline int64_t tellp() override {
			return out_off;
		}
	};
}

#endif
