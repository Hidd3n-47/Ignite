#include "IgnitePch.h"
#include "Renderer.h"

#include <ranges>

#include <SDL3/SDL_render.h>

#include "Defines.h"
#include "RenderCommand.h"
#include "TextureManager.h"

#include "EC/Components/Transform.h"
#include "Math/Mat2.h"
#include "Math/Math.h"

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
    for (const std::vector<mem::WeakRef<RenderCommand>>& layerCommands : mCommands | std::views::values)
    {
        for (const mem::WeakRef<RenderCommand> command : layerCommands)
        {
            const float rotation = command->transform->rotation;
            const float cosRot = std::cos(Math::DegToRads(rotation));
            const float sinRot = std::sin(Math::DegToRads(rotation));
            const Mat2 rotMat
            {
                .m11 = cosRot,
                .m12 = -sinRot,
                .m21 = sinRot,
                .m22 = cosRot
            };
            const Vec2 rotatedOffset = rotMat * command->offset;
#ifdef DEV_CONFIGURATION
            if (command->debugSquare)
            {
                const Vec2 debugScreenPos = camera.PositionToScreenSpace(command->transform->translation + rotatedOffset);
                const Vec2 debugBoxHalfExtentsScreen = camera.SizeInScreenSpace(command->debugSquareHalfExtents);

                const SDL_FRect destRect =
                {
                    .x = debugScreenPos.x - debugBoxHalfExtentsScreen.x,
                    .y = debugScreenPos.y - debugBoxHalfExtentsScreen.y,
                    .w = 2.0f * debugBoxHalfExtentsScreen.x,
                    .h = 2.0f * debugBoxHalfExtentsScreen.y
                };

                SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
                SDL_RenderRect(mRenderer, &destRect);

                continue;
            }
#endif // DEV_CONFIGURATION.

            const Vec2 screenPosition = camera.PositionToScreenSpace(command->transform->translation + rotatedOffset);

            const float srcTextureWidth  = command->texture.width;
            const float srcTextureHeight = command->texture.height;
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
                .x = screenPosition.x - textureWidth  * 0.5f,
                .y = screenPosition.y - textureHeight * 0.5f,
                .w = textureWidth,
                .h = textureHeight
            };

            SDL_SetTextureAlphaMod(mTextureManagerRef->mTextureMap[command->texture.id], command->alpha);

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

