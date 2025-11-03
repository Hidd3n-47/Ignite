#pragma once

#include "IgniteEngine/EC/GameObject.h"

namespace ignite
{
/**
 * @class Scene: A class that represents an abstract idea of a 'Scene'.
 * A \c Scene is a collection of \c GameObjects.
 */
class Scene
{
public:
    Scene() = default;
    virtual ~Scene();

    inline virtual void InitScene() { }
    inline virtual void SceneUpdate() const { }

    void Update(const float dt) const;
    void Render() const;

    [[nodiscard]] mem::WeakRef<GameObject> CreateGameObject();
private:
    std::vector<GameObject*> mGameObjects;
};

} // Namespace ignite.