#include "IgnitePch.h"
#include "Engine.h"

#include <SDL3/SDL.h>

#include <IgniteMem/Core/MemoryManager.h>

#include "Defines.h"
#include "EC/Scene.h"
#include "Core/Input/InputManager.h"
#include "Core/Rendering/Renderer.h"
#include "Physics/CollisionHandler.h"
#include "Core/Rendering/FontRenderer.h"
#include "Core/Rendering/TextureManager.h"
#include "Core/Rendering/ParticleManager.h"
#include "IgniteUtils/Core/InstrumentationSession.h"

void* operator new(std::size_t size)
{
    //std::cout << "using global new...\n";
    //return malloc(size);
    PROFILE_FUNC();
    return ignite::mem::MemoryManager::Instance()->New(size);
}

void operator delete(void* address) noexcept
{
    //std::cout << "using global delete...\n";
    //return free(address);
#ifdef DEV_CONFIGURATION
    if (ignite::utils::InstrumentationSession::Instance())
    {
        PROFILE_FUNC();
        ignite::mem::MemoryManager::Instance()->Delete(address);
        return;
    }
#endif // DEV_CONFIGURATION.

    ignite::mem::MemoryManager::Instance()->Delete(address);
}

namespace ignite
{

Engine* Engine::mInstance = nullptr;

mem::WeakRef<Engine> Engine::CreateEngine()
{
    if (mInstance)
    {
        DEBUG_ERROR("Failed to create engine as engine has already been created!");
        DEBUG_BREAK();

        return mem::WeakRef{ mInstance };
    }

    mem::MemoryManager::Init(128 * 1'024);

    DEBUG(utils::InstrumentationSession::Create());
    DEBUG(utils::InstrumentationSession::Instance()->StartSession());

    mInstance = mem::MemoryManager::Instance()->New<Engine>();
    DEBUG_INFO("Successfully created Ignite Engine.");

    return mem::WeakRef{ mInstance };
}

void Engine::Init()
{
    PROFILE_FUNC();

    DEBUG_INFO("Successfully initialized Ignite Engine.");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO))
    {
        DEBUG_ERROR("Failed to initialize SDL3.");
        return;
    }

    mInputManager = mem::MemoryManager::Instance()->New<InputManager>();

    const OrthoCameraValues cameraSize
    {
        .left   = -8.0f,
        .right  =  8.0f,
        .top    =  4.5f,
        .bottom = -4.5f,
        .screenDimensions = DEFAULT_SCREEN_SIZE
    };
    mCamera = OrthoCamera(cameraSize);

    mWindow = SDL_CreateWindow("Ignite", static_cast<int>(DEFAULT_SCREEN_SIZE.x), static_cast<int>(DEFAULT_SCREEN_SIZE.y), 0);

    if (!mWindow)
    {
        DEBUG_ERROR("Failed to create SDL3 window");
        return;
    }

    mRenderer       = mem::MemoryManager::Instance()->New<Renderer>(mWindow);
    mTextureManager = mem::MemoryManager::Instance()->New<TextureManager>(mRenderer->GetRendererBackend());
    mRenderer->SetTextureManagerRef(mem::WeakRef{ mTextureManager });

    mFontRenderer     = mem::MemoryManager::Instance()->New<FontRenderer>(mRenderer->GetRendererBackend());
    mCollisionHandler = mem::MemoryManager::Instance()->New<CollisionHandler>();
    mParticleManager  = mem::MemoryManager::Instance()->New<ParticleManager>();

    mRunning = true;
}

void Engine::Run()
{
    PROFILE_FUNC();

    while (mRunning)
    {
        StartFrame();

        mInputManager->Poll();

        Update();
        mCollisionHandler->Update();
        mParticleManager->Update(mDeltaTime);

        PostUpdate();

        Render();

#ifdef DEV_CONFIGURATION
        // Max frames displayed as: 120.123 <- total of 7 characters + 1 for null terminating character.
        char title[8];
        std::to_chars(title, title + sizeof(title), 1.0 / mDeltaTime, std::chars_format::fixed, 3);
        title[sizeof(title) - 1] = '\0';
        SDL_SetWindowTitle(mWindow, title);
#endif // DEV_CONFIGURATION.

        EndFrame();
    }
}

void Engine::Destroy() const
{
    {
        PROFILE_FUNC();

        mem::MemoryManager::Instance()->Delete(mParticleManager);

        mem::MemoryManager::Instance()->Delete(mCollisionHandler);

        mem::MemoryManager::Instance()->Delete(mFontRenderer);

        mem::MemoryManager::Instance()->Delete(mTextureManager);
        mem::MemoryManager::Instance()->Delete(mRenderer);

        SDL_DestroyWindow(mWindow);

        mem::MemoryManager::Instance()->Delete(mInputManager);

    }

    DEBUG_INFO("Successfully destroyed Ignite Engine.");

    DEBUG(utils::InstrumentationSession::Instance()->EndSession());
    DEBUG(utils::InstrumentationSession::Destroy());

    mem::MemoryManager::Instance()->Delete(mInstance);
    mInstance = nullptr;

    mem::MemoryManager::Destroy();
}

void Engine::Update()
{
    PROFILE_FUNC();

    if (mActiveScene.IsRefValid())
    {
        mActiveScene->SceneUpdate();
        mActiveScene->Update(mDeltaTime);
    }
}

void Engine::PostUpdate()
{
    PROFILE_FUNC();

    if (mSceneToChangeTo.IsRefValid())
    {
        mActiveScene = mSceneToChangeTo;
        mActiveScene->InitScene();

        mSceneToChangeTo.Invalidate();
    }
}

void Engine::Render() const
{
    PROFILE_FUNC();

    mRenderer->StartRender();

    if (mActiveScene.IsRefValid())
    {
        const mem::WeakRef<Renderer> renderer = mem::WeakRef{ mRenderer };
        mActiveScene->Render(renderer);
        mParticleManager->Render(renderer);
    }

    mRenderer->Render(mCamera);

    mFontRenderer->RenderFonts(mCamera);

    mRenderer->EndRender();
}

void Engine::StartFrame()
{
    PROFILE_FUNC();

    mStartFrameTime = SDL_GetPerformanceCounter();
}

void Engine::EndFrame()
{
    PROFILE_FUNC();
    const uint64_t now = SDL_GetPerformanceCounter();

    mDeltaTime = static_cast<float>(now - mStartFrameTime) / static_cast<float>(SDL_GetPerformanceFrequency());
}

} // Namespace ignite.
