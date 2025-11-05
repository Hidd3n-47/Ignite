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
    RemoveAllTextures();
}

Texture TextureManager::Load(const std::filesystem::path& filePath)
{
    Texture t
    {
        .id     = INVALID_ID,
        .width  = 1.0f,
        .height = 1.0f
    };

    SDL_Surface* surface = IMG_Load(filePath.string().c_str());
    if (!surface)
    {
        DEBUG_ERROR("Failed to load image at path: {}. Error: {}", filePath.string(), SDL_GetError());
        return t;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRendererBackend, surface);
    SDL_DestroySurface(surface);

    if (!texture)
    {
        DEBUG_ERROR("Failed to create texture from surface. Error: {}", SDL_GetError());
        return t;
    }

    mTextureMap[mId] = texture;

    t.id     = mId++;
    t.width  = static_cast<float>(texture->w);
    t.height = static_cast<float>(texture->h);

    return t;
}


// todo change this to be render commands
    // render commands will be a class that just contains information about the render, such as spritesheet info,
    // location, rotation, scale, LAYER. this can then be added to a queue and processed from there.

void TextureManager::RenderSingle(const Texture texture, mem::WeakRef<Transform> transform, const OrthoCamera& camera)
{
    const Vec2 screenPosition = camera.PositionToScreenSpace(transform->translation);

    const SDL_FRect srcRect { 0.0f, 0.0f, texture.width, texture.height };
    const SDL_FRect destRect{ screenPosition.x - texture.width * 0.5f, screenPosition.y - texture.height * 0.5f, texture.width * transform->scale.x, texture.height * transform->scale.y };

    const bool err = SDL_RenderTextureRotated(mRendererBackend, mTextureMap[texture.id], &srcRect, &destRect, transform->rotation, nullptr, SDL_FLIP_NONE);

    DEBUG(if (!err))
        DEBUG_ERROR("Failed to render texture with ID: {}. Error: {}", texture.id, SDL_GetError());
}

void TextureManager::RenderSingleFromSpriteSheet(const Texture texture, mem::WeakRef<Transform> transform,
    const OrthoCamera& camera, const float x, const float y, const float xMax, const float yMax)
{
    const Vec2 screenPosition = camera.PositionToScreenSpace(transform->translation);

    const float textureWidth  = texture.width  / xMax;
    const float textureHeight = texture.height / yMax;
    const SDL_FRect srcRect{ textureWidth * x, textureHeight * y, textureWidth, textureHeight };
    const SDL_FRect destRect{ screenPosition.x - textureWidth * 0.5f, screenPosition.y - textureHeight * 0.5f, textureWidth * transform->scale.x, textureHeight * transform->scale.y };

    const bool err = SDL_RenderTextureRotated(mRendererBackend, mTextureMap[texture.id], &srcRect, &destRect, transform->rotation, nullptr, SDL_FLIP_NONE);

    DEBUG(if (!err))
        DEBUG_ERROR("Failed to render texture with ID: {}. Error: {}", texture.id, SDL_GetError());
}

void TextureManager::RemoveTexture(const uint16_t id)
{
    SDL_DestroyTexture(mTextureMap[id]);

    mTextureMap.erase(id);
}

void TextureManager::RemoveAllTextures()
{
    for (SDL_Texture* texture : mTextureMap | std::views::values)
    {
        SDL_DestroyTexture(texture);
    }

    mTextureMap.clear();
}

} // Namespace ignite.