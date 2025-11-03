#pragma once

#ifdef DEV_CONFIGURATION
#define DEBUG_BREAK() __debugbreak()
#define DEBUG(...) __VA_ARGS__
#else  // DEV_CONFIGURATION.
#define DEBUG_BREAK()
#define DEBUG(...)
#endif // ! DEV_CONFIGURATION.

#ifdef ENGINE_DEBUG
#define DEBUG_LOG(...)   ignite::Log::Debug("IgniteEngine", __VA_ARGS__)
#define DEBUG_INFO(...)  ignite::Log::Info("IgniteEngine", __VA_ARGS__)
#define DEBUG_WARN(...)  ignite::Log::Warn("IgniteEngine", __VA_ARGS__)
#define DEBUG_ERROR(...) ignite::Log::Error("IgniteEngine", __VA_ARGS__)
#else // ENGINE_DEBUG.
#define DEBUG_LOG(...)
#define DEBUG_INFO(...)
#define DEBUG_WARN(...)
#define DEBUG_ERROR(...)
#endif // !ENGINE_DEBUG.