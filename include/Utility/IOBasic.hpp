#ifndef CM3D_IO_BASIC_HPP
#define CM3D_IO_BASIC_HPP

#include <cstdio>

namespace cm3d
{
	static inline FILE *openStream(const char *path, const char *modes)	
	{
#ifndef _WIN32
		return fopen(path, modes);
#else
		FILE *stream;
		(void)fopen_s(&stream, path, modes);
		return stream;
#endif
	}
}

#endif