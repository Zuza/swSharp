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
#include <cuda.h>
#include <limits.h>

extern "C" {
    #include "sw_solve_gpu.h"
    #include "sw_reconstruct_gpu_mm.h"

    #include "sw_hirschberg_data.h"
    #include "sw_prefs.h"
    #include "sw_data.h"
    #include "chain.h"
    #include "matcher.h"
    #include "utils.h"
}

#define ALPHA 1
#define GPU_CNST CONF_RCN_MM

#include "templates/mm.template.h"
#include "templates/generic.h"

#include "templates/sw_reconstruct.template"

extern SWHirschbergData* swReconstructGPUMM(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    int rows = chainGetLength(rowChain);
    int columns = chainGetLength(columnChain);

    SWHirschbergData* data = swHirschbergDataCreate();
    HirshbergParent parent = {0, 0, 0, 0, rows - 1, columns - 1};

    hirschberg(data, rowChain, columnChain, swPrefs, parent);

    return data;
}
