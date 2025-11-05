#pragma once

#ifdef DEV_CONFIGURATION
#include "Log/Log.h"
#define GAME_BREAK() __debugbreak()
#define GAME_DEBUG(...) __VA_ARGS__
#define GAME_LOG(...)   ignite::Log::Debug("IgniteGame", __VA_ARGS__)
#define GAME_INFO(...)  ignite::Log::Info("IgniteGame", __VA_ARGS__)
#define GAME_WARN(...)  ignite::Log::Warn("IgniteGame", __VA_ARGS__)
#define GAME_ERROR(...) ignite::Log::Error("IgniteGame", __VA_ARGS__)
#else  // DEV_CONFIGURATION.
#define GAME_DEBUG_BREAK()
#define GAME_DEBUG(...)
#define DEBUG_LOG(...)
#define DEBUG_INFO(...)
#define DEBUG_WARN(...)
#define DEBUG_ERROR(...)
#endif // ! DEV_CONFIGURATION.