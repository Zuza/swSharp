#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

extern "C" {
    #include "sw_find_gpu_sm.h"

    #include "sw_prefs.h"
    #include "sw_data.h"
    #include "chain.h"
    #include "matcher.h"
    #include "utils.h"
}

#define ALPHA 5
#define SCORE CONF_FIND_SCORE_SM
#define GPU_CNST CONF_FIND_SM

#include "templates/sm.template.h"
#include "templates/generic.h"

#include "templates/sw_find.template"

extern SWData* swFindGPUSM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, MatcherScore score) {

    SWData* swData = swFindGPU(rowChain, columnChain, swPrefs, score);

    return swData;
}
