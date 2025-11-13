#pragma once

#include <cstdint>

namespace ignite::mem
{

enum class DebugMemoryHexValues : uint8_t
{
    FREED = 0xFF,
    NEWLY_ALLOCATED = 0xAA,
    UNALLOCATED = 0xF0
};

} // Namespace ignite::mem.