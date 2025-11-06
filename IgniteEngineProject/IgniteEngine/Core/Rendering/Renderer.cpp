#include "IgnitePch.h"
#include "Renderer.h"

#include <ranges>

#include <SDL3/SDL_render.h>

#include "Defines.h"
#include "RenderCommand.h"
#include "TextureManager.h"

#include "EC/Components/Transform.h"

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

void Renderer::Render(const OrthoCamera& camera)
{
    // loop through and render the commands.
    for (const std::vector<mem::WeakRef<RenderCommand>>& layerCommands : mCommands | std::views::values)
    {
        for (const mem::WeakRef<RenderCommand> command : layerCommands)
        {
            const Vec2 screenPosition = camera.PositionToScreenSpace(command->transform->translation);

            // todo look at possibly storing this info into texture to prevent per frame calculations below.
            const float srcTextureWidth  = command->texture.width  / command->spritesheetMaxX;
            const float srcTextureHeight = command->texture.height / command->spritesheetMaxY;
            const float textureWidth     = srcTextureWidth  * command->transform->scale.x;
            const float textureHeight    = srcTextureHeight * command->transform->scale.y;

            const SDL_FRect srcRect
            {
                .x = srcTextureWidth  * command->spritesheetPosX,
                .y = srcTextureHeight * command->spritesheetPosY,
                .w = srcTextureWidth,
                .h = srcTextureHeight
            };
            const SDL_FRect destRect
            {
                .x = screenPosition.x - textureWidth  * 0.5f * command->transform->scale.x,
                .y = screenPosition.y - textureHeight * 0.5f * command->transform->scale.y,
                .w = textureWidth  * command->transform->scale.x,
                .h = textureHeight * command->transform->scale.y
            };

            const bool err = SDL_RenderTextureRotated(mRenderer, mTextureManagerRef->mTextureMap[command->texture.id], &srcRect, &destRect, command->transform->rotation, nullptr, SDL_FLIP_NONE);

            DEBUG(if (!err))
                DEBUG_ERROR("Failed to render texture with ID: {}. Error: {}", command->texture.id, SDL_GetError());
        }
    }


    mCommands.clear();
}

void Renderer::EndRender() const
{
    SDL_RenderPresent(mRenderer);
}

} // Namespace ignite.

