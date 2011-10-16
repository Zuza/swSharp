#ifndef __NW_SOLVE_GPUSMH__
#define __NW_SOLVE_GPUSMH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWData* nwSolveGPUSM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);
#endif // __NWSOLVE_GPUSMH__
