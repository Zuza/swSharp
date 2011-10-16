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

#include "sw_hirschberg_data.h"

#define SIZE 1000

typedef struct SWHirschbergBlock {
    int startRow;
    int startColumn;
    int endRow;
    int endColumn;
    int frontGap;
    int backGap;
} SWHirschbergBlock;

struct SWHirschbergData {
    SWHirschbergBlock* blocks;
    int blockNmr;
    int size;
    int sorted;
};

static void sort(SWHirschbergData* data);

extern SWHirschbergData* swHirschbergDataCreate() {

    SWHirschbergData* data = 
        (SWHirschbergData*) malloc(sizeof(struct SWHirschbergData));

    data->size = SIZE;
    data->blockNmr = 0;
    data->sorted = 0;
    data->blocks = 
        (SWHirschbergBlock*) malloc(data->size * sizeof(struct SWHirschbergBlock));

    return data;
}

extern void swHirschbergDataDelete(SWHirschbergData* data) {
    free(data->blocks);
    free(data);
}

extern void swHirschbergDataAddBlock(SWHirschbergData* data, int startRow, 
    int startColumn, int endRow, int endColumn, int frontGap, int backGap) {

    data->sorted = 0;

    if (data->blockNmr == data->size) {
        data->size += SIZE;

        int size = data->size * sizeof(struct SWHirschbergBlock);
        data->blocks = (SWHirschbergBlock*) realloc(data->blocks, size);
    }


    int index = data->blockNmr;

    data->blocks[index].startRow = startRow;
    data->blocks[index].startColumn = startColumn;
    data->blocks[index].endRow = endRow;
    data->blocks[index].endColumn = endColumn;
    data->blocks[index].frontGap = frontGap;
    data->blocks[index].backGap = backGap;

    data->blockNmr++;
}

extern int swHirschbergDataGetBlockNmr(SWHirschbergData* data) {
    return data->blockNmr;
}

extern int swHirschbergDataGetStartRow(SWHirschbergData* data, int index) {
    sort(data);
    return data->blocks[index].startRow;
}

extern int swHirschbergDataGetStartColumn(SWHirschbergData* data, int index) {
    sort(data);
    return data->blocks[index].startColumn;
}

extern int swHirschbergDataGetEndRow(SWHirschbergData* data, int index) {
    sort(data);
    return data->blocks[index].endRow;
}

extern int swHirschbergDataGetEndColumn(SWHirschbergData* data, int index) {
    sort(data);
    return data->blocks[index].endColumn;
}

extern int swHirschbergDataGetFrontGap(SWHirschbergData* data, int index) {
    sort(data);
    return data->blocks[index].frontGap;
}

extern int swHirschbergDataGetBackGap(SWHirschbergData* data, int index) {
    sort(data);
    return data->blocks[index].backGap;
}

static void swapBlocks(SWHirschbergBlock* first, SWHirschbergBlock* second) {
   SWHirschbergBlock holder;
   holder = *first;
   *first = *second;
   *second = holder;
}

static int compareBlocks(SWHirschbergBlock* first, SWHirschbergBlock* second) {

    if (first->startRow < second->startRow) {
        return -1;
    }

    if (first->startRow == second->startRow) {
        if (first->startColumn < second->startColumn) {
            return -1;
        }

        if (first->startColumn == second->startColumn) {
            return 0;
        }
    }

    return 1;
}

static void sortBlocks(SWHirschbergBlock list[], int start, int end) {

    SWHirschbergBlock key;
    int frontIdx;
    int backIdx;
    int pivot;

    if (start < end) {

        pivot = (start + end) / 2;
        swapBlocks(&list[start], &list[pivot]);
        key = list[start];

        frontIdx = start + 1;
        backIdx = end;

        while (frontIdx <= backIdx) {

            while (
                frontIdx <= end && 
                compareBlocks(&list[frontIdx], &key) <= 0
            ) {
                frontIdx++;
            }

            while (
                backIdx >= start && 
                compareBlocks(&list[backIdx], &key) > 0
            ) {
                backIdx--;
            }

            if (frontIdx < backIdx) {
                swapBlocks(&list[frontIdx], &list[backIdx]);
            }
        }

        swapBlocks(&list[start], &list[backIdx]);

        sortBlocks(list, start, backIdx - 1);
        sortBlocks(list, backIdx + 1, end);
    }
}


static void sort(SWHirschbergData* data) {
    if (!data->sorted) {
        data->sorted = 0;
        sortBlocks(data->blocks, 0, data->blockNmr - 1);
    }
}
