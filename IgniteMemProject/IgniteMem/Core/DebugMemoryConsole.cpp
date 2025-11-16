#include "DebugMemoryConsole.h"

#ifdef DEV_CONFIGURATION

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

#include "MemoryManager.h"

namespace ignite::mem
{

struct BinaryTreePosition 
{
    int index;
    int depth;
};

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

    const ImGuiIO& io = ImGui::GetIO();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    {

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Begin("Free memory block binary tree");

        Node* root = MemoryManager::mInstance->GetRootNode();

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImVec2 canvasPos  = ImGui::GetCursorScreenPos();
        const ImVec2 canvasSize = ImGui::GetContentRegionAvail();

        /*
         * ============================================================================================
         * The following section is to draw the binary tree representation of the free blocks of memory
         * This makes use of lambda functions that are recursive. It has to be lambda functions as else
         * the functions would require to pass too many variables around, so it's easier to make the 
         * functions lambda with a reference instead of having to pass in each required variable.
         * ============================================================================================
         */

        // Background canvas.
        drawList->AddRectFilled(canvasPos, { canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y }, IM_COL32(32, 32, 32, 255));


        constexpr ImVec2 quadHalfSize = { 50.0f, 25.0f };

        std::unordered_map<Node*, BinaryTreePosition> pos;

        int order = 0;
        AssignPositionsForBinaryTree(root, 0, order, pos);

        // Find bounds to center horizontally.
        int minIndex = INT_MAX;
        int maxIndex = INT_MIN;
        int maxDepth = 0;

        for (const BinaryTreePosition& p : pos | std::views::values) 
        {
            minIndex = std::min(minIndex, p.index);
            maxIndex = std::max(maxIndex, p.index);
            maxDepth = std::max(maxDepth, p.depth);
        }

        constexpr float xStep = 80.0f;
        constexpr float yStep = 100.0f;

        const float totalWidth = static_cast<float>(maxIndex - minIndex + 1) * xStep;
        const float xOffset    = canvasPos.x + (canvasSize.x - totalWidth) * 0.5f;
        const float yOffset    = canvasPos.y + 40.0f;

        // Helper function to get the centre of the nodes.
        std::function<ImVec2(Node*)> GetNodeCentre = [&](Node* node) -> ImVec2
        {
            const float x = xOffset + static_cast<float>(pos[node].index - minIndex) * xStep;
            const float y = yOffset + static_cast<float>(pos[node].depth) * yStep;

            return ImVec2{ x, y };
        };

        // Draw the edges connecting the centres of the binary tree. Done first to be rendered under nodes.
        std::function<void(Node*)> DrawEdges = [&](Node* node)
        {
            if (!node)
            {
                return;
            }

            const ImVec2 centre = GetNodeCentre(node);

            constexpr uint32_t lineColor = IM_COL32(180, 180, 180, 255);

            if (node->left) 
            {
                const ImVec2 centreLeft = GetNodeCentre(node->left);
                drawList->AddLine(centre, centreLeft, lineColor, 2.0f);
                DrawEdges(node->left);
            }
            if (node->right) 
            {
                const ImVec2 centreRight = GetNodeCentre(node->right);
                drawList->AddLine(centre, centreRight, lineColor, 2.0f);
                DrawEdges(node->right);
            }
        };
        DrawEdges(root);

        // Draw the nodes of the binary tree.
        std::function<void(Node*)> DrawNodes = [&](Node* node)
        {
            if (!node)
            {
                return;
            }

            const ImVec2 centre = GetNodeCentre(node);

            constexpr uint32_t nodeColorSmallestBlock = IM_COL32(117, 223, 237, 255);
            constexpr uint32_t nodeColorLargestBlock  = IM_COL32(237, 131, 117, 255);
            constexpr uint32_t nodeColor        = IM_COL32(70, 120, 200, 255);
            constexpr uint32_t hoveredNodeColor = IM_COL32(70, 120, 255, 255);

            const bool smallest = MemoryManager::mInstance->GetSmallestBlockNode() == node;
            const bool largest  = MemoryManager::mInstance->GetLargestBlockNode()  == node;

            uint32_t leftColor  = smallest ? nodeColorSmallestBlock : nodeColor;
            uint32_t rightColor = largest  ? nodeColorLargestBlock  : nodeColor;

            leftColor  = !smallest ? rightColor : leftColor;
            rightColor = !largest  ? leftColor  : rightColor;

            // Draw the outline rect first to see the color.
            constexpr float borderWidth = 4.0f;
            drawList->AddRectFilledMultiColor({ centre.x - quadHalfSize.x - borderWidth, centre.y - quadHalfSize.y - borderWidth },
                                              { centre.x + quadHalfSize.x + borderWidth, centre.y + quadHalfSize.y + borderWidth },
                                          leftColor, rightColor, rightColor, leftColor);

            drawList->AddRectFilled({ centre.x - quadHalfSize.x, centre.y - quadHalfSize.y},
                                    { centre.x + quadHalfSize.x, centre.y + quadHalfSize.y }, nodeColor);

            // Strings for the text on the nodes and tooltip.
            char blockMemoryAddress[64];
            (void)snprintf(blockMemoryAddress, sizeof(blockMemoryAddress), "%" PRIX64, reinterpret_cast<uintptr_t>(node->start));
            const std::string blockFreeSpace = std::to_string(node->size);
            const std::string blockAllocated = node->parent ? std::to_string(node->parent->start - node->start - node->size) : "0";

            ImGui::SetCursorScreenPos({ centre.x - quadHalfSize.x, centre.y - quadHalfSize.y });
            ImGui::InvisibleButton(reinterpret_cast<const char*>(node), { quadHalfSize.x * 2, quadHalfSize.y * 2 });
            if (ImGui::IsItemHovered())
            {
                constexpr float sizeIncrease = 2.0f;
                drawList->AddRectFilled({ centre.x - quadHalfSize.x - sizeIncrease, centre.y - quadHalfSize.y - sizeIncrease }, 
                                        { centre.x + quadHalfSize.x + sizeIncrease, centre.y + quadHalfSize.y + sizeIncrease }, hoveredNodeColor);

                const std::string tooltip = std::format("Block: {}\nBlock Free : {}\nBlock Alloc: {}\nSmallest   : {}\nLargest    : {}", blockMemoryAddress, blockFreeSpace, blockAllocated, smallest ? "Y" : "N", largest ? "Y" : "N");
                ImGui::SetItemTooltip("%s", tooltip.c_str());
            }

            constexpr uint32_t white = IM_COL32(255, 255, 255, 255);
            constexpr uint32_t green = IM_COL32( 70, 200,  70, 255);
            constexpr uint32_t red   = IM_COL32(255,   0,   0, 255);

            ImVec2 textSize = ImGui::CalcTextSize(blockMemoryAddress);
            drawList->AddText({ centre.x - textSize.x * 0.5f, centre.y - textSize.y }, white, blockMemoryAddress);

            textSize = ImGui::CalcTextSize(blockFreeSpace.c_str());
            drawList->AddText({ centre.x - textSize.x * 0.5f - quadHalfSize.x * 0.5f, centre.y + 3.0f }, green, blockFreeSpace.c_str());

            textSize = ImGui::CalcTextSize(blockAllocated.c_str());
            drawList->AddText({ centre.x - textSize.x * 0.5f + quadHalfSize.x * 0.5f, centre.y + 3.0f }, red, blockAllocated.c_str());

            DrawNodes(node->left);
            DrawNodes(node->right);
        };
        DrawNodes(root);

        ImGui::Dummy(canvasSize);

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

        ImGui::Text("\nMemory allocated:\n%d of %d bytes", MemoryManager::mInstance->GetAllocated(), MemoryManager::mInstance->GetSize());
        ImGui::ProgressBar(1.0f - static_cast<float>(MemoryManager::mInstance->GetSizeFree()) / static_cast<float>(MemoryManager::mInstance->GetSize()), { 200.0f, 0.0f });

        ImGui::Text("\nStart of Memory Block:");
        char buf[64];
        (void)snprintf(buf, sizeof(buf), "%016" PRIX64, reinterpret_cast<uintptr_t>(MemoryManager::mInstance->GetStartOfMemoryBlock()));
        ImGui::Text(buf);
        ImGui::End();
    }

    constexpr ImVec4 clearColor{ 0.45f, 0.55f, 0.6f, 1.0f };

    ImGui::Render();
    SDL_SetRenderScale(mRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(mRenderer, clearColor.x, clearColor.y, clearColor.z, clearColor.w);
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

    if (node->left)
    {
        UpdateMemoryBlockVector(vector, node->left, barSize);
    }

    if (node->right)
    {
        UpdateMemoryBlockVector(vector, node->right, barSize);
    }
}

void DebugMemoryConsole::AssignPositionsForBinaryTree(Node* node, const int depth, int& order, std::unordered_map<Node*, BinaryTreePosition>& positions) 
{
    if (!node)
    {
        return;
    }

    AssignPositionsForBinaryTree(node->left, depth + 1, order, positions);

    positions[node] = {  .index = order, .depth = depth };

    ++order;

    AssignPositionsForBinaryTree(node->right, depth + 1, order, positions);
}

} // Namespace ignite::mem.

#endif // DEV_CONFIGURATION.