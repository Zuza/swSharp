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

#include "nw_solve.h"

#include "sw_prefs.h"
#include "sw_result.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"
#include "sw_nw_find_solve_cpu.h"
#include "nw_solve_gpu.h"

static SWData* nwSolveSpecific(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);

extern SWData* nwSolve(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    SWData* swData = nwSolveSpecific(rowChain, columnChain, swPrefs);

    return swData;
}

static SWData* nwSolveSpecific(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    if (VERBOSE) {
        printf("NW solving:\n");
    }

    long rows = chainGetLength(rowChain);
    long columns = chainGetLength(columnChain);

    if (!swPrefsSolveOnly(swPrefs)) {

        SWData* swData = swDataCreate(rows, columns, swPrefs);

        // reconstroction will set the results
        swDataAddResult(swData, swResultCreate(0, rows - 1, columns - 1));

        return swData;
    }

    if (rows * columns < CPU_SOLVE_MAX_SIZE || swPrefsForceCPU(swPrefs)) {
        if (VERBOSE) {
            printf("CPU solve\n");
        }
        return nwSolveCPU(rowChain, columnChain, swPrefs);
    } else {
        if (VERBOSE) {
            printf("GPU solve\n");
        }
        return nwSolveGPU(rowChain, columnChain, swPrefs);
    }
}
