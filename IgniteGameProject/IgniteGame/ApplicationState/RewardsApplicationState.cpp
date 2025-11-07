#include "RewardsApplicationState.h"

#include <IgniteEngine/Core/Engine.h>
#include <IgniteEngine/Core/Rendering/TextureManager.h>

#include <IgniteEngine/EC/Components/UiText.h>
#include <IgniteEngine/EC/Components/UiButton.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>

#include "Core/GameManager.h"

namespace ignite
{

RewardsApplicationState::RewardsApplicationState(mem::WeakRef<RewardsApplicationStateInitInfo> initInfo)
    : mPlayerRanking(initInfo->ranking)
{
    // Empty.
}

void RewardsApplicationState::InitScene()
{
    mTextObject = CreateGameObject();
    mem::WeakRef<GameObject> rewardVisual = CreateGameObject();

    if (mPlayerRanking == TrophyRanking::NONE)
    {
        mTextObject->AddComponent<UiText>("Too Slow! Try again", 150.0f);
    }
    else
    {
        Texture texture;
        Engine::Instance()->GetTextureManager()->Load(texture, "E:/Programming/Ignite/Assets/TrophySpritesheet.png", 3);

        rewardVisual->AddComponent<SpriteRenderer>(texture, static_cast<float>(mPlayerRanking), 0.0f, 100);

        rewardVisual->GetComponent<Transform>()->scale = Vec2{ 10.0f, 10.0f };

        mTextObject->AddComponent<UiText>("Well done!", 150.0f);
        mTextObject->GetComponent<Transform>()->translation.y = 2.5f;
    }

    mem::WeakRef<GameObject> backButtonObject = CreateGameObject();
    backButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, -2.0f };
    mem::WeakRef<UiButton> backButton = backButtonObject->AddComponent<UiButton>(std::filesystem::path{ "E:/Programming/Ignite/Assets/BackButton.png" }, true);
    backButton->SetOnButtonPressedEvent([&] { mTextObject->RemoveComponent<UiText>(); GameManager::Instance()->ChangeState(ApplicationStates::MAIN_MENU); });
    backButton->SetOnHoveredEvent(
        [](mem::WeakRef<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.2f };
        });
    backButton->SetOnRestingStateEvent(
        [](mem::WeakRef<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.0f };
        });
}

} // Namespace ignite.