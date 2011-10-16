#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <limits.h>

extern "C" {
    #include "sw_solve_gpu.h"
    #include "sw_reconstruct_gpu_sm.h"

    #include "sw_hirschberg_data.h"
    #include "sw_prefs.h"
    #include "sw_data.h"
    #include "chain.h"
    #include "matcher.h"
    #include "utils.h"
}

#define ALPHA 1
#define GPU_CNST CONF_RCN_SM

#include "templates/sm.template.h"
#include "templates/generic.h"

#include "templates/sw_reconstruct.template"

extern SWHirschbergData* swReconstructGPUSM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    int rows = chainGetLength(rowChain);
    int columns = chainGetLength(columnChain);

    SWHirschbergData* data = swHirschbergDataCreate();
    HirshbergParent parent = {0, 0, 0, 0, rows - 1, columns - 1};

    hirschberg(data, rowChain, columnChain, swPrefs, parent);

    return data;
}
