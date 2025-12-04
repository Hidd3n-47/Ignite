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
    std::ofstream statsOutput("MemoryManagerStats.log");
    if (!statsOutput.fail())
    {
        statsOutput << std::format("Max number of Memory Allocated: {:.2}%", mInstance->mMaxAllocationPercent)      << "\n";
        statsOutput << std::format("Average allocation percentage : {:.2}%", mInstance->mAverageAllocation.average) << "\n";
        statsOutput << std::format("Max number of Memory Fragments: {}"    , mInstance->mMaxNumberOfFragments)      << "\n";
        statsOutput << std::format("Average memory fragments used : {:.2}" , mInstance->mAverageFragments.average)  << "\n";
    }

    delete mInstance;
    mInstance = nullptr;
}

void DebugMemoryConsole::Run()
{
    while (mRunning)
    {
        Render();
        Update();
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

void DebugMemoryConsole::Render()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    const ImGuiIO& io = ImGui::GetIO();

    {
        ImGui::Begin("Free memory block linked list");

        ListNode<MemoryBlock>* head = MemoryManager::mInstance->mStartingListNode;

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImVec2 canvasPos  = ImGui::GetCursorScreenPos();
        const ImVec2 canvasSize = ImGui::GetContentRegionAvail();

        drawList->AddRectFilled(canvasPos,
            { canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y },
            IM_COL32(32, 32, 32, 255));

        constexpr ImVec2 quadHalfSize = { 50.0f, 25.0f };
        constexpr float  xStep   = 140.0f;
        const float      yOffset = canvasPos.y + 60.0f;

        float xOffset = canvasPos.x + 100.0f;

        ListNode<MemoryBlock>* node = head;
        while (node)
        {
            const ImVec2 centre = { xOffset, yOffset };

            drawList->AddRectFilled(
                { centre.x - quadHalfSize.x, centre.y - quadHalfSize.y },
                { centre.x + quadHalfSize.x, centre.y + quadHalfSize.y },
                IM_COL32(70, 120, 200, 255));

            std::string blockMemoryAddress = std::format("{:X}", reinterpret_cast<uintptr_t>(node->value.address));
            std::string blockFreeSpace     = std::to_string(node->value.sizeFree);

            ImVec2 textSize = ImGui::CalcTextSize(blockMemoryAddress.c_str());
            drawList->AddText({ centre.x - textSize.x * 0.5f, centre.y - textSize.y },
                IM_COL32(255, 255, 255, 255), blockMemoryAddress.c_str());

            textSize = ImGui::CalcTextSize(blockFreeSpace.c_str());
            drawList->AddText({ centre.x - textSize.x * 0.5f, centre.y + 5.0f },
                IM_COL32(70, 200, 70, 255), blockFreeSpace.c_str());

            if (node->next)
            {
                const ImVec2 nextCentre = { xOffset + xStep, yOffset };
                drawList->AddLine({ centre.x + quadHalfSize.x, centre.y },
                    { nextCentre.x - quadHalfSize.x, nextCentre.y },
                    IM_COL32(180, 180, 180, 255), 2.0f);
            }

            xOffset += xStep;
            node = node->next;
        }

        ImGui::Dummy(canvasSize);
        ImGui::End();

    }

    {
        ImGui::Begin("Allocation Stats");

        ImGui::Text("Memory Blocks:");
        const uint64_t size = MemoryManager::mInstance->GetSize();

        constexpr uint32_t numMemorySlices = 128;

        std::vector<float> values(numMemorySlices, 1.0f);

        const uint64_t sliceWidth = static_cast<uint64_t>(size / numMemorySlices);
        UpdateMemoryBlockVector(values, sliceWidth);

        ImGui::PlotHistogram("##Memory block", values.data(), static_cast<int>(values.size()), 0, nullptr, 0.0f, 1.0f);

        const float allocatedPercentDecimal = 1.0f - static_cast<float>(MemoryManager::mInstance->GetSizeFree()) / static_cast<float>(size);
        mMaxAllocationPercent = std::max(mMaxAllocationPercent, 100 * allocatedPercentDecimal);
        mAverageAllocation.UpdateAverage(allocatedPercentDecimal * 100.0f);

        ImGui::Text("\nMemory allocated:\n%llu of %llu bytes", MemoryManager::mInstance->GetAllocated(), size);
        ImGui::ProgressBar(allocatedPercentDecimal, { 200.0f, 0.0f });

        const uint32_t fragCapacity = MemoryManager::mInstance->mListNodeFreeIndicesStack.capacity();
        const uint32_t fragFree     = MemoryManager::mInstance->mListNodeFreeIndicesStack.size();
        const uint32_t fragments    = MemoryManager::mInstance->GetNumberOfFragments();
        mMaxNumberOfFragments = std::max(mMaxNumberOfFragments, fragments);
        mAverageFragments.UpdateAverage(fragments);

        ImGui::Text("\nMemory Fragmentation:\n%llu of %llu bytes", fragments, fragCapacity);
        ImGui::ProgressBar(1.0f - static_cast<float>(fragFree) / static_cast<float>(fragCapacity), { 200.0f, 0.0f });

        std::string blockMemoryAddress = std::format("{:X}", reinterpret_cast<uintptr_t>(MemoryManager::mInstance->GetStartOfMemoryBlock()));
        ImGui::Text("\nStart of Memory Block:\n%s", blockMemoryAddress.c_str());

        ImGui::SameLine(300.0f);

        ImGui::Text("\nNumber of Memory Fragments:\n%u", fragments);

        ImGui::End();
    }

    {
        ImGui::Begin("Memory Manager Runtime Stats");

        ImGui::Text("Max number of Memory Allocated: %.2f%%", mMaxAllocationPercent);
        ImGui::Text("\nAverage allocation percentage : %.2f", mAverageAllocation.average);
        ImGui::Text("\nMax number of Memory Fragments: %u"  , mMaxNumberOfFragments);
        ImGui::Text("\nAverage memory fragments used : %.2f", mAverageFragments.average);

        ImGui::End();
    }

    {
        ImGui::Begin("Allocation Info");

        if (ImGui::BeginTable("AllocInfoTable", 4))
        {
            ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 55.0f);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Name");
            ImGui::TableHeadersRow();

            AllocationMap allocationMap = MemoryManager::Instance()->GetAllocationMap();

            for (const auto& [address, info] : allocationMap)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", info.id);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%llu", info.size);

                ImGui::TableSetColumnIndex(2);
                std::string blockMemoryAddress = std::format("{:X}", reinterpret_cast<uintptr_t>(address));
                ImGui::Text("%s", blockMemoryAddress.c_str());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", info.allocationLocation);
            }

            ImGui::EndTable();
        }


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

void DebugMemoryConsole::UpdateMemoryBlockVector(std::vector<float>& vector, const uint64_t barSize)
{
    const uint64_t memoryStart  = (uint64_t)MemoryManager::mInstance->GetStartOfMemoryBlock();
    ListNode<MemoryBlock>* node = MemoryManager::mInstance->mStartingListNode;

    std::fill(vector.begin(), vector.begin(), 1.0f);

    while (node)
    {
        const size_t start = reinterpret_cast<std::intptr_t>(node->value.address - memoryStart) / barSize;
        const size_t end   = start + static_cast<size_t>(std::ceil(static_cast<float>(node->value.sizeFree) / barSize));

        node = node->next;

        if (start > vector.size())
        {
            continue;
        }

        std::fill(vector.begin() + start, vector.begin() + std::min(end, vector.size()), 0.1f);
    }
}

} // Namespace ignite::mem.

#endif // DEV_CONFIGURATION.