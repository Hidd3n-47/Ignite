#include "IgnitePch.h"
#include "Renderer.h"

#include <SDL3/SDL_render.h>

#include "Defines.h"

namespace ignite
{

Renderer::Renderer(SDL_Window* window)
{
    mRenderer = SDL_CreateRenderer(window, nullptr);

    DEBUG(if (!mRenderer))
        DEBUG_ERROR("Failed to create SDL Renderer.");
}

Renderer::~Renderer()
{
    SDL_DestroyRenderer(mRenderer);
}

void Renderer::StartRender() const
{
    SDL_SetRenderDrawColor(mRenderer, 120, 120, 120, 255);
    SDL_RenderClear(mRenderer);
}

void Renderer::Render()
{
}

void Renderer::EndRender() const
{
    SDL_RenderPresent(mRenderer);
}

} // Namespace ignite.

