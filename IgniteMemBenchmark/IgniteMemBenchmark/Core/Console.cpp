#include "Console.h"

#include <print>
#include <ranges>
#include <string>
#include <cinttypes>
#include <functional>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_sdlrenderer3.h>

#include <imgui/implot.h>

#include "TestResultsManager.h"

namespace ignite::bench
{

struct BinaryTreePosition 
{
    int index;
    int depth;
};

Console* Console::mInstance = nullptr;

Console::Console()
{
    mRunning = false;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        std::println("Error: SDL_Init(): {}", SDL_GetError());
        return;
    }

    mWindow = SDL_CreateWindow("Benchmark Console", 1080, 720, SDL_WINDOW_RESIZABLE);
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
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(mWindow, mRenderer);
    ImGui_ImplSDLRenderer3_Init(mRenderer);

    mRunning = true;
}

Console::~Console()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Console::Init()
{
    mInstance = new Console();
}

void Console::Destroy()
{
    delete mInstance;
}

void Console::Run()
{
    while (mRunning)
    {
        Update();
        Render();
    }
    Destroy();
}

void Console::Update()
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

void Console::Render() const
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    bool show_demo_window = true;

    const ImGuiIO& io = ImGui::GetIO();

    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
        ImPlot::ShowDemoWindow(&show_demo_window);
    }

    {
        for (const TestResult& result : TestResultsManager::Instance()->GetTestResults())
        {
            ImGui::Begin(result.testName.c_str());

            std::string allocationTitle   = result.testName + "_allocation";
            std::string deallocationTitle = result.testName + "_deallocation";

            if (ImPlot::BeginPlot(allocationTitle.c_str()))
            {
                ImPlot::PlotLine("Memory Manager", result.memoryAllocTimes.data(), result.memoryAllocTimes.size());

                ImPlot::PlotLine("C++ New/Delete", result.memoryAllocTimesCpp.data(), result.memoryAllocTimesCpp.size());

                double x[2] = { 0, static_cast<double>(result.memoryAllocTimes.size()) };
                double y[2] = { result.averageAlloc, result.averageAlloc };
                ImPlot::PlotLine("Memory Manager Avg", x, y, 2);

                x[1] = static_cast<double>(result.memoryAllocTimesCpp.size());
                y[0] = result.averageAllocCpp;
                y[1] = result.averageAllocCpp;
                ImPlot::PlotLine("C++ New/Delete Avg", x, y, 2);

                ImPlot::EndPlot();
            }

            if (ImPlot::BeginPlot(deallocationTitle.c_str()))
            {
                ImPlot::PlotLine("Memory Manager", result.memoryDeallocTimes.data(), result.memoryDeallocTimes.size());

                ImPlot::PlotLine("C++ New/Delete", result.memoryDeallocTimesCpp.data(), result.memoryDeallocTimesCpp.size());
                
                double x[2] = { 0, static_cast<double>(result.memoryDeallocTimes.size() - 1) };
                double y[2] = { result.averageDealloc, result.averageDealloc };
                ImPlot::PlotLine("Memory Manager Avg", x, y, 2);

                x[1] = static_cast<double>(result.memoryDeallocTimesCpp.size() - 1);
                y[0] = result.averageDeallocCpp;
                y[1] = result.averageDeallocCpp;
                ImPlot::PlotLine("C++ New/Delete Avg", x, y, 2);

                ImPlot::EndPlot();
            }

            ImGui::End();
        }
    }

    constexpr ImVec4 clearColor{ 0.45f, 0.55f, 0.6f, 1.0f };

    ImGui::Render();
    SDL_SetRenderScale(mRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(mRenderer, clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    SDL_RenderClear(mRenderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), mRenderer);
    SDL_RenderPresent(mRenderer);
}

} // Namespace ignite::bench.
