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
#include <math.h>

#include "sw_threshold.h"

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"
#include "sw_solve.h"
#include "sw_result.h"

#define MALLOC_STEP 100

typedef struct Interval {
    int startRow;
    int startColumn;
    int endRow;
    int endColumn;
} Interval;

typedef struct Intervals {
    Interval** data;
    int length;
    int size;
} Intervals;

static void thresholdSolveStep(SWData* swData, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs, Intervals* intervals, Interval* parent);

static Interval* createInterval(int startRow, int startColumn, int endRow, int endColumn);

static void addInterval(Intervals* intervals, Interval* interval);

static int inIntervals(Intervals* intervals, Interval* interval);

static void deleteIntervals(Intervals* intervals);

extern SWData* swThreshold(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {

    int rows = chainGetLength(rowChain);
    int columns = chainGetLength(columnChain);

    if (VERBOSE) {
        printf("SW threshold solving\n");
    }

    SWData* swData = swDataCreate(rows, columns, swPrefs);

    Intervals intervals = { NULL, 0, 0 };
    Interval* start = createInterval(0, 0, rows - 1, columns - 1);
 
    thresholdSolveStep(swData, rowChain, columnChain, swPrefs, &intervals, start);

    deleteIntervals(&intervals);

    return swData;
}

static void thresholdSolveStep(SWData* swData, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs, Intervals* intervals, Interval* parent) {

    if (
        inIntervals(intervals, parent) || 
        abs(parent->endRow - parent->startRow) <= 1 ||
        abs(parent->endColumn - parent->startColumn)  <= 1
    ) {
        free(parent);
        return;
    }

    addInterval(intervals, parent);

    MatcherScore threshold = swPrefsGetThreshold(swPrefs);

    Chain* subRowChain = subchain(rowChain, parent->startRow, parent->endRow);
    Chain* subColumnChain = subchain(columnChain, parent->startColumn, parent->endColumn);
  
    SWData* subData = swSolve(subRowChain, subColumnChain, swPrefs);
    SWResult* subResult = swDataGetResult(subData, 0);

    MatcherScore score = swResultGetScore(subResult);

    if (score >= threshold) {

        int startRow = parent->startRow + swResultGetStartRow(subResult);
        int startColumn = parent->startColumn + swResultGetStartColumn(subResult);
        int endRow = parent->startRow + swResultGetEndRow(subResult);
        int endColumn = parent->startColumn + swResultGetEndColumn(subResult);

        SWResult* swResult = swResultCreate(score, endRow, endColumn);
        swResultSetStartRow(swResult, startRow);
        swResultSetStartColumn(swResult, startColumn);

        swDataAddResult(swData, swResult);

        Interval* up = createInterval( 
            parent->startRow, 
            parent->startColumn, 
            parent->startRow + swResultGetStartRow(subResult) - 1, 
            parent->endColumn
        );

        thresholdSolveStep(swData, rowChain, columnChain, swPrefs, intervals, up);

        Interval* down = createInterval(  
            parent->startRow + swResultGetEndRow(subResult) + 1, 
            parent->startColumn, 
            parent->endRow, 
            parent->endColumn 
        );

        thresholdSolveStep(swData, rowChain, columnChain, swPrefs, intervals, down);

        /*
        Interval* left = createInterval( 
            parent->startRow, 
            parent->startColumn, 
            parent->endRow, 
            parent->startColumn + swResultGetStartColumn(subResult) - 1
        );

        thresholdSolveStep(swData, rowChain, columnChain, swPrefs, intervals, left);

        Interval* right = createInterval(  
            parent->startRow, 
            parent->startColumn + swResultGetEndColumn(subResult) + 1, 
            parent->endRow, 
            parent->endColumn 
        );

        thresholdSolveStep(swData, rowChain, columnChain, swPrefs, intervals, right);
        */
    }

    chainDelete(subRowChain);
    chainDelete(subColumnChain);

    swDataDelete(subData);
}

static Interval* createInterval(int startRow, int startColumn, int endRow, 
    int endColumn) {

    Interval* interval = (Interval*) malloc(sizeof(Interval));
    
    interval->startRow = startRow;
    interval->startColumn = startColumn;
    interval->endRow = endRow;
    interval->endColumn = endColumn;

    return interval;
}

static void addInterval(Intervals* intervals, Interval* interval) {

    if (intervals->length == intervals->size) {
        intervals->size += MALLOC_STEP;
        intervals->data = 
            realloc(intervals->data, intervals->size * sizeof(Interval*));
    }

    intervals->data[intervals->length] = interval;
    intervals->length++;
}

static int inIntervals(Intervals* intervals, Interval* interval) {

    int idx;
    for (idx = 0; idx < intervals->length; ++idx) {

        if (
            intervals->data[idx]->startRow == interval->startRow &&
            intervals->data[idx]->startColumn == interval->startColumn &&
            intervals->data[idx]->endRow == interval->endRow &&
            intervals->data[idx]->endColumn == interval->endColumn
        ) {
            return 1;
        }
    }

    return 0;
}

static void deleteIntervals(Intervals* intervals) {

    int idx;

    for (idx = 0; idx < intervals->length; ++idx) {
        free(intervals->data[idx]);
    }

    free(intervals->data);
}
