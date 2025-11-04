#include "IgnitePch.h"
#include "InputManager.h"

#include <SDL3/SDL_events.h>

#include "Core/Engine.h"

namespace ignite
{

void InputManager::Poll()
{
    StartFrame();

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_EVENT_QUIT:
            Engine::Instance()->CloseGame();
            break;
        case SDL_EVENT_KEY_DOWN:
            KeyDown(e.key.key);
            break;
        case SDL_EVENT_KEY_UP:
            KeyUp(e.key.key);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            MouseDown(e.button.button);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            MouseUp(e.button.button);
            break;
        default:
            break;
        }
    }
}

void InputManager::KeyDown(const keycode key)
{
    mKeyMap[key] = true;

    mKeyMapForSingleFrame.emplace(key);
}

void InputManager::KeyUp(const keycode key)
{
    mKeyMap[key] = false;
}

void InputManager::MouseDown(const mouse button)
{
    mMouseButtonMap[button] = true;

    mMouseButtonMapForSingleFrame.emplace(button);
}

void InputManager::MouseUp(const mouse button)
{
    mMouseButtonMap[button] = false;
}

} // Namespace ignite.