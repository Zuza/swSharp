#ifndef __SW_NW_FIND_SOLVE_CPUH__
#define __SW_NW_FIND_SOLVE_CPUH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* swSolveCPU(Chain* rowChain, Chain* columnChain, SWPrefs* swPrefs);

extern SWData* swFindCPU(Chain* rowChain, Chain* columnChain, SWPrefs* swPrefs,
    MatcherScore score);

extern SWData* nwSolveCPU(Chain* rowChain, Chain* columnChain, SWPrefs* swPrefs);
#endif // __SW_NW_FIND_SOLVE_CPUH__
