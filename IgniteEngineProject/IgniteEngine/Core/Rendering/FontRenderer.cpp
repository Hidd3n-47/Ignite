#include "IgnitePch.h"
#include "FontRenderer.h"

#include <ranges>
#include <SDL3_ttf/SDL_ttf.h>

#include "Defines.h"
#include "Font.h"
#include "EC/Components/Transform.h"

namespace ignite
{

FontRenderer::FontRenderer(SDL_Renderer* rendererBackend)
    : mRendererBackend(rendererBackend)
{
    TTF_Init();
}

FontRenderer::~FontRenderer()
{
    DeleteAllFonts();

    TTF_Quit();
}

uint16_t FontRenderer::CreateFont(const char* fontFilepath, const float fontSize, const std::string& text, const mem::WeakHandle<Transform> transform)
{
    LoadFont(mFontId, fontFilepath, fontSize, text, transform);

    return mFontId++;
}

void FontRenderer::UpdateFont(const uint16_t id, const mem::WeakHandle<Transform> transform)
{
    mFonts[id]->transform = transform;
}

void FontRenderer::UpdateFont(const uint16_t id, const std::string& text)
{
    if (mFonts[id]->text != text)
    {
        mFonts[id]->text = text;
        LoadFont(id, mFonts[id]->filePath.c_str(), mFonts[id]->fontSize, mFonts[id]->text, mFonts[id]->transform);
    }
}

void FontRenderer::RenderFonts(const OrthoCamera& camera)
{
    PROFILE_FUNC();

    for (const Font* font : mFonts | std::views::values)
    {
        const float width  = static_cast<float>(font->width);
        const float height = static_cast<float>(font->height);

        const Vec2 screenPos = camera.PositionToScreenSpace(font->transform->translation);

        const SDL_FRect srcRect
        {
            .x = 0.0f,
            .y = 0.0f,
            .w = width,
            .h = height
        };
        const SDL_FRect destRect
        {
            .x = screenPos.x - width  * 0.5f,
            .y = screenPos.y - height * 0.5f,
            .w = width,
            .h = height
        };

        const bool err = SDL_RenderTexture(mRendererBackend, font->texture, &srcRect, &destRect);

        DEBUG(if (!err))
            DEBUG_ERROR("Failed to render font texture.");
    }
}

void FontRenderer::RemoveFont(const uint16_t id)
{
    delete mFonts[id];

    mFonts.erase(id);
}

void FontRenderer::DeleteAllFonts()
{
    for (const Font* font : mFonts | std::views::values)
    {
        delete font;
    }

    mFonts.clear();
}

void FontRenderer::LoadFont(const uint16_t fontId, const char* fontFilepath, const float fontSize, const std::string& text, const mem::WeakHandle<Transform> transform)
{
    mFont = TTF_OpenFont(fontFilepath, fontSize);
    DEBUG(if (!mFont))
        DEBUG_ERROR("Failed to load font at the passed in filepath: {}", fontFilepath);

    SDL_Surface* surface = TTF_RenderText_Solid(mFont, text.c_str(), text.length(), { 255, 255, 255 });
    DEBUG(if (!surface))
        DEBUG_ERROR("Failed to create surface for font.");

    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRendererBackend, surface);
    DEBUG(if (!texture))
        DEBUG_ERROR("Failed to create texture for the font.");

    const uint32_t width = surface->w;
    const uint32_t height = surface->h;

    SDL_DestroySurface(surface);

    TTF_CloseFont(mFont);

    if (!mFonts.contains(fontId))
    {
        mFonts[fontId] = new Font(texture, width, height, fontSize, transform, text, std::string{ fontFilepath });

        return;
    }

    SDL_DestroyTexture(mFonts[fontId]->texture);

    mFonts[fontId]->texture = texture;
    mFonts[fontId]->width   = width;
    mFonts[fontId]->height  = height;
}

} // Namespace ignite.