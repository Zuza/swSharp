#ifndef __SW_RECONSTRUCT_GPUMMH__
#define __SW_RECONSTRUCT_GPUMMH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"
#include "sw_hirschberg_data.h"

extern SWHirschbergData* swReconstructGPUMM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);

#endif // __SW_RECONSTRUCT_GPUMMH__
