#include <stdio.h>
#include <stdlib.h>

#include "sw_solve.h"

#include "sw_find.h"
#include "sw_prefs.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"
#include "sw_nw_find_solve_cpu.h"
#include "sw_solve_gpu.h"

static SWData* swSolveSpecific(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);

static void swSetStartCell(SWResult* swResult, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs);

extern SWData* swSolve(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    SWData* swData = swSolveSpecific(rowChain, columnChain, swPrefs);

    if (!swPrefsGlobal(swPrefs) && !swPrefsSolveOnly(swPrefs)) {
        swSetStartCell(swDataGetResult(swData, 0), rowChain, columnChain, swPrefs);
    }

    return swData;
}

static SWData* swSolveSpecific(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    long rows = chainGetLength(rowChain);
    long columns = chainGetLength(columnChain);

    if (VERBOSE) {
        printf("SW solving:\n");
    }

    if ((rows * columns < CPU_SOLVE_MAX_SIZE) || swPrefsForceCPU(swPrefs)) {
        if (VERBOSE) {
            printf("CPU solve\n");
        }
        return swSolveCPU(rowChain, columnChain, swPrefs);
    } else {
        if (VERBOSE) {
            printf("GPU solve\n");
        }
        return swSolveGPU(rowChain, columnChain, swPrefs);
    }
}

static void swSetStartCell(SWResult* swResult, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs) {

    if (VERBOSE) {
        printf("Local to global:\n");
    }

    int row = swResultGetEndRow(swResult);
    int column = swResultGetEndColumn(swResult);
    MatcherScore score = swResultGetScore(swResult);

    Chain* subRowChain = reverseSubchain(rowChain, 0, row);
    Chain* subColumnChain = reverseSubchain(columnChain, 0, column);

    SWData* reverseData = swFind(subRowChain, subColumnChain, swPrefs, score);
    SWResult* reverse = swDataGetResult(reverseData, 0);

    row = row - swResultGetEndRow(reverse);
    column = column - swResultGetEndColumn(reverse);

    swResultSetStartRow(swResult, row);
    swResultSetStartColumn(swResult, column);

    chainDelete(subRowChain);
    chainDelete(subColumnChain);

    swDataDelete(reverseData);
}
