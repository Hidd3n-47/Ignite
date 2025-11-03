#include "IgnitePch.h"
#include "Engine.h"

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
}

void Engine::Run()
{
}

void Engine::Destroy()
{
    delete mInstance;

    DEBUG_INFO("Successfully destroyed Ignite Engine.");
}

void Engine::Update() const
{
}

void Engine::PostUpdate()
{
}


void Engine::Render() const
{
}

} // Namespace ignite.
