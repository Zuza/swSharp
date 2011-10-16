#ifndef __SW_THRESHOLDH__
#define __SW_THRESHOLDH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* swThreshold(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);

#endif // __SW_THRESHOLDH__
