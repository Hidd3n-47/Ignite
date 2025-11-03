#pragma once

namespace ignite::mem
{

/**
 * @class WeakRef: A class used to hold a reference to a pointer such that the ownership is not transferred.<br/>
 * Having a \c Ref to a pointer allows you to access the pointer and its methods but not call \c delete on it.
 * @tparam T: The class of the pointer.
 */
template <typename T>
class WeakRef
{
public:
    WeakRef()      = default;
    ~WeakRef()     = default;

    WeakRef(const WeakRef&)             = default;
    WeakRef(WeakRef&&)                  = default;
    WeakRef& operator=(WeakRef&&)       = default;
    WeakRef& operator=(const WeakRef&)  = default;

    inline explicit WeakRef(T* reference)
        : mReferencePtr(reference)
    { /* Empty. */ }

    /**
     * A method used to check if the pointer is still valid.
     * @return \c true if the reference is still valid, \c false otherwise.
     */
    [[nodiscard]] inline bool IsRefValid() const { return mReferencePtr != nullptr; }
    /**
     * A method used to invalidate a pointer.
     */
    inline void Invalidate() { mReferencePtr = nullptr;  }

    inline T* GetRawPtr() const { return mReferencePtr; }

    /**
     * @brief Cast a \c WeakRef to be a \c WeakRef of a different class. Useful when trying to cast for inheritance.
     * @tparam CastType The class that the reference is being cast to.
     * @return A \c WeakRef that has been cast to the type \c CastType. Note, this \c WeakRef could be invalid if the cast failed, and hence should be checked if its is valid before use.
     */
    template <typename CastType>
    [[nodiscard]] inline WeakRef<CastType> Cast() { return WeakRef<CastType>{ reinterpret_cast<CastType*>(mReferencePtr) }; }

    inline T* operator->()              { return mReferencePtr; }
    inline const T* operator->() const  { return mReferencePtr; }
    inline bool operator==(const WeakRef<T>& other) const { return mReferencePtr == other.mReferencePtr; }
    inline bool operator<(const WeakRef<T>& other)  const { return mReferencePtr < other.mReferencePtr; }
private:
    T* mReferencePtr = nullptr;
};

} // Namespace ignite::mem.
