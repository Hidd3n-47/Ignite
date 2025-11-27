#pragma once

struct TTF_Font;
struct SDL_Renderer;

#include <IgniteEngine/Core/OrthoCamera.h>

namespace ignite
{

struct Font;
class Transform;

class FontRenderer
{
public:
    FontRenderer(SDL_Renderer* rendererBackend);
    ~FontRenderer();

    [[nodiscard]] uint16_t CreateFont(const char* fontFilepath, const float fontSize, const std::string& text, const mem::WeakHandle<Transform> transform);

    void UpdateFont(const uint16_t id, const mem::WeakHandle<Transform> transform);
    void UpdateFont(const uint16_t id, const std::string& text);

    void RenderFonts(const OrthoCamera& camera);

    void RemoveFont(const uint16_t id);
    void DeleteAllFonts();
private:
    SDL_Renderer* mRendererBackend;

    uint16_t  mFontId = 0;
    TTF_Font* mFont   = nullptr;

    std::unordered_map<uint16_t, Font*> mFonts;
    void LoadFont(const uint16_t fontId, const char* fontFilepath, const float fontSize, const std::string& text, const mem::WeakHandle<Transform> transform);
};

} // Namespace ignite.