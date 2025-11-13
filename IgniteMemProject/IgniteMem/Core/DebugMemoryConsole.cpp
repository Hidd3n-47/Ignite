#include "DebugMemoryConsole.h"

#include <print>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

#include "MemoryManager.h"

namespace ignite::mem
{

DebugMemoryConsole* DebugMemoryConsole::mInstance = nullptr;

DebugMemoryConsole::DebugMemoryConsole()
{
    mRunning = false;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::println("Error: SDL_Init(): {}", SDL_GetError());
        return;
    }

    mWindow = SDL_CreateWindow("Debug Memory Console", 1080, 720, 0);
    if (!mWindow)
    {
        std::println("Error: SDL_CreateWindow(): {}", SDL_GetError());
        return;
    }

    mRenderer = SDL_CreateRenderer(mWindow, nullptr);
    SDL_SetRenderVSync(mRenderer, 1);
    if (!mRenderer)
    {
        std::println("Error: SDL_CreateRenderer(): {}", SDL_GetError());
        return;
    }

    SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(mWindow);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(mWindow, mRenderer);
    ImGui_ImplSDLRenderer3_Init(mRenderer);

    mRunning = true;
}

DebugMemoryConsole::~DebugMemoryConsole()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void DebugMemoryConsole::Init()
{
    mInstance = new DebugMemoryConsole();
    mInstance->Run();
}

void DebugMemoryConsole::Destroy()
{
    delete mInstance;
}

void DebugMemoryConsole::Run()
{
    while (mRunning)
    {
        Update();
        Render();
    }
    Destroy();
}

void DebugMemoryConsole::Update()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
        {
            mRunning = false;
        }
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(mWindow))
        {
            mRunning = false;
        }
    }
}

void DebugMemoryConsole::Render() const
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    bool show_demo_window = true;
    bool show_another_window;
    const ImGuiIO& io = ImGui::GetIO();
    constexpr ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");

        ImGui::Text("This is some useful text.");
        ImGui::Checkbox("Demo Window", &show_demo_window);
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);

        if (ImGui::Button("Button"))
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    {
        ImGui::Begin("Allocation Stats");

        ImGui::Text("Memory Blocks:");
        const uint64_t size = MemoryManager::mInstance->GetSize();

        std::vector<float> values;
        values.resize(size / 4, 1.0f);

        UpdateMemoryBlockVector(values, MemoryManager::mInstance->GetRootNode(), 4);

        ImGui::PlotHistogram("##Memory block", values.data(), static_cast<int>(values.size()), 0, nullptr, 0.0f, 1.0f);

        ImGui::Text("Memory allocated:");
        ImGui::ProgressBar(1.0f - static_cast<float>(MemoryManager::mInstance->GetSizeFree()) / static_cast<float>(MemoryManager::mInstance->GetSize()), { 200.0f, 0.0f });
        ImGui::End();
    }

    ImGui::Render();
    SDL_SetRenderScale(mRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(mRenderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    SDL_RenderClear(mRenderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenderer);
    SDL_RenderPresent(mRenderer);
}

void DebugMemoryConsole::UpdateMemoryBlockVector(std::vector<float>& vector, const Node* node, const uint64_t barSize)
{
    const uint64_t memoryStart = (uint64_t)MemoryManager::mInstance->GetStartOfMemoryBlock();
    const uint64_t start = ((uint64_t)(node->start) - (uint64_t)(memoryStart)) / barSize;
    const uint64_t end = start + node->size / barSize;

    std::fill(vector.begin() + start, vector.begin() + end, 0.1f);

    //todo add traversing through the children.
}

} // Namespace ignite::mem.