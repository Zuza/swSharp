#ifndef __SW_FIND_GPUMMH__
#define __SW_FIND_GPUMMH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* swFindGPUMM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, MatcherScore score);
#endif // __SW_FIND_GPUMMH__
