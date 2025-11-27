#include "IgnitePch.h"
#include "Scene.h"

namespace ignite
{

Scene::~Scene()
{
    PROFILE_FUNC();

    for (const GameObject* go : mGameObjects)
    {
        delete go;
    }
}

void Scene::Update(const float dt) const
{
    PROFILE_FUNC();

    for (const GameObject* gameObject : mGameObjects)
    {
        gameObject->Update(dt);
    }
}

void Scene::Render(mem::WeakHandle<Renderer> renderer) const
{
    PROFILE_FUNC();

    for (const GameObject* gameObject : mGameObjects)
    {
        gameObject->Render(renderer);
    }
}

mem::WeakHandle<GameObject> Scene::CreateGameObject()
{
    PROFILE_FUNC();

    mGameObjects.emplace_back(new GameObject{ Ulid{}, mem::WeakHandle{ this } });
    return mem::WeakHandle{ mGameObjects.back() };
}

} // Namespace ignite.
