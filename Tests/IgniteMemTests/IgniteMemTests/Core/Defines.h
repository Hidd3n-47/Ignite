#pragma once

#ifdef DEV_LIVE_STATS
#define DEV_PAUSE() system("PAUSE")
#else
#define DEV_PAUSE()
#endif