#include <stdio.h>
#include <stdlib.h>

#include "sw_find_gpu.h"

#include "sw_find_gpu_sm.h"
#include "sw_find_gpu_mm.h"
#include "sw_prefs.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"

extern SWData* swFindGPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, MatcherScore score) {

    int matcherType = matcherGetType(swPrefsGetMatcher(swPrefs));

    if (matcherType == MATCHER_MATRIX) {
        return swFindGPUSM(rowChain, columnChain, swPrefs, score); 
    } else {
        return swFindGPUMM(rowChain, columnChain, swPrefs, score); 
    }
}
