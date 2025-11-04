#include "IgnitePch.h"
#include "Engine.h"

#include <SDL3/SDL.h>

#include "EC/Scene.h"
#include "Core/Input/InputManager.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Rendering/TextureManager.h"

namespace ignite
{

Engine* Engine::mInstance = nullptr;

static uint16_t i;

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

    if (!mWindow)
    {
        DEBUG_ERROR("Failed to create SDL3 window");
        return;
    }

    mRenderer = new Renderer(mWindow);

    mTextureManager = new TextureManager(mRenderer->GetRendererBackend());
    i = mTextureManager->Load("E:/Programming/Ignite/Assets/car_24px_8way_blue_1.png", 24, 24);

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
    delete mTextureManager;

    delete mRenderer;
    SDL_DestroyWindow(mWindow);

    delete mInputManager;

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
    mRenderer->StartRender();

    if (mActiveScene.IsRefValid())
    {
        mActiveScene->Render();
    }
    mTextureManager->RenderSingle(i, Vec2{ 0.0f, 0.0f }, Vec2{ 1.0f, 1.0f }, Vec2{ 24.0f, 24.0f }, 0, 0.0f, false);

    mRenderer->EndRender();
}

} // Namespace ignite.
