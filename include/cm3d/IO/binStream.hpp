#ifndef CM3D_IO_BINSTREAM_HPP
#define CM3D_IO_BINSTREAM_HPP

#include <cm3d/IO/binStreamR.hpp>
#include <cm3d/IO/binStreamW.hpp>

namespace cm3d::io
{
	class binStream: public binStreamR, public binStreamW
	{
	public:
		inline virtual ~binStream() {}
	};

	// the only difference is that seekg() here must not affect tellp() value
	// and vice versa. The name stands for 'parallel'
	class binStreamP: public binStream
	{
	public:
		inline virtual ~binStreamP() {}
	};
}

#endif
