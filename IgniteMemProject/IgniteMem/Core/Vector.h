#pragma once

#include <cassert>
#include <cstdint>
#include <utility>

namespace ignite::mem
{

template <typename T>
class vector
{
public:
    explicit vector(T* startAddress, const uint32_t capacity) noexcept;
    ~vector() = default;

    vector(const vector&)             = delete;
    vector(vector&&)                  = delete;
    vector& operator=(vector&&)       = delete;
    vector& operator=(const vector&)  = delete;

    template <typename... Args>
    void emplace_back(Args... args) noexcept;

    [[nodiscard]] inline T pop_back() noexcept { assert(mIndex > 0); return mStartAddress[mIndex--]; }
    [[nodiscard]] inline uint32_t size() const noexcept { return mIndex; }
private:
    T*       mStartAddress = nullptr;
    uint32_t mCapacity     = 0;
    uint32_t mIndex        = 0;
};

/*
   ===================================================================================================
                                                                                                        */

template <typename T>
inline vector<T>::vector(T* startAddress, const uint32_t capacity)
    : mStartAddress(startAddress), mCapacity(capacity)
{ /* Empty. */ }

template <typename T>
template <typename... Args>
void vector<T>::emplace_back(Args... args)
{
    assert(mIndex <= mCapacity && "Capacity of vector is less than the required amount.");
    mStartAddress[mIndex++] = T{ std::forward<Args>(args)... };
}

} // Namespace ignite::mem;