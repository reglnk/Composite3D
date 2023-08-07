#ifndef CM3D_HARD_INFO_HPP
#define CM3D_HARD_INFO_HPP

#include <cstddef>
#include <cstdint>

namespace cm3d
{
    namespace HardInfo
    {
        size_t getTotalSystemMemory();
        size_t getFreeSystemMemory();
        
        int32_t getNumCPUPackages();
        int32_t getNumCPUCores();
        int32_t getNumCPUThreads();

        // 127 symbols followed by '\0' may be written.
        // Returns length of written string.
        unsigned int getCPUname(char *info);
    }
}

#endif // CM3D_HARD_INFO_HPP