#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

extern "C" {
    #include "nw_solve_gpu_mm.h"

    #include "sw_prefs.h"
    #include "sw_data.h"
    #include "chain.h"
    #include "matcher.h"
    #include "utils.h"
}

#define ALPHA 5
#define GPU_CNST NW_CONF_SLV_MM

#include "templates/mm.template.h"
#include "templates/generic.h"

#include "templates/nw_solve.template"

extern SWData* nwSolveGPUMM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    SWData* swData = nwSolveGPU(rowChain, columnChain, swPrefs);

    return swData;
}
