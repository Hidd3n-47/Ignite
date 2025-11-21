#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>

namespace ignite::mem
{

template <typename T>
class Stack
{
public:
    //Stack() = default;
    Stack(const uint32_t maxSize);
    ~Stack();

    [[nodiscard]] T pop();
    void push(const T value);

    [[nodiscard]] inline T* data()           const { return mArray; }
    [[nodsicard]] inline bool empty()        const { return mIndex == 0; }
    [[nodiscard]] inline uint32_t size()     const { return mIndex; }
    [[nodiscard]] inline uint32_t capacity() const { return mCapacity; }
private:
    T* mArray = nullptr;
    uint32_t mIndex    { 0 };
    uint32_t mCapacity { 0 };
};

/*
   =================================================================================================
                                                                                                      */

template <typename T>
Stack<T>::Stack(const uint32_t maxSize)
    : mCapacity(maxSize)
{
    mArray = (T*)malloc(sizeof(T) * maxSize);
}

template <typename T>
Stack<T>::~Stack()
{
    free(mArray);
}


template <typename T>
T Stack<T>::pop()
{
    //assert(mArray && "Stack not created!");
    assert(mIndex > 0 && "Need to have values to be able to pop.");

    return mArray[--mIndex];
}

template <typename T>
void Stack<T>::push(const T value)
{
    //assert(mArray && "Stack not created!");
    assert(mIndex < mCapacity && "Need to have capacity to be able to push.");
    mArray[mIndex++] = value;
}

} // Namespace ignite::mem.