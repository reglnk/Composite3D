#warning HardInfo.cpp isn't finished!

#include <Core/HardInfo.hpp>

#ifndef _WIN32
#	include <unistd.h>
#	include <sys/sysinfo.h>
#	include <fstream>
#	include <cstring>
#else
#	include <windows.h>
#endif


namespace cm3d
{
	namespace HardInfo
	{
#		ifndef _WIN32
		size_t getTotalSystemMemory() {
			return get_phys_pages() * getpagesize();
		}
		size_t getFreeSystemMemory() {
			return get_avphys_pages() * getpagesize();
		}
		uint32_t getNumCPUCores(uint64_t *info) {
			return get_nprocs();
		}
		uint32_t getNumCPUThreads(uint64_t *info) {
			return get_nprocs_conf();
		}
		unsigned int getCPUName(char *info)
		{
			// @todo
			return 0;
		}
#		else
		size_t getTotalSystemMemory()
		{
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);
			return status.ullTotalPhys;
		}
#		endif
	}
}
