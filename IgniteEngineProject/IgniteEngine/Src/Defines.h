#pragma once

#ifdef DEV_CONFIGURATION
#define DEBUG_BREAK() __debugbreak()
#define DEBUG(...) __VA_ARGS__
#else  // DEV_CONFIGURATION.
#define DEBUG_BREAK()
#define DEBUG(...)
#endif // ! DEV_CONFIGURATION.

#ifdef ENGINE_DEBUG
#include "IgniteEngine/Core/Engine.h"
#define DEBUG_LOG(...)   Engine::Instance()->logger.Debug(__VA_ARGS__)
#define DEBUG_INFO(...)  Engine::Instance()->logger.Info(__VA_ARGS__)
#define DEBUG_WARN(...)  Engine::Instance()->logger.Warn(__VA_ARGS__)
#define DEBUG_ERROR(...) Engine::Instance()->logger.Error(__VA_ARGS__)
#else // ENGINE_DEBUG.
#define DEBUG_LOG(...)
#define DEBUG_INFO(...)
#define DEBUG_WARN(...)
#define DEBUG_ERROR(...)
#endif // !ENGINE_DEBUG.