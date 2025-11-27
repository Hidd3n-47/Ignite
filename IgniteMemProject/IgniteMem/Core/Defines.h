#pragma once

#ifdef DEV_CONFIGURATION
#include <Logger/Log.h>

#define DEBUG(X) X
#define MEM_LOG_DEBUG(...) MemoryManager::Instance()->logger.Debug(__VA_ARGS__)
#define MEM_LOG_INFO(...)  MemoryManager::Instance()->logger.Info(__VA_ARGS__)
#define MEM_LOG_WARN(...)  MemoryManager::Instance()->logger.Warn(__VA_ARGS__)
#define MEM_LOG_ERROR(...) MemoryManager::Instance()->logger.Error(__VA_ARGS__)
#else  // DEV_CONFIGURATION
#define DEBUG(X)
#define MEM_LOG_DEBUG(...)
#define MEM_LOG_INFO(...) 
#define MEM_LOG_WARN(...) 
#define MEM_LOG_ERROR(...)
#endif // !DEV_CONFIGURATION

#ifdef MEM_MANAGER
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
