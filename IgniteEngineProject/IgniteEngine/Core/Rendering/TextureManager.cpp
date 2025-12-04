#include "IgnitePch.h"
#include "TextureManager.h"

#include <ranges>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

#include "Defines.h"

namespace ignite
{
TextureManager::TextureManager(SDL_Renderer* rendererBackend)
    : mRendererBackend(rendererBackend)
{
    PROFILE_FUNC();

    // Create the invalid texture. This texture will be created manually from a 1x1 pixel that is pink.
    SDL_Surface* surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA32);

    const uint32_t pixelColor = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), nullptr, 255, 0, 255, 255);

    uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);
    *pixels = pixelColor;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRendererBackend, surface);
    mTextureMap[mId++] = texture;

    SDL_DestroySurface(surface);

}

TextureManager::~TextureManager()
{
    PROFILE_FUNC();

    RemoveAllTextures();
}

void TextureManager::Load(Texture& texture, const char* filePath, const uint32_t spritesheetMaxX, const uint32_t spritesheetMaxY)
{
    PROFILE_FUNC();

    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface)
    {
        DEBUG_ERROR("Failed to load image at path: {}. Error: {}", filePath, SDL_GetError());
        return;
    }

    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(mRendererBackend, surface);
    SDL_DestroySurface(surface);

    if (!sdlTexture)
    {
        DEBUG_ERROR("Failed to create texture from surface. Error: {}", SDL_GetError());
        return;
    }

    SDL_SetTextureScaleMode(sdlTexture, SDL_SCALEMODE_NEAREST);

    mTextureMap[mId] = sdlTexture;

    texture.id = mId++;
    texture.spritesheetMaxX = static_cast<float>(spritesheetMaxX);
    texture.spritesheetMaxY = static_cast<float>(spritesheetMaxY);
    texture.width           = static_cast<float>(sdlTexture->w) / texture.spritesheetMaxX;
    texture.height          = static_cast<float>(sdlTexture->h) / texture.spritesheetMaxY;

}

void TextureManager::RemoveTexture(const uint16_t id)
{
    PROFILE_FUNC();
    SDL_DestroyTexture(mTextureMap[id]);

    mTextureMap.erase(id);
}

void TextureManager::RemoveAllTextures()
{
    PROFILE_FUNC();
    for (SDL_Texture* texture : mTextureMap | std::views::values)
    {
        SDL_DestroyTexture(texture);
    }

    mTextureMap.clear();
}

} // Namespace ignite.