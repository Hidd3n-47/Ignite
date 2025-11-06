#pragma once

#include "Texture.h"
#include "IgniteEngine/Core/OrthoCamera.h"
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

    [[nodiscard]] Texture Load(const std::filesystem::path& filePath);

    void RemoveTexture(const uint16_t id);
    void RemoveAllTextures();

    inline static constexpr uint16_t INVALID_ID{ 0 };
private:
    SDL_Renderer* mRendererBackend;

    uint16_t mId = 0;
    std::unordered_map<uint16_t, SDL_Texture*> mTextureMap;
};

} // Namespace ignite.