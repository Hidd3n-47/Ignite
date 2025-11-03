#include "IgnitePch.h"
#include "Engine.h"

#include <SDL3/SDL.h>

#include "EC/Scene.h"
#include "InputManager.h"

namespace ignite
{

Engine* Engine::mInstance = nullptr;

Engine* Engine::CreateEngine()
{
    if (mInstance)
    {
        DEBUG_ERROR("Failed to create engine as engine has already been created!");
        DEBUG_BREAK();

        return mInstance;
    }

    mInstance = new Engine();
    DEBUG_INFO("Successfully created Ignite Engine.");

    return mInstance;
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

    mWindow   = SDL_CreateWindow("Ignite", 1080, 720, 0);
    mRenderer = SDL_CreateRenderer(mWindow, nullptr);

    if (!mWindow || !mRenderer)
    {
        DEBUG_ERROR("Failed to create SDL3 window or renderer.");
        return;
    }

    mRunning = true;
}

void Engine::Run() const
{
    while (mRunning)
    {
        mInputManager->Poll();

        Update();
        Render();
    }
}

void Engine::Destroy() const
{
    delete mInputManager;

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);

    delete mInstance;

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
    }
}

void Engine::Render() const
{
}

} // Namespace ignite.
