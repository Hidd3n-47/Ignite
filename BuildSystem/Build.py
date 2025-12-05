import os
import csv
import xml.etree.ElementTree as et
from pathlib import Path

assets_path       = "../IgniteGameProject/IgniteGame/Assets/"
game_project_path = "..//IgniteGameProject/IgniteGame/"
output_path       = game_project_path + "Level/Build/"

tree = et.parse(assets_path + "EnvironmentSpritesheet.tsx")
root = tree.getroot()

entries = []
for child in root.findall("tile"):
    tile_id = int(child.attrib["id"])

    obj = child.find("objectgroup/object")
    if obj is None:
        continue

    x = float(obj.attrib["x"])
    y = float(obj.attrib["y"])
    w = float(obj.attrib["width"])
    h = float(obj.attrib["height"])

    entries.append((tile_id, x, y, w, h))

with open(output_path + "GeneratedCollisionInfo.cpp", "w") as f:
    f.write(
'''#include <unordered_map>

#ifndef DEV_CONFIGURATION

#include "Level/LevelParser.h"
#include "Level/CollisionInfoRect.h"

void ignite::LevelParser::CreateCollisionMap()
{
    mSpritesheetIdToCollisionInfo =
    {\n''')

    for tile_id, x, y, w, h in entries:
        f.write(f'        {{ {tile_id}, ignite::CollisionInfoRect{{ .x = {x}f, .y = {y}f, .w = {w}f, .h = {h}f }} }},\n')

    f.write('    };\n}\n\n#endif // !DEV_CONFIGURATION.\n')

levels_root = Path(assets_path + "Levels/")

with open(output_path + "BuildTiles.h", "w") as f:
    f.write(
'''#pragma once
    
#include <IgniteEngine/EC/Scene.h>
#include <IgniteEngine/EC/Components/Transform.h>
#include <IgniteEngine/EC/Components/BoxCollider.h>
#include <IgniteEngine/EC/Components/SpriteRenderer.h>
#include <IgniteEngine/EC/Components/RaceManagerComponent.h>

#include <IgniteEngine/Core/Engine.h>

#include "Core/GameManager.h"
#include "Level/LevelParser.h"
#include "Level/CollisionInfoRect.h"

namespace ignite
{

static void BuildTile(const uint32_t tileId, const uint32_t layer, const uint32_t x, const uint32_t y, 
                      const OrthoCamera& camera, const float tileSize, const float scalingFactor, mem::WeakHandle<Scene> scene, mem::WeakHandle<GameObject> player, const Texture& levelSpritesheet, std::unordered_map<uint32_t, CollisionInfoRect>& collisionMap)
{
    const float spritesheetX = static_cast<float>(tileId % 7);
    const float spritesheetY = static_cast<float>(tileId / 7);
    const Vec2 position{ static_cast<float>(x) - 7.5f, -static_cast<float>(y) + 4.5f };

    mem::WeakHandle<GameObject> gameObject = scene->CreateGameObject();

    gameObject->AddComponent<SpriteRenderer>(levelSpritesheet, spritesheetX, spritesheetY, layer);

    mem::WeakHandle<Transform> transform = gameObject->GetComponent<Transform>();
    transform->scale       = Vec2{scalingFactor, scalingFactor};
    transform->translation = position;

    if (tileId == LevelParser::START_LINE_TEXTURE_ID)
    {
        player->GetComponent<Transform>()->translation = position - Vec2{ 0.4f, 0.0f };
    }

    if (collisionMap.contains(tileId))
    {
        const CollisionInfoRect rect = collisionMap[tileId];

        const Vec2 centre      = Vec2{rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f};
        const Vec2 offset      = centre - Vec2{tileSize * 0.5f};
        const Vec2 centreWorld = camera.ScreenSizeToWorldSize(offset);
        const Vec2 sizeWorld   = camera.ScreenSizeToWorldSize(Vec2{rect.w, rect.h});

        mem::WeakHandle<BoxCollider> box = gameObject->AddComponent<BoxCollider>(sizeWorld, false, tileId == LevelParser::START_LINE_TEXTURE_ID);
        box->SetOffset(Vec2{centreWorld.x, -centreWorld.y});
        box->SetOnTriggeredCallback([](mem::WeakHandle<GameObject> other) -> void {
            other->GetComponent<RaceManagerComponent>()->PassedStartFinishLine();
        });
    }
}

} // Namespace ignite.
''')

header_lines = ["#pragma once\n\nnamespace ignite\n{\n"]

for level_dir in sorted(levels_root.iterdir()):
    if not level_dir.is_dir():
        continue

    level_name = level_dir.name  # e.g. "Level1"
    cpp_filename = f"Generated{level_name}Build.cpp"


    track = []
    bg = []
    ranks = []

    # The level data is defined in the Track.csv and BG.csv file.
    for filename, target in [("Track.csv", track), ("BG.csv", bg)]:
        file_path = level_dir / filename
        if file_path.exists():
            with open(file_path) as f:
                reader = csv.reader(f)
                for row in reader:
                    target.append([int(x) for x in row])

    # The results and desired times are stored in the Ranks.lvl file.
    ranks_file = level_dir / "Ranks.lvl"
    if ranks_file.exists():
        with open(ranks_file) as f:
            ranks = [float(line.strip()) for line in f.readlines()]

    with open(output_path + cpp_filename, "w") as f:

        f.write('#include "BuildTiles.h"\n\n')

        f.write('namespace ignite\n{\n\n')

        f.write(f'void Build{level_name}(const mem::WeakHandle<Scene> scene, const mem::WeakHandle<GameObject> player, const Texture& levelSpritesheet, std::unordered_map<uint32_t, CollisionInfoRect>& collisionMap)\n')
        f.write('{\n')
        f.write('    const OrthoCamera& camera       = Engine::Instance()->GetCamera();\n')
        f.write('    const float tileSize            = levelSpritesheet.width;\n')
        f.write('    const float tileSizeScreenspace = camera.SizeInScreenSpace(Vec2{1.0f, 0.0f}).x;\n')
        f.write('    const float scalingFactor       = tileSizeScreenspace / tileSize;\n\n')

        for layer, grid in enumerate([track, bg]):
            f.write(f'    // Layer {layer}\n')
            for y, row in enumerate(grid):
                for x, tileId in enumerate(row):
                    if tileId == -1:
                        continue
                    f.write(f'    BuildTile({tileId}, {layer}, {x}, {y}, camera, tileSize, scalingFactor, scene, player, levelSpritesheet, collisionMap);\n')
        f.write('\n')

        if ranks:
            f.write(f'    GameManager::Instance()->SetLevelRankTimes({ranks[0]}f, {ranks[1]}f, {ranks[2]}f);\n')

        f.write('}\n\n')

        f.write('} // Namespace ignite.\n')

    # Add declaration to header
    header_lines.append(f'void Build{level_name}(mem::WeakHandle<Scene> scene, mem::WeakHandle<GameObject> player, const Texture& levelSpritesheet, std::unordered_map<uint32_t, CollisionInfoRect>& collisionMap);')

header_lines.append('\n} // Namespace ignite.\n')

# --- Write header file ---
with open(output_path + "GeneratedLevels.h", "w") as f:
    f.write("\n".join(header_lines))
