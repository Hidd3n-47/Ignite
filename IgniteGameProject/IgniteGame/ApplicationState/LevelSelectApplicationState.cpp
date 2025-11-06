#include "LevelSelectApplicationState.h"

#include <IgniteEngine/EC/Components/UiButton.h>

#include "Core/GameManager.h"

namespace ignite
{

void LevelSelectApplicationState::InitScene()
{
    mem::WeakRef<GameObject> playButtonObject = CreateGameObject();

    mem::WeakRef<Transform> transform = playButtonObject->GetComponent<Transform>();
    transform->translation = Vec2{ 0.0f, 1.0f };
    transform->scale       = Vec2{ 0.5f };

    mem::WeakRef<UiButton> playButton = playButtonObject->AddComponent<UiButton>(std::filesystem::path{ "E:/Programming/Ignite/Assets/Levels/Level1/Level1Track.png" });
    playButton->SetOnButtonPressedEvent([] { GameManager::Instance()->ChangeState(ApplicationStates::GAME); });
    playButton->SetOnHoveredEvent(
        [](mem::WeakRef<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 0.7f };
        });
    playButton->SetOnRestingStateEvent(
        [](mem::WeakRef<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 0.5f };
        });

    mem::WeakRef<GameObject> backButtonObject = CreateGameObject();
    backButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, -2.0f };
    mem::WeakRef<UiButton> backButton = backButtonObject->AddComponent<UiButton>(std::filesystem::path{ "E:/Programming/Ignite/Assets/BackButton.png" }, true);
    backButton->SetOnButtonPressedEvent([] { GameManager::Instance()->ChangeState(ApplicationStates::MAIN_MENU); });
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

void LevelSelectApplicationState::SceneUpdate() const
{

}

} // Namespace ignite.