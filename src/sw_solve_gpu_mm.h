#ifndef __SW_SOLVE_GPUMMH__
#define __SW_SOLVE_GPUMMH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* swSolveGPUMM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);
#endif // __SW_SOLVE_GPUMMH__
