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
#include <time.h>

#include "sw.h"

#include "sw_solve.h"
#include "nw_solve.h"
#include "sw_reconstruct.h"
#include "sw_threshold.h"

#include "sw_prefs.h"
#include "sw_data.h"
#include "chain_base.h"
#include "chain.h"
#include "matcher.h"
#include "utils.h"

struct SW {
    Chain* query;
    ChainBase* database;
    SWPrefs* swPrefs;
};

static void statistics(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);

static SWData* smithWaterman(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs);

extern SW* swCreate(int argc, char* argv[]) {

    SW* sw = (SW*) malloc(sizeof(struct SW));

    sw->swPrefs = swPrefsCreate(argc, argv);

    if (!TEST_MODE) {
        printf("%s\n", SEPARATOR2);
    }

    sw->query = chainCreateFromFile(swPrefsGetQueryFile(sw->swPrefs), sw->swPrefs);
    sw->database = chainBaseCreate(swPrefsGetDatabaseFile(sw->swPrefs), sw->swPrefs);

    return sw;
}

extern void swDelete(SW* sw) {

    swPrefsDelete(sw->swPrefs);
    chainDelete(sw->query);
    chainBaseDelete(sw->database);

    free(sw);
}

extern void swRun(SW* sw) {

    if (!TEST_MODE) {
        printf("%s\n", SEPARATOR2);
        swPrefsPrint(sw->swPrefs);
    }

    int chainIdx;
    Chain* chain1;
    Chain* chain2;

    for (
        chainIdx = 0; 
        chainIdx < chainBaseGetLength(sw->database); 
        ++chainIdx
    ) {

        chain1 = sw->query;
        chain2 = chainBaseGetChain(sw->database, chainIdx);

        statistics(chain1, chain2, sw->swPrefs);
    }
}

static void statistics(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    if (!TEST_MODE) {
        printf("%s\n", SEPARATOR2);
    }

    clock_t start = clock();

    SWData* swData = smithWaterman(rowChain, columnChain, swPrefs);

    if (swPrefsComplement(swPrefs)) {

        Chain* complement = chainCreateComplement(columnChain, swPrefs);
        SWData* complementData = smithWaterman(rowChain, complement, swPrefs);

        int complementLen = chainGetLength(complement);

        int resultIdx;
        int resultNmr = swDataGetResultNmr(complementData);
        SWResult* swResult;

        for (resultIdx = 0; resultIdx < resultNmr; ++resultIdx) {
            swResult = swDataPopResult(complementData);

            swResultSetComplementary(swResult, complementLen);
            
            swDataAddResult(swData, swResult);
        }

        chainDelete(complement);
        swDataDelete(complementData);
    }

    clock_t finish = clock();
    swDataSetTime(swData, (double) (finish - start) / CLOCKS_PER_SEC);

    if (!TEST_MODE) {
        chainPrint(rowChain);
        chainPrint(columnChain);
        swDataPrint(swData);
    }

    swDataDelete(swData);
}

static SWData* smithWaterman(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    SWData* swData;
    MatcherScore threshold = swPrefsGetThreshold(swPrefs);

    if (!preciseEqual(threshold, NO_THRESHOLD)) {
        swData = swThreshold(rowChain, columnChain, swPrefs);
    } else {
        if (!swPrefsGlobal(swPrefs)) {
            swData = swSolve(rowChain, columnChain, swPrefs);
        } else {
            swData = nwSolve(rowChain, columnChain, swPrefs);
        }
    }

    if (!swPrefsSolveOnly(swPrefs)) {
        swReconstruct(swData, rowChain, columnChain, swPrefs);
    }

    return swData;
}
