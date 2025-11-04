#pragma once

#include <IgniteMem/Core/WeakRef.h>

struct SDL_Window;

namespace ignite
{

class Scene;
class Renderer;
class InputManager;
class TextureManager;

/**
 * @class Engine: The engine class is responsible for the game logic and the running of the game.
 */
class Engine
{
public:
    [[nodiscard]] inline static Engine* Instance() { return mInstance; }

    Engine(const Engine&)             = delete;
    Engine(Engine&&)                  = delete;
    Engine& operator=(Engine&&)       = delete;
    Engine& operator=(const Engine&)  = delete;

    /**
     * @brief Create an instance of the engine.
     * @note This should only happen once per application lifetime. Returns existing instance if called multiple times.
     * @return The instance of the engine that is created.
     */
    static Engine* CreateEngine();

    /**
     * @brief Initialize the engine and its requirements.
     */
    void Init();
    /**
     * @brief Run the core gameplay loop of the game.
     */
    void Run() const;
    /**
     * @brief Destroy and clean up used resources by the engine.
     */
    void Destroy() const;

    /**
     * @brief Update the \c GameObjects and \c Components.
     */
    void Update() const;
    /**
     * @brief An update that is used to preform actions that affect other objects, example, changing a scene.
     */
    void PostUpdate();
    /**
     * @brief Render loop for the client.
     */
    void Render() const;

    inline void CloseGame() { mRunning = false; }

    //static constexpr uint32_t TARGET_FRAMES = 120;
    //static constexpr std::chrono::duration<float> TARGET_FRAME_TIME{ 1.0f / TARGET_FRAMES };

    //const Vec2 DEFAULT_SCREEN_SIZE{ 1920.0f, 1080.0f };
private:
    Engine()  = default;
    ~Engine() = default;

    static Engine* mInstance;

    bool mRunning = false;

    InputManager*   mInputManager;
    TextureManager* mTextureManager;

    SDL_Window*   mWindow   = nullptr;
    Renderer*     mRenderer = nullptr;

    mem::WeakRef<Scene> mActiveScene;
    mem::WeakRef<Scene> mSceneToChangeTo;
};

} // Namespace ignite.
