#include "LevelSelectApplicationState.h"

#include <IgniteEngine/EC/Components/UiButton.h>

#include "Core/GameManager.h"
#include "GameApplicationState.h"

namespace ignite
{

void LevelSelectApplicationState::InitScene()
{
    {
        mem::WeakHandle<GameObject> levelOneObject = CreateGameObject();

        mem::WeakHandle<Transform> transform = levelOneObject->GetComponent<Transform>();
        transform->translation = Vec2{ -4.0f, 1.0f };
        transform->scale = Vec2{ 0.3f };

        mem::WeakHandle<UiButton> playButton = levelOneObject->AddComponent<UiButton>("Assets/Levels/Level1/Track.png");
        playButton->SetOnButtonPressedEvent([] { GameManager::Instance()->ChangeState(ApplicationStates::GAME, new GameApplicationStateInitInfo(LevelState::ONE)); });
        playButton->SetOnHoveredEvent(
            [](mem::WeakHandle<GameObject> gameObject)
            {
                gameObject->GetComponent<Transform>()->scale = Vec2{ 0.7f };
            });
        playButton->SetOnRestingStateEvent(
            [](mem::WeakHandle<GameObject> gameObject)
            {
                gameObject->GetComponent<Transform>()->scale = Vec2{ 0.5f };
            });
    }

    {
        mem::WeakHandle<GameObject> levelTwoObject = CreateGameObject();

        mem::WeakHandle<Transform> transform = levelTwoObject->GetComponent<Transform>();
        transform->translation = Vec2{ 4.0f, 1.0f };
        transform->scale = Vec2{ 0.3f };

        mem::WeakHandle<UiButton> playButton = levelTwoObject->AddComponent<UiButton>("Assets/Levels/Level2/Track.png");
        playButton->SetOnButtonPressedEvent([] { GameManager::Instance()->ChangeState(ApplicationStates::GAME, new GameApplicationStateInitInfo(LevelState::TWO)); });
        playButton->SetOnHoveredEvent(
            [](mem::WeakHandle<GameObject> gameObject)
            {
                gameObject->GetComponent<Transform>()->scale = Vec2{ 0.7f };
            });
        playButton->SetOnRestingStateEvent(
            [](mem::WeakHandle<GameObject> gameObject)
            {
                gameObject->GetComponent<Transform>()->scale = Vec2{ 0.5f };
            });
    }

    mem::WeakHandle<GameObject> backButtonObject = CreateGameObject();
    backButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, -2.0f };
    mem::WeakHandle<UiButton> backButton = backButtonObject->AddComponent<UiButton>("Assets/BackButton.png", true);
    backButton->SetOnButtonPressedEvent([] { GameManager::Instance()->ChangeState(ApplicationStates::MAIN_MENU); });
    backButton->SetOnHoveredEvent(
        [](mem::WeakHandle<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.2f };
        });
    backButton->SetOnRestingStateEvent(
        [](mem::WeakHandle<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.0f };
        });
}

} // Namespace ignite.