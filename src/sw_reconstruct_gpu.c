#include <stdio.h>
#include <stdlib.h>

#include "sw_reconstruct_gpu.h"

#include "sw_solve_gpu.h"
#include "sw_reconstruct_gpu_sm.h"
#include "sw_reconstruct_gpu_mm.h"
#include "sw_hirschberg_data.h"
#include "sw_prefs.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"

extern SWHirschbergData* swReconstructGPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    int matcherType = matcherGetType(swPrefsGetMatcher(swPrefs));

    if (matcherType == MATCHER_MATRIX) {
        return swReconstructGPUSM(rowChain, columnChain, swPrefs); 
    } else {
        return swReconstructGPUMM(rowChain, columnChain, swPrefs); 
    }
}
