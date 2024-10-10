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
        
        int32_t getNumProcessors();
        int32_t getNumProcessorsAvail();
    }
}

#endif // CM3D_HARD_INFO_HPP