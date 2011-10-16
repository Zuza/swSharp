#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

extern "C" {
    #include "sw_solve_gpu_sm.h"

    #include "sw_prefs.h"
    #include "sw_data.h"
    #include "chain.h"
    #include "matcher.h"
    #include "utils.h"
}

#define ALPHA 5
#define GPU_CNST CONF_SLV_SM

#include "templates/sm.template.h"
#include "templates/generic.h"

#include "templates/sw_solve.template"

extern SWData* swSolveGPUSM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    SWData* swData = swSolveGPU(rowChain, columnChain, swPrefs);

    return swData;
}
