#ifndef __SW_RECONSTRUCT_CPUH__
#define __SW_RECONSTRUCT_CPUH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

extern SWResultMove* swReconstructCPU(SWResult* swResult, Chain* rowChain, 
    Chain* columnChain, SWPrefs* swPrefs, int frontGap, int backGap);

#endif // __SW_RECONSTRUCT_CPUH__
