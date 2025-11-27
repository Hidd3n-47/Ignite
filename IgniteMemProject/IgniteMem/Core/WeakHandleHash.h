#pragma once

#include "WeakHandle.h"

namespace std
{

template <typename T>
struct hash<ignite::mem::WeakHandle<T>>
{
    [[nodiscard]] inline size_t operator()(const ignite::mem::WeakHandle<T>& ref) const noexcept
    {
        return std::hash<void*>{}(ref.GetRawPtr());
    }
};

} // Namespace std.