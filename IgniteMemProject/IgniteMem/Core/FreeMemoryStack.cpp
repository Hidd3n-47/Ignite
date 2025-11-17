#include "FreeMemoryStack.h"

namespace ignite::mem
{

FreeMemoryStack::FreeMemoryStack(std::byte** startAddress, const uint32_t capacity)
    : mFreeMemoryStack(startAddress, capacity)
{
    // Empty.
}

std::byte* FreeMemoryStack::Pop()
{
    if (mFreeMemoryStack.size() > 0)
    {
        return mFreeMemoryStack.pop_back();
    }

    return nullptr;
}

void FreeMemoryStack::FreeAddress(const std::byte* address)
{
    mFreeMemoryStack.emplace_back(address);
}

} // Namespace ignite::mem.