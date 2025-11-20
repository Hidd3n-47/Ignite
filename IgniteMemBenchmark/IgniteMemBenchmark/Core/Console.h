#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct SDL_Window;
struct SDL_Renderer;

namespace ignite::bench
{

class Console
{
public:
    [[nodiscard]] inline static Console* Instance() { return mInstance; }

    Console(const Console&)            = delete;
    Console(Console&&)                 = delete;
    Console& operator=(Console&&)      = delete;
    Console& operator=(const Console&) = delete;

    static void Init();
    static void Destroy();

    inline void StopRunning() { mRunning = false; }

    void Run();
    void Update();
    void Render() const;
private:
    Console();
    ~Console();

    static Console* mInstance;

    bool          mRunning;
    SDL_Window*   mWindow;
    SDL_Renderer* mRenderer;
};

} // Namespace ignite::bench.
