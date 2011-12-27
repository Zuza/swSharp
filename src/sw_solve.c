/*
SW# - CUDA parallelized Smith Waterman with applying Hirschberg's algorithm
Copyright (C) 2011 Matija Korpar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Contact the author by mkorpar@gmail.com.
*/

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

        int resultIdx;
        int resultNmr = swDataGetResultNmr(swData);

        for (resultIdx = 0; resultIdx < resultNmr; ++resultIdx) {
            swSetStartCell(swDataGetResult(swData, resultIdx), rowChain, 
                columnChain, swPrefs);
        }
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
