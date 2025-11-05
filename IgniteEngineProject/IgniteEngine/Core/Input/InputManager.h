#pragma once

namespace ignite
{

using keycode = uint16_t;
using mouse   = uint16_t;

class InputManager
{
public:
    InputManager()  = default;
    ~InputManager() = default;

    InputManager(const InputManager&)             = delete;
    InputManager(InputManager&&)                  = delete;
    InputManager& operator=(InputManager&&)       = delete;
    InputManager& operator=(const InputManager&)  = delete;

    void Poll();
    inline void StartFrame() { mKeyMapForSingleFrame.clear(); mMouseButtonMapForSingleFrame.clear(); }

    void KeyDown(const keycode key);
    void KeyUp(const keycode key);
    void MouseDown(const mouse button);
    void MouseUp(const mouse button);

    [[nodiscard]] inline bool IsKeyDown(const keycode key)         const { if (!mKeyMap.contains(key)) return false; return mKeyMap.at(key); }
    [[nodiscard]] inline bool IsMouseDown(const mouse button)      const { if (!mMouseButtonMap.contains(button)) return false; return mMouseButtonMap.at(button); }
    [[nodiscard]] inline bool IsKeyPressed(const keycode key)      const { return mKeyMapForSingleFrame.contains(key); }
    [[nodiscard]] inline bool IsMousePressed(const mouse button)   const { return mMouseButtonMapForSingleFrame.contains(button); }
    [[nodiscard]] inline Vec2 GetMousePosition() const { return mMousePosition; }

    inline void SetMousePosition(const Vec2 mousePosition) { mMousePosition = mousePosition; }
private:

    std::unordered_map<uint16_t, bool> mKeyMap;
    std::unordered_set<uint16_t>       mKeyMapForSingleFrame;

    Vec2 mMousePosition;
    std::unordered_map<uint16_t, bool> mMouseButtonMap;
    std::unordered_set<uint16_t>       mMouseButtonMapForSingleFrame;
};

} // Namespace ignite.