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

#ifdef DEV_CONFIGURATION
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

namespace
{
static HANDLE gCurrentProcess = GetCurrentProcess();
} // Anonymous Namespace.
#endif // DEV_CONFIGURATION.

void* operator new(std::size_t size)
{
#ifdef DEV_CONFIGURATION
    void* stack[10];
    const USHORT stackCount = RtlCaptureStackBackTrace(1, 10, stack,nullptr);

    // We init it like this as static allocations can cause issues if symbols are not initialized so can't init 
    // in engine or somewhere similar.
    static bool init = false;
    if (!init)
    {
        SymInitialize(::gCurrentProcess, nullptr, TRUE);
        init = true;
    }

    BYTE symbolBuffer[sizeof(SYMBOL_INFO) + 255];
    SYMBOL_INFO* symbol  = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    bool found = false;
    for (USHORT i{ 0 }; i < stackCount; ++i)
    {
        SymFromAddr(::gCurrentProcess, reinterpret_cast<DWORD64>(stack[i]), nullptr, symbol);
        if (std::strstr(symbol->Name, "ignite"))
        {
            found = true;
            break;
        }
    }
    char name[256] = { 0 };
    strncpy(name, symbol->Name, symbol->NameLen);
    name[symbol->NameLen] = '\0';
    return ignite::mem::MemoryManager::Instance()->New(static_cast<uint32_t>(size), symbol->NameLen > 0 && found ? name : "Unknown");
#else // Else DEV_CONFIGURATION.
    return ignite::mem::MemoryManager::Instance()->New(static_cast<uint32_t>(size));
#endif // !DEV_CONFIGURATION.
}

void operator delete(void* address) noexcept
{
    ignite::mem::MemoryManager::Instance()->Delete(address);
}

namespace ignite
{

Engine* Engine::mInstance = nullptr;

mem::WeakHandle<Engine> Engine::CreateEngine()
{
    if (mInstance)
    {
        DEBUG_ERROR("Failed to create engine as engine has already been created!");
        DEBUG_BREAK();

        return mem::WeakHandle{ mInstance };
    }

    mem::MemoryManager::Init(128 * 1'024);

    DEBUG(utils::InstrumentationSession::Create());
    DEBUG(utils::InstrumentationSession::Instance()->StartSession());

    // In debug mode, if we have the live debug window for memory manager, we create an SDL window on another thread.
    // We then get a race condition as the engine creates another window, meaning we can fail to create one of the windows
    // if one is being created at the same time. Add a sleep here to ensure the profiling window has time to finish initializing.
    DEBUG(Sleep(10));

    mInstance = new Engine();
    DEBUG_INFO("Successfully created Ignite Engine.");

    return mem::WeakHandle{ mInstance };
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

    mInputManager = new InputManager();

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

    mRenderer       = new Renderer(mWindow);
    mTextureManager = new TextureManager(mRenderer->GetRendererBackend());
    mRenderer->SetTextureManagerRef(mem::WeakHandle{ mTextureManager });

    mFontRenderer     = new FontRenderer(mRenderer->GetRendererBackend());
    mCollisionHandler = new CollisionHandler();
    mParticleManager  = new ParticleManager();

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
        // Max frames displayed as: 1234.123 <- total of 8 characters + 1 for null terminating character.
        char title[9];
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

        delete mParticleManager;
        delete mCollisionHandler;

        delete mFontRenderer;

        delete mTextureManager;
        delete mRenderer;

        SDL_DestroyWindow(mWindow);

        delete mInputManager;

    }

    DEBUG_INFO("Successfully destroyed Ignite Engine.");

    DEBUG(utils::InstrumentationSession::Instance()->EndSession());
    DEBUG(utils::InstrumentationSession::Destroy());

    delete mInstance;
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
        const mem::WeakHandle<Renderer> renderer = mem::WeakHandle{ mRenderer };
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
