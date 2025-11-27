#include "IgnitePch.h"
#include "Font.h"

#include <SDL3/SDL_render.h>

namespace ignite
{

Font::Font(SDL_Texture* texture, const uint32_t w, const uint32_t h, const float size, const mem::WeakRef<Transform> transform, std::string text, std::string filePath)
    : texture(texture)
    , width(w)
    , height(h)
    , fontSize(size)
    , transform(transform)
    , text(std::move(text))
    , filePath(std::move(filePath))
{
    // Empty.
}
Font::~Font()
{
    SDL_DestroyTexture(texture);
}

} // Namespace ignite.