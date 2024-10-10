#include <cm3d/Core/HardInfo.hpp>

#ifndef _WIN32
#	include <unistd.h>
#	include <sys/sysinfo.h>
#	include <fstream>
#	include <cstring>
#else
#	include <sysinfoapi.h>
#endif

namespace cm3d::HardInfo
{
#ifdef _WIN32
	size_t getTotalSystemMemory()
	{
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);
		return status.ullTotalPhys;
	}
	
	size_t getFreeSystemMemory()
	{
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);
		return status.ullAvailPhys;
	}
	
	int getNumProcessors()
	{
		SYSTEM_INFO inf;
		GetSystemInfo(&inf);
		return inf.dwNumberOfProcessors;
	}
	
	int getNumProcessorsAvail()
	{
		SYSTEM_INFO inf;
		GetSystemInfo(&inf);
		
		DWORD_PTR msk = 0x1;
		int active = 0;
		for (DWORD i = 0; i < inf.dwNumberOfProcessors; ++i)
		{
			active += (inf.dwActiveProcessorMask & msk) != 0;
			msk <<= 1;
		}
		return active;
	}
	
#else
	size_t getTotalSystemMemory() {
		return get_phys_pages() * getpagesize();
	}
	
	size_t getFreeSystemMemory() {
		return get_avphys_pages() * getpagesize();
	}
	
	int getNumProcessors() {
		return get_nprocs();
	}
	
	int getNumProcessorsAvail() {
		return get_nprocs_conf();
	}
#endif
}
