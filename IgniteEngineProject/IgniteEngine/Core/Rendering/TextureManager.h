#pragma once

#include "Texture.h"
#include "IgniteEngine/EC/Components/Transform.h"

struct SDL_Texture;
struct SDL_Renderer;

namespace ignite
{

class TextureManager
{
    friend class Renderer;
public:
    TextureManager(SDL_Renderer* rendererBackend);
    ~TextureManager();

    TextureManager(const TextureManager&)             = delete;
    TextureManager(TextureManager&&)                  = delete;
    TextureManager& operator=(TextureManager&&)       = delete;
    TextureManager& operator=(const TextureManager&)  = delete;

    void Load(Texture& texture, const char* filePath, const uint32_t spritesheetMaxX = 1, const uint32_t spritesheetMaxY = 1);

    void RemoveTexture(const uint16_t id);
    void RemoveAllTextures();
private:
    SDL_Renderer* mRendererBackend;

    uint16_t mId = 0;
    std::unordered_map<uint16_t, SDL_Texture*> mTextureMap;
    std::unordered_map <std::string, Texture> mFilePathToTexture;
};

} // Namespace ignite.