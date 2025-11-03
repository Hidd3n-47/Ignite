#pragma once

#include "WeakRef.h"

namespace std
{

template <typename T>
struct hash<ignite::mem::WeakRef<T>>
{
    [[nodiscard]] inline size_t operator()(const ignite::mem::WeakRef<T>& ref) const noexcept
    {
        return std::hash<void*>{}(ref.GetRawPtr());
    }
};

} // Namespace std.