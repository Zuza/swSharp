#include <stdio.h>
#include <stdlib.h>

#include "nw_solve_gpu.h"

#include "nw_solve_gpu_sm.h"
#include "nw_solve_gpu_mm.h"
#include "sw_prefs.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"

extern SWData* nwSolveGPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    int matcherType = matcherGetType(swPrefsGetMatcher(swPrefs));

    if (matcherType == MATCHER_MATRIX) {
        return nwSolveGPUSM(rowChain, columnChain, swPrefs); 
    } else {
        return nwSolveGPUMM(rowChain, columnChain, swPrefs); 
    }
}
