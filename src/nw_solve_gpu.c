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

#include "nw_solve_gpu.h"

#include "nw_solve_gpu_sm.h"
#include "nw_solve_gpu_mm.h"
#include "sw_prefs.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"

extern SWData* nwSolveGPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    int matcherType = matcherGetType(swPrefsGetMatcher(swPrefs));

    if (matcherType == MATCHER_MATRIX) {
        return nwSolveGPUSM(rowChain, columnChain, swPrefs); 
    } else {
        return nwSolveGPUMM(rowChain, columnChain, swPrefs); 
    }
}
