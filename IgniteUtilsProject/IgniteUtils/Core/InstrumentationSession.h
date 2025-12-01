#pragma once

#include <mutex>
#include <thread>
#include <vector>
#include <fstream>

namespace ignite::utils
{

template<typename T>
class HeapAllocator
{
public:
    using value_type = T;

    HeapAllocator() = default;

    template<typename U>
    constexpr HeapAllocator(const HeapAllocator<U>&) noexcept {}

    T* allocate(std::size_t n)
    {
        if (n > std::allocator_traits<HeapAllocator>::max_size(*this)) 
        {
            throw std::bad_alloc();
        }

        return new (malloc(n * sizeof(T))) T{};
    }

    void deallocate(T* p, std::size_t) noexcept
    {
        p->~T();
        free(p);
    }
};

template <typename T, typename U>
[[nodiscard]] constexpr bool operator==(const HeapAllocator<T>, const HeapAllocator<U>) { return true; }

struct FunctionProfile
{
    const char* name;
    uint64_t    startTime;
    uint64_t    duration;
};

class InstrumentationSession
{
public:
    [[nodiscard]] inline static InstrumentationSession* Instance() { return mInstance; }

    static void Create();
    static void Destroy();

    void StartSession();
    void Run();
    void ProfileFunctionTime(const char* name, const uint64_t startTime, const uint64_t duration);
    void EndSession();
private:
    inline static InstrumentationSession* mInstance = nullptr;

    std::ofstream mFileOutput;
    bool mOutputtedTimeOnce = false;

    std::mutex  mFlushMutex;
    std::thread mFlushThread;
    std::atomic<bool> mRunning = true;
    std::vector<FunctionProfile, HeapAllocator<FunctionProfile>> mFunctionTimes;

    void Flush();
};

} // Namespace ignite::utils.