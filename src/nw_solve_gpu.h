#ifndef __NW_SOLVE_GPUH__
#define __NW_SOLVE_GPUH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* nwSolveGPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);
#endif // __NW_SOLVE_GPUH__
