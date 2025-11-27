#pragma once

#include <IgniteMem/Core/WeakHandle.h>

#include "IgniteEngine/Math/Vec2.h"
#include "IgniteEngine/Core/OrthoCamera.h"

#ifdef DEV_CONFIGURATION
#include "Logger/Log.h"
#endif // DEV_CONFIGURATION.

struct SDL_Window;

namespace ignite
{

class Scene;
class Renderer;
class FontRenderer;
class InputManager;
class TextureManager;
class ParticleManager;
class CollisionHandler;

/**
 * @class Engine: The engine class is responsible for the game logic and the running of the game.
 */
class Engine
{
public:
    Engine()  = default;
    ~Engine() = default;

    [[nodiscard]] inline static mem::WeakHandle<Engine> Instance() { return mem::WeakHandle{ mInstance }; }

    Engine(const Engine&)             = delete;
    Engine(Engine&&)                  = delete;
    Engine& operator=(Engine&&)       = delete;
    Engine& operator=(const Engine&)  = delete;

    /**
     * @brief Create an instance of the engine.
     * @note This should only happen once per application lifetime. Returns existing instance if called multiple times.
     * @return The instance of the engine that is created.
     */
    static mem::WeakHandle<Engine> CreateEngine();

    /**
     * @brief Initialize the engine and its requirements.
     */
    void Init();
    /**
     * @brief Run the core gameplay loop of the game.
     */
    void Run();
    /**
     * @brief Destroy and clean up used resources by the engine.
     */
    void Destroy() const;

    /**
     * @brief Update the \c GameObjects and \c Components.
     */
    void Update();
    /**
     * @brief An update that is used to preform actions that affect other objects, example, changing a scene.
     */
    void PostUpdate();
    /**
     * @brief Render loop for the client.
     */
    void Render() const;

    /**
     * @brief Close the game by stopping running of the main run loop.
     */
    inline void CloseGame() { mRunning = false; }

    /**
     * @brief Set the scene that will become active in the next frame.
     * @note: This occurs next frame as the current game object in the scene could be used, therefore only change next frame once nothing is using them.
     * @param scene A weak reference to the scene that will become active next frame.
     */
    inline void SetSceneToChangeTo(const mem::WeakHandle<Scene> scene) { mSceneToChangeTo = scene; }

    [[nodiscard]] inline mem::WeakHandle<InputManager>     GetInputManager()     const { return mem::WeakHandle{ mInputManager }; }
    [[nodiscard]] inline mem::WeakHandle<FontRenderer>     GetFontRenderer()     const { return mem::WeakHandle{ mFontRenderer }; }
    [[nodiscard]] inline mem::WeakHandle<TextureManager>   GetTextureManager()   const { return mem::WeakHandle{ mTextureManager }; }
    [[nodiscard]] inline mem::WeakHandle<CollisionHandler> GetCollisionHandler() const { return mem::WeakHandle{ mCollisionHandler }; }
    [[nodiscard]] inline mem::WeakHandle<ParticleManager>  GetParticleManager()  const { return mem::WeakHandle{ mParticleManager }; }
    [[nodiscard]] inline const OrthoCamera& GetCamera() const { return mCamera; }

    static constexpr uint32_t TARGET_FRAMES = 120;
    static constexpr uint64_t TARGET_FRAME_TIME_MS{ static_cast<uint64_t>(1000.0 / static_cast<double>(TARGET_FRAMES)) };

    //const Vec2 DEFAULT_SCREEN_SIZE{ 1280.0f, 720.0f };
    const Vec2 DEFAULT_SCREEN_SIZE{ 1920.0f, 1080.0f };

#ifdef DEV_CONFIGURATION
    Log logger{"IgniteEngine"};
#endif // DEV_CONFIGURATION.
private:
    static Engine* mInstance;

    bool mRunning = false;

    FontRenderer*     mFontRenderer;
    InputManager*     mInputManager;
    TextureManager*   mTextureManager;
    CollisionHandler* mCollisionHandler;
    ParticleManager*  mParticleManager;

    SDL_Window*   mWindow   = nullptr;
    Renderer*     mRenderer = nullptr;

    mem::WeakHandle<Scene> mActiveScene;
    mem::WeakHandle<Scene> mSceneToChangeTo;

    OrthoCamera mCamera;

    void StartFrame();
    void EndFrame();

    uint64_t mStartFrameTime{ 0 };
    float    mDeltaTime { TARGET_FRAME_TIME_MS / 1000.0f };
};

} // Namespace ignite.
