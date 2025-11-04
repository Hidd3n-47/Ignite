#pragma once

struct SDL_Texture;
struct SDL_Renderer;

namespace ignite
{

class TextureManager
{
public:
    TextureManager(SDL_Renderer* rendererBackend);
    ~TextureManager();

    [[nodiscard]] uint16_t Load(const std::string& filePath, int width, int height);

    void RenderSingle(const uint16_t id, Vec2 world, Vec2 scale, const Vec2& dimensions, int sheetX, float angle, bool flip);

    void RemoveTexture(const uint16_t id);
    void RemoveAllTextures();
private:
    SDL_Renderer* mRendererBackend;

    uint16_t mId = 0;
    std::unordered_map<uint16_t, SDL_Texture*> mTextureMap;
};

} // Namespace ignite.