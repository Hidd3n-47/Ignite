#include "IgnitePch.h"
#include "TextureManager.h"

#include <ranges>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

namespace ignite
{
TextureManager::TextureManager(SDL_Renderer* rendererBackend)
    : mRendererBackend(rendererBackend)
{
    // Empty.
}

TextureManager::~TextureManager()
{
    RemoveAllTextures();
}

//todo make 0 the invalid texture.
uint16_t TextureManager::Load(const std::string& filePath, int width, int height)
{
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface)
    {
        DEBUG_ERROR("Failed to load image at path: {}. Error: ", filePath, SDL_GetError());
        return 0;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRendererBackend, surface);
    SDL_DestroySurface(surface);

    if (!texture)
    {
        DEBUG_ERROR("Failed to create texture from surface. Error: ", SDL_GetError());
        return 0;
    }

    mTextureMap[mId] = texture;

    return mId++;
}

void TextureManager::RenderSingle(const uint16_t id, Vec2 world, Vec2 scale, const Vec2& dimensions, int sheetX, float angle, bool flip)
{
    SDL_FRect srcRect;
    SDL_FRect destRect;

    srcRect = { 0.0f, 0.0f, dimensions.x, dimensions.y };
    SDL_FRect r = { floor(world.x), floor(world.y), dimensions.x * scale.x, dimensions.y * scale.y };
    destRect = r;

    bool err = SDL_RenderTextureRotated(mRendererBackend, mTextureMap[id], &srcRect, &destRect, angle, nullptr, SDL_FLIP_NONE);

    DEBUG(if (!err))
        DEBUG_ERROR("Failed to render texture with ID: {}. Error: {}", id, SDL_GetError());
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