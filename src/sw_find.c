#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sw_find.h"

#include "sw_prefs.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"
#include "sw_nw_find_solve_cpu.h"
#include "sw_find_gpu.h"

static SWData* swFindSpecific(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, MatcherScore score);

extern SWData* swFind(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, MatcherScore score) {

    SWData* swData = swFindSpecific(rowChain, columnChain, swPrefs, score);

    return swData;
}

static SWData* swFindSpecific(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, MatcherScore score) {

    long rows = chainGetLength(rowChain);
    long columns = chainGetLength(columnChain);

    if (rows * columns < CPU_SOLVE_MAX_SIZE || swPrefsForceCPU(swPrefs)) {
        if (VERBOSE) {
            printf("CPU find\n");
        }
        return swFindCPU(rowChain, columnChain, swPrefs, score);
    } else {
        if (VERBOSE) {
            printf("GPU find\n");
        }
        return swFindGPU(rowChain, columnChain, swPrefs, score);
    }
}

