#pragma once

#ifdef DEV_CONFIGURATION
#define DEBUG(X) X
#else  // DEV_CONFIGURATION
#define DEBUG(X)
#endif // !DEV_CONFIGURATION