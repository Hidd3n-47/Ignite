#include "IgnitePch.h"
#include "Scene.h"

namespace ignite
{

Scene::~Scene()
{
    for (const GameObject* go : mGameObjects)
    {
        delete go;
    }
}

void Scene::Update(const float dt) const
{
    for (const GameObject* gameObject : mGameObjects)
    {
        gameObject->Update(dt);
    }
}

void Scene::Render(const OrthoCamera& camera) const
{
    for (const GameObject* gameObject : mGameObjects)
    {
        gameObject->Render(camera);
    }
}

mem::WeakRef<GameObject> Scene::CreateGameObject()
{
    mGameObjects.emplace_back(new GameObject{ Ulid{}, mem::WeakRef{ this } });
    return mem::WeakRef{ mGameObjects.back() };
}

} // Namespace ignite.
