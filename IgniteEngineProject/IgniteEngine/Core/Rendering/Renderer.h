#pragma once

struct SDL_Window;
struct SDL_Renderer;

namespace ignite
{

class Renderer
{
public:
    Renderer(SDL_Window* window);
    ~Renderer();

    void StartRender() const;
    void Render();
    void EndRender() const;

private:
    SDL_Renderer* mRenderer;
};

} // Namespace ignite.