#include "IgnitePch.h"
#include "Engine.h"

#include <SDL3/SDL.h>

#include "Defines.h"
#include "EC/Scene.h"
#include "Core/Input/InputManager.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Rendering/TextureManager.h"

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

    mInstance = new Engine();
    DEBUG_INFO("Successfully created Ignite Engine.");

    return mem::WeakRef{ mInstance };
}

void Engine::Init()
{
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

    mWindow   = SDL_CreateWindow("Ignite", static_cast<int>(DEFAULT_SCREEN_SIZE.x), static_cast<int>(DEFAULT_SCREEN_SIZE.y), 0);

    if (!mWindow)
    {
        DEBUG_ERROR("Failed to create SDL3 window");
        return;
    }

    mRenderer = new Renderer(mWindow);

    mTextureManager = new TextureManager(mRenderer->GetRendererBackend());

    mRunning = true;
}

void Engine::Run()
{
    while (mRunning)
    {
        mInputManager->Poll();

        Update();
        PostUpdate();
        Render();
    }
}

void Engine::Destroy() const
{
    delete mTextureManager;

    delete mRenderer;
    SDL_DestroyWindow(mWindow);

    delete mInputManager;

    delete mInstance;
    mInstance = nullptr;

    DEBUG_INFO("Successfully destroyed Ignite Engine.");
}

void Engine::Update() const
{
    if (mActiveScene.IsRefValid())
    {
        mActiveScene->Update(0.0f);
    }
}

void Engine::PostUpdate()
{
    if (mSceneToChangeTo.IsRefValid())
    {
        mActiveScene = mSceneToChangeTo;
        mActiveScene->InitScene();

        mSceneToChangeTo.Invalidate();
    }
}

void Engine::Render() const
{
    mRenderer->StartRender();

    if (mActiveScene.IsRefValid())
    {
        mActiveScene->Render(mCamera);
    }

    mRenderer->EndRender();
}

} // Namespace ignite.
