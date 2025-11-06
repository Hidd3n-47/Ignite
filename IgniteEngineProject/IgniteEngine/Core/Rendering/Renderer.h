#pragma once

#include "IgniteEngine/Core/OrthoCamera.h"

struct SDL_Window;
struct SDL_Renderer;

namespace ignite
{

class TextureManager;
struct RenderCommand;

class Renderer
{
public:
    Renderer(SDL_Window* window);
    ~Renderer();

    void StartRender() const;
    void Render(const OrthoCamera& camera);
    void EndRender() const;

    inline void AddRenderCommand(const uint32_t layer, mem::WeakRef<RenderCommand> command) { mCommands[layer].emplace_back(command); }

    inline void SetTextureManagerRef(mem::WeakRef<TextureManager> textureManagerRef) { mTextureManagerRef = textureManagerRef; }

    [[nodiscard]] inline SDL_Renderer* GetRendererBackend() const { return mRenderer; }
private:
    SDL_Renderer* mRenderer;
    mem::WeakRef<TextureManager> mTextureManagerRef;

    std::map<uint32_t, std::vector<mem::WeakRef<RenderCommand>>> mCommands;
};

} // Namespace ignite.