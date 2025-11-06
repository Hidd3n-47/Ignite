#pragma once

#include <cstdint>

namespace ignite
{
enum class ApplicationStates : uint8_t
{
    MAIN_MENU,
    LEVE_SELECT,
    GAME,
    REWARDS
};

/**
 * @class IApplicationStateInitInfo: A class used to represent initialization information when changing application states.
 * This class should be inherited by a new class that will represent the information that needs to be processed and passed to the application state on creation.
 */
class IApplicationStateInitInfo
{
public:
    virtual ~IApplicationStateInitInfo() = default;
};

} // Namespace ignite.