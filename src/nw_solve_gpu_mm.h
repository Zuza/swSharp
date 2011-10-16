#ifndef __NW_SOLVE_GPUMMH__
#define __NW_SOLVE_GPUMMH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* nwSolveGPUMM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);
#endif // __NW_SOLVE_GPUMMH__
