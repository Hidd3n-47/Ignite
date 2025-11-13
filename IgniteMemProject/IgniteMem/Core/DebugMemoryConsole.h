#pragma once

#include <vector>

#include "Node.h"

struct SDL_Window;
struct SDL_Renderer;

namespace ignite::mem
{

class DebugMemoryConsole
{
public:
    [[nodiscard]] inline static DebugMemoryConsole* Instance() { return mInstance; }

    DebugMemoryConsole(const DebugMemoryConsole&)            = delete;
    DebugMemoryConsole(DebugMemoryConsole&&)                 = delete;
    DebugMemoryConsole& operator=(DebugMemoryConsole&&)      = delete;
    DebugMemoryConsole& operator=(const DebugMemoryConsole&) = delete;

    static void Init();
    static void Destroy();

    inline void StopRunning() { mRunning = false; }

    void Run();
    void Update();
    void Render() const;
private:
    DebugMemoryConsole();
    ~DebugMemoryConsole();

    static DebugMemoryConsole* mInstance;

    bool mRunning;
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    static void UpdateMemoryBlockVector(std::vector<float>& vector, const Node* node, const uint64_t barSize);
};

} // Namespace ignite::mem;