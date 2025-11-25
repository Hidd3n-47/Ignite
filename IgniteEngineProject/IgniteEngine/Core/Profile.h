#pragma once

#ifdef DEV_CONFIGURATION
#include <IgniteUtils/Core/InstrumentationTimer.h>
#define PROFILE_FUNC() ignite::utils::InstrumentationTimer t{__FUNCTION__}
#else  // DEV_CONFIGURATION.
#define PROFILE_FUNC()
#endif // ! DEV_CONFIGURATION.
