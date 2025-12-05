#pragma once

namespace ignite
{


void BuildLevel1(mem::WeakHandle<Scene> scene, mem::WeakHandle<GameObject> player, const Texture& levelSpritesheet, std::unordered_map<uint32_t, CollisionInfoRect>& collisionMap);
void BuildLevel2(mem::WeakHandle<Scene> scene, mem::WeakHandle<GameObject> player, const Texture& levelSpritesheet, std::unordered_map<uint32_t, CollisionInfoRect>& collisionMap);

}
