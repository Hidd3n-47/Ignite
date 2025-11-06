#pragma once

struct SDL_Texture;

namespace ignite
{

class Transform;

struct Font
{
    Font() = default;
    Font(SDL_Texture* texture, const uint32_t w, const uint32_t h, const float size, const mem::WeakRef<Transform> transform, std::string text, std::string filePath);
    ~Font();

    SDL_Texture* texture = nullptr;

    uint32_t width    = 0;
    uint32_t height   = 0;
    float    fontSize = 12;

    mem::WeakRef<Transform> transform;

    std::string text;
    std::string filePath;
};

} // Namespace ignite.