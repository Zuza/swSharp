#ifndef __SW_FIND_GPUSMH__
#define __SW_FIND_GPUSMH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* swFindGPUSM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, MatcherScore score);
#endif // __SW_SOLVE_GPUSMH__
