#pragma once

#include "IgniteMem/Core/Vector.h"

namespace ignite::mem
{

class FreeMemoryStack
{
public:
    FreeMemoryStack(std::byte** startAddress, const uint32_t capacity);

    std::byte* Pop();
    void FreeAddress(const std::byte* address);
private:
    vector<std::byte*> mFreeMemoryStack;
};

} // Namespace ignite::mem.