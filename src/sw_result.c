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
#include <math.h>

#include "sw_result.h"

#include "utils.h"
#include "sw_prefs.h"

#define LINE_LENGTH 50
#define DEFUALT_STRAND '+'
#define COMPLEMENT_STRAND '-'

struct SWResult {

    SWResultScore score;

    int startRow;
    int startColumn;
    int endRow;
    int endColumn;

    int length;
    char* row;
    char* column;

    int strand;
};

extern SWResult* swResultCreate(SWResultScore score, int row, int column) {

    SWResult* swResult = (SWResult*) malloc(sizeof(struct SWResult));

    swResult->score = score;

    swResult->startRow = 0;
    swResult->startColumn = 0;
    swResult->endRow = row;
    swResult->endColumn = column;

    swResult->length = 0;
    swResult->row = NULL;
    swResult->column = NULL;

    swResult->strand = DEFUALT_STRAND;

    return swResult;
}

extern void swResultDelete(SWResult* swResult) {
    free(swResult->row);
    free(swResult->column);
    free(swResult);
}

extern void swResultSetLength(SWResult* swResult, int length) {
    swResult->length = length;
}

extern void swResultSetReconstruction(SWResult* swResult, char* row, char* column) {
    swResult->row = row;
    swResult->column = column;
}

extern void swResultSetScore(SWResult* swResult, SWResultScore score) {
    swResult->score = score;
}

extern void swResultSetStartRow(SWResult* swResult, int row) {
    swResult->startRow = row;
}

extern void swResultSetStartColumn(SWResult* swResult, int column) {
    swResult->startColumn = column;
}

extern SWResultScore swResultGetScore(SWResult* swResult) {
    return swResult->score;
}

extern int swResultGetStartRow(SWResult* swResult) {
    return swResult->startRow;
}

extern int swResultGetStartColumn(SWResult* swResult) {
    return swResult->startColumn;
}

extern int swResultGetEndRow(SWResult* swResult) {
    return swResult->endRow;
}

extern int swResultGetEndColumn(SWResult* swResult) {
    return swResult->endColumn;
}

extern int swResultGetLength(SWResult* swResult) {
    return swResult->length;
}

extern void swResultSetComplementary(SWResult* swResult, int length) {

    swResult->strand = COMPLEMENT_STRAND;

    // replace start and end of the complement, complement is always the column
    // chain
    int holder = swResult->startColumn;
    swResult->startColumn = swResult->endColumn;
    swResult->endColumn = holder;

    // set inverse index
    swResult->startColumn = length - 1 - swResult->startColumn;
    swResult->endColumn = length - 1 - swResult->endColumn;
}

extern int swResultCompare(SWResult* a, SWResult* b) {

    // classic equality
    if (
        a->startRow == b->startRow && 
        a->startColumn == b->startColumn && 
        a->endRow == b->endRow && 
        a->endColumn == b->endColumn
    ) {
        return SW_RESULT_EQUAL;
    }

    return SW_RESULT_NOT_EQUAL;
}

extern void swResultPrint(SWResult* swResult) {

    if (TEST_MODE) return;

    printf(
        "Intervals: %d %d %d %d (%c) strand\n", 
        swResult->startRow + 1, 
        swResult->endRow + 1, 
        swResult->startColumn + 1,
        swResult->endColumn + 1,
        swResult->strand
    );

    printf("Score: %.3lf\n", (double) swResult->score);

    if (!swResult->row || !swResult->column) {
        return;
    }

    int idxUp = swResult->startRow;
    int idxUpNext = idxUp;
    int idxDown = swResult->startColumn;
    int idxDownNext = idxDown;

    int itemIdx;
    int line = 0;

    char* row = swResult->row;
    char* column = swResult->column;

    for (itemIdx = 0; itemIdx < swResult->length; ++itemIdx) {

        if (row[itemIdx] != GAP_ITEM) {
            ++idxUpNext;
        }

        if (column[itemIdx] != GAP_ITEM) {
            ++idxDownNext;
        }

        if (itemIdx % LINE_LENGTH == 0) {
            idxUp = idxUpNext;
            idxDown = idxDownNext;
        } 

        if (
            ((itemIdx + 1) % LINE_LENGTH == 0) || 
            (itemIdx == swResult->length - 1)
        ) {

            printf(
                "\n%13d  %-50.50s  %-10d\n%13d  %-50.50s  %-13d\n",
                    idxUp, 
                    row + (line * LINE_LENGTH), 
                    idxUpNext, 
                    idxDown, 
                    column + (line * LINE_LENGTH), 
                    idxDownNext
            );

            if (itemIdx == swResult->length) {
                break;
            }

            ++line;
        }
    }

    if (
        idxUpNext != (swResult->endRow + 1) || 
        idxDownNext != (swResult->endColumn + 1)
    ) {
        printf("err\n");
        exit(-1);
    }
}
