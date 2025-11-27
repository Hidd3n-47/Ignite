#include "MainMenuApplicationState.h"

#include <IgniteMem/Core/WeakHandle.h>

#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/Core/Engine.h>
#include <IgniteEngine/EC/Components/UiButton.h>
#include <IgniteEngine/EC/Components/Transform.h>

#include "Core/GameManager.h"

namespace ignite
{
void MainMenuApplicationState::InitScene()
{
    mem::WeakHandle<GameObject> playButtonObject = CreateGameObject();
    playButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, 2.0f };
    mem::WeakHandle<UiButton> playButton = playButtonObject->AddComponent<UiButton>("Assets/PlayButton.png", true);
    playButton->SetOnButtonPressedEvent([] { GameManager::Instance()->ChangeState(ApplicationStates::LEVEL_SELECT); });
    playButton->SetOnHoveredEvent(
        [](mem::WeakHandle<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.2f };
        });
    playButton->SetOnRestingStateEvent(
        [](mem::WeakHandle<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.0f };
        });


    mem::WeakHandle<GameObject> exitButtonObject = CreateGameObject();
    exitButtonObject->GetComponent<Transform>()->translation = Vec2{ 0.0f, -2.0f };
    mem::WeakHandle<UiButton> exitButton = exitButtonObject->AddComponent<UiButton>("Assets/QuitButton.png", true);
    exitButton->SetOnButtonPressedEvent([] { Engine::Instance()->CloseGame(); });
    exitButton->SetOnHoveredEvent(
        [](mem::WeakHandle<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.2f };
        });
    exitButton->SetOnRestingStateEvent(
        [](mem::WeakHandle<GameObject> gameObject)
        {
            gameObject->GetComponent<Transform>()->scale = Vec2{ 1.0f };
        });
}

} // Namespace ignite.