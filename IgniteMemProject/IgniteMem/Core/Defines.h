#pragma once

#ifdef DEV_CONFIGURATION
#define DEBUG(X) X
#else  // DEV_CONFIGURATION
#define DEBUG(X)
#endif // !DEV_CONFIGURATION

#ifdef MEM_MANAGER
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
