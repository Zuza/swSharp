#ifndef __SW_SOLVE_GPUH__
#define __SW_SOLVE_GPUH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* swSolveGPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);
#endif // __SW_SOLVE_GPUH__
