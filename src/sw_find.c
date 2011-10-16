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

