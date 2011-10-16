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

#include "sw_data.h"

#include "sw_result.h"
#include "sw_prefs.h"

#define MALLOC_STEP 100

struct SWData {
    int rows;
    int columns;
    int resultNmr;
    int resultSize;
    SWResult** results;
    double time;
    int global;
};

static void addResult(SWData* swData, SWResult* result);
static void sortResults(SWResult* list[], int start, int end);

extern SWData* swDataCreate(int rows, int columns, SWPrefs* swPrefs) {

    SWData* swData = malloc(sizeof(struct SWData));

    swData->rows = rows;
    swData->columns = columns;
    
    swData->resultNmr = 0;
    swData->resultSize = 0;
    swData->results = NULL;

    swData->time = 0;
    
    swData->global = swPrefsGlobal(swPrefs);

    return swData;
}

extern void swDataDelete(SWData* swData) {

    int resultIdx;  
    for (resultIdx = 0; resultIdx < swData->resultNmr; ++resultIdx) {
        swResultDelete(swData->results[resultIdx]);
    }

    free(swData->results);
    free(swData);
}

extern void swDataAddResult(SWData* swData, SWResult* result) {
    addResult(swData, result);
}

extern SWResult* swDataGetResult(SWData* swData, int index) {
    return swData->results[index];
}

extern int swDataGetRows(SWData* swData) {
    return swData->rows;
}

extern int swDataGetColumns(SWData* swData) {
    return swData->columns;
}

extern int swDataGetResultNmr(SWData* swData) {
    return swData->resultNmr;
}

extern void swDataSetTime(SWData* swData, double time) {
    swData->time = time;
}

extern SWResult* swDataPopResult(SWData* swData) {

    SWResult* swResult = swData->results[0];

    int resultIdx;
    for (resultIdx = 0; resultIdx < swData->resultNmr - 1; ++resultIdx) {
        swData->results[resultIdx] = swData->results[resultIdx + 1];
    }

    swData->resultNmr--;

    return swResult;
}

extern void swDataPrint(SWData* swData) {

    if (TEST_MODE) return;

    double rows = swData->rows;
    double columns = swData->columns;
    double gcups = columns * rows / swData->time * 10e-10;
    printf(
        "Size: %d * %d | Time: %lf | GCuPS: %lf\n", 
        swData->rows, swData->columns, swData->time, gcups
    );
    
    sortResults(swData->results, 0, swData->resultNmr - 1);

    int resultIdx;
    for (resultIdx = swData->resultNmr - 1; resultIdx >= 0; --resultIdx) {
        printf("\n");
        swResultPrint(swData->results[resultIdx]);
    }
}

static void addResult(SWData* swData, SWResult* result) {

    int resultIdx;

    if (!swData->global) {
        for (resultIdx = 0; resultIdx < swData->resultNmr; ++resultIdx) {

            int cmp = swResultCompare(swData->results[resultIdx], result);

            if (cmp == SW_RESULT_EQUAL) {
                return;
            }
        }
    }

    if (swData->resultNmr == swData->resultSize) {
        swData->resultSize += MALLOC_STEP;
        swData->results = 
            realloc(swData->results, swData->resultSize * sizeof(SWResult*));
    }

    swData->results[swData->resultNmr] = result;
    swData->resultNmr++;
}

static void swapResults(SWResult** first,SWResult** second) {
   SWResult* holder;
   holder = *first;
   *first = *second;
   *second = holder;
}

static void sortResults(SWResult* list[], int start, int end) {

    MatcherScore key;
    int frontIdx;
    int backIdx;
    int pivot;

    if (start < end) {

        pivot = (start + end) / 2;
        swapResults(&list[start], &list[pivot]);
        key = swResultGetScore(list[start]);

        frontIdx = start + 1;
        backIdx = end;

        while (frontIdx <= backIdx) {

            while ((frontIdx <= end) && (swResultGetScore(list[frontIdx]) <= key)) {
                frontIdx++;
            }

            while ((backIdx >= start) && (swResultGetScore(list[backIdx]) > key)) {
                backIdx--;
            }

            if (frontIdx < backIdx) {
                swapResults(&list[frontIdx], &list[backIdx]);
            }
        }

        swapResults(&list[start], &list[backIdx]);

        sortResults(list, start, backIdx - 1);
        sortResults(list, backIdx + 1, end);
    }
}
