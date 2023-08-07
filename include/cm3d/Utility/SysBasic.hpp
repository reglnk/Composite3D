#ifndef CM3D_SYS_BASIC_HPP
#define CM3D_SYS_BASIC_HPP

#ifndef _WIN32
#  include <unistd.h>
#else
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

namespace cm3d
{
	namespace System
	{
		inline int sleepms(unsigned int t)
		{
#		ifndef _WIN32
			const unsigned int msec = t % 1000;
			const unsigned int sec = (t - msec) / 1000;
			const int sms = sleep(sec);
			const int sus = usleep(msec * 1000);
			return sms * 1000 + sus;
#		else
			Sleep(t);
			return t;
#		endif
		}
	}
}

#endif
