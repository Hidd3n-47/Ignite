#pragma once

#ifdef DEV_CONFIGURATION
#include "Logger/Log.h"
#define GAME_BREAK() __debugbreak()
#define GAME_DEBUG(...) __VA_ARGS__
#define GAME_LOG(...)   GameManager::Instance()->logger.Debug(__VA_ARGS__)
#define GAME_INFO(...)  GameManager::Instance()->logger.Info(__VA_ARGS__)
#define GAME_WARN(...)  GameManager::Instance()->logger.Warn(__VA_ARGS__)
#define GAME_ERROR(...) GameManager::Instance()->logger.Error(__VA_ARGS__)
#else  // DEV_CONFIGURATION.
#define GAME_BREAK()
#define GAME_DEBUG(...)
#define GAME_LOG(...)
#define GAME_INFO(...)
#define GAME_WARN(...)
#define GAME_ERROR(...)
#endif // ! DEV_CONFIGURATION.