#pragma once

#include <fstream>

namespace ignite::utils
{

class InstrumentationSession
{
public:
    [[nodiscard]] inline static InstrumentationSession* Instance() { return mInstance; }

    static void Create();
    static void Destroy();

    void StartSession();
    void WriteTime(const char* name, const uint64_t startTime, const uint64_t duration);
    void EndSession();
private:
    inline static InstrumentationSession* mInstance = nullptr;

    std::ofstream mFileOutput;

    uint32_t mProfileCount = 0;
};

} // Namespace ignite::utils.