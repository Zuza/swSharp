#include <stdio.h>
#include <stdlib.h>

#include "sw_solve_gpu.h"

#include "sw_solve_gpu_sm.h"
#include "sw_solve_gpu_mm.h"
#include "sw_prefs.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"

extern SWData* swSolveGPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    int matcherType = matcherGetType(swPrefsGetMatcher(swPrefs));

    if (matcherType == MATCHER_MATRIX) {
        return swSolveGPUSM(rowChain, columnChain, swPrefs); 
    } else {
        return swSolveGPUMM(rowChain, columnChain, swPrefs); 
    }
}
