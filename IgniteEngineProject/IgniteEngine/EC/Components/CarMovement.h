#pragma once

#include "IgniteEngine/EC/IComponent.h"
#include "IgniteEngine/EC/IUpdateableComponent.h"

namespace ignite
{

class Transform;
class InputManager;
class SpriteRenderer;

class CarMovement : public IComponent, public IUpdateableComponent
{
public:
    void OnComponentAdded(const mem::WeakRef<GameObject> parent) override;
    void Update(const float dt) override;

    inline void EnableMovement() { mMovementEnabled = true; }
    [[nodiscard]] inline bool IsMoving() const { return mMoving; }
private:
    mem::WeakRef<Transform>    mTransform;
    mem::WeakRef<InputManager> mInputManagerRef;

    bool  mMovementEnabled = false;
    bool  mMoving    = false;
    float mSpeed     = 4.0f;
    float mTurnSpeed = 40.0f;
};

} // Namespace ignite.