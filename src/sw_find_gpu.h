#ifndef __SW_FIND_GPUH__
#define __SW_FIND_GPUH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* swFindGPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, MatcherScore score);
#endif // __SW_FIND_GPUH__
