#ifndef __SW_SOLVE_GPUSMH__
#define __SW_SOLVE_GPUSMH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* swSolveGPUSM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);
#endif // __SW_SOLVE_GPUSMH__
