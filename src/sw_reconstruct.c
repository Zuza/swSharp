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

#include "sw_reconstruct.h"

#include "chain.h"
#include "sw_data.h"
#include "sw_result.h"
#include "sw_prefs.h"
#include "sw_reconstruct_cpu.h"
#include "sw_reconstruct_gpu.h"
#include "sw_hirschberg_data.h"
#include "utils.h"

#define MOVE_ERASE_MASK 10
#define MOVE_START_ERASE 4
#define MOVE_STOP_ERASE 7
#define MOVE_NONE -1

static SWResultMove* hybridReconstruct(SWResult* swResult, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs);

static void reconstruct(SWData* swData, SWResult* swResult, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs);

static void windowMoves(SWResultMove* moves, int length, SWPrefs* swPrefs);

extern void swReconstruct(SWData* swData, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs) {

    int resultIdx;
    int resultNmr = swDataGetResultNmr(swData);
    SWResult* swResult;

    for (resultIdx = 0; resultIdx < resultNmr; ++resultIdx) {
        swResult = swDataPopResult(swData);
        reconstruct(swData, swResult, rowChain, columnChain, swPrefs);
        swResultDelete(swResult);
    }
}

static void reconstruct(SWData* swData, SWResult* swResult, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs) {

    if (VERBOSE) {
        printf("Reconstructing:\n");
    }

    int startRow = swResultGetStartRow(swResult);
    int startColumn = swResultGetStartColumn(swResult);
    int endRow = swResultGetEndRow(swResult);
    int endColumn = swResultGetEndColumn(swResult);

    Chain* rowSubchain = subchain(rowChain, startRow, endRow);
    Chain* columnSubchain = subchain(columnChain, startColumn, endColumn);

    long aproximateSize = (endRow - startRow) + (endColumn - startColumn) / 2.;
    SWResultMove* moves;

    if (aproximateSize < CPU_RECONSTRUCT_MAX_SIZE || swPrefsForceCPU(swPrefs)) {
        if (VERBOSE) {
            printf("CPU reconstruction\n");
        }
        moves = swReconstructCPU(swResult, rowSubchain, columnSubchain, swPrefs, 0, 0);
    } else {
        if (VERBOSE) {
            printf("Hybrid reconstruction\n");
        }
        moves = hybridReconstruct(swResult, rowSubchain, columnSubchain, swPrefs);
    }

    chainDelete(rowSubchain);
    chainDelete(columnSubchain);

    int length = swResultGetLength(swResult);
    int moveIdx;
    SWResultMove move;

    int rowChainIdx = startRow;
    int columnChainIdx = startColumn;

    char* up = malloc((length + 1) * sizeof(char)); 
    char* bottom = malloc((length + 1) * sizeof(char));

    Matcher* matcher = swPrefsGetMatcher(swPrefs);

    MatcherScore matchScr;
    MatcherScore preciseScore = 0;
    MatcherScore gapOpen = swPrefsGetGapOpen(swPrefs);
    MatcherScore gapExtend = swPrefsGetGapExtend(swPrefs);

    int holeUp = 0; // is currently hole in up chain, boolean
    int holeLeft = 0; // is currently hole in down chain, boolean

    windowMoves(moves, length, swPrefs);

    int rcnIdx = 0;
    int erasing = 0;

    int startRowLocal = startRow;
    int startColumnLocal = startColumn;
    int moveIdxLocal = 0;

    char* rowReconstruction;
    char* columnReconstruction;

    for (moveIdx = 0; moveIdx < length; ++moveIdx) {

        move = moves[moveIdx];

        if (move / MOVE_ERASE_MASK == MOVE_STOP_ERASE) {

            startRowLocal = rowChainIdx;
            startColumnLocal = columnChainIdx;
            moveIdxLocal = moveIdx;

            erasing = 0;
            rcnIdx = 0;

            holeLeft = 0;
            holeUp = 0;
            preciseScore = 0;
        }

        switch (move % MOVE_ERASE_MASK) {
        case SW_RESULT_MOVE_DIAGONAL:

            if (!erasing) {
                up[rcnIdx]= chainGetItem(rowChain, rowChainIdx);
                bottom[rcnIdx] = chainGetItem(columnChain, columnChainIdx);

                matchScr = matcherGetScore(
                    matcher, 
                    chainGetCode(rowChain, rowChainIdx), 
                    chainGetCode(columnChain, columnChainIdx)
                );

                preciseScore += matchScr;
                holeUp = 0;
                holeLeft = 0;
            }

            rowChainIdx++;
            columnChainIdx++;

            break;
        case SW_RESULT_MOVE_LEFT:

            if (!erasing) {
                preciseScore -= holeLeft ? gapExtend : gapOpen;
                holeLeft = 1;
                holeUp = 0;

                up[rcnIdx]= GAP_ITEM;
                bottom[rcnIdx] = chainGetItem(columnChain, columnChainIdx);
            }        

            columnChainIdx++;

            break;
        case SW_RESULT_MOVE_UP:

            if (!erasing) {
                preciseScore -= holeUp ? gapExtend : gapOpen;
                holeLeft = 0;
                holeUp = 1;

                up[rcnIdx]= chainGetItem(rowChain, rowChainIdx);
                bottom[rcnIdx] = GAP_ITEM;
            }

            rowChainIdx++;

            break;
        }

        if (
            (move / MOVE_ERASE_MASK == MOVE_START_ERASE) || 
            (!erasing && moveIdx == length - 1)
        ) {

            if (moveIdx - moveIdxLocal > 0) {
                rowReconstruction = malloc((moveIdx - moveIdxLocal + 2) * sizeof(char));
                columnReconstruction = malloc((moveIdx - moveIdxLocal + 2) * sizeof(char));

                strncpy(rowReconstruction, up, moveIdx - moveIdxLocal + 1);
                strncpy(columnReconstruction, bottom, moveIdx - moveIdxLocal + 1);

                rowReconstruction[moveIdx - moveIdxLocal + 1] = '\0';
                columnReconstruction[moveIdx - moveIdxLocal + 1] = '\0';

                swResult = swResultCreate(preciseScore, rowChainIdx - 1, columnChainIdx - 1);
                swResultSetStartRow(swResult, startRowLocal);
                swResultSetStartColumn(swResult, startColumnLocal);
                swResultSetLength(swResult, moveIdx - moveIdxLocal + 1);
                swResultSetReconstruction(swResult, rowReconstruction, columnReconstruction);
                
                swDataAddResult(swData, swResult);
            }

            erasing = 1;
        }

        rcnIdx++;   
    }

    up[length]= '\0';
    bottom[length]= '\0';

    free(moves);
    free(up);
    free(bottom);
}

static SWResultMove* hybridReconstruct(SWResult* swResult, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs) {

    int rows = chainGetLength(rowChain);
    int columns = chainGetLength(columnChain);

    SWHirschbergData* data = swReconstructGPU(rowChain, columnChain, swPrefs);
    SWResultMove* moves = 
        (SWResultMove*) malloc((columns + rows) * sizeof(SWResultMove));

    int length;
    int moveIdx = 0;
    int moveIdxLocal;
    int blockIdx;

    Chain* rowSubchain;
    Chain* columnSubchain;
    SWResult* swResultLocal;

    int rowsLocal;
    int columnsLocal;

    int startRow;
    int endRow;
    int startColumn;
    int endColumn;
    int frontGap; 
    int backGap;

    int verbose = VERBOSE;
    VERBOSE = 0;
    
    SWResultMove* movesLocal;

    for (blockIdx = 0; blockIdx < swHirschbergDataGetBlockNmr(data); ++blockIdx) { 

        startRow = swHirschbergDataGetStartRow(data, blockIdx);
        endRow = swHirschbergDataGetEndRow(data, blockIdx);
        startColumn = swHirschbergDataGetStartColumn(data, blockIdx);
        endColumn = swHirschbergDataGetEndColumn(data, blockIdx);
        frontGap = swHirschbergDataGetFrontGap(data, blockIdx);
        backGap = swHirschbergDataGetBackGap(data, blockIdx);

        rowSubchain = subchain(rowChain, startRow, endRow);
        columnSubchain = subchain(columnChain, startColumn, endColumn);

        rowsLocal = chainGetLength(rowSubchain);
        columnsLocal = chainGetLength(columnSubchain);
        // data here is irelevant, not sw, but nw score
        swResultLocal = swResultCreate(0, 0, 0);

        movesLocal = swReconstructCPU(swResultLocal, rowSubchain, columnSubchain,
            swPrefs, frontGap, backGap);

        length = swResultGetLength(swResultLocal);

        for (
            moveIdxLocal = 0; 
            moveIdxLocal < length; 
            ++moveIdxLocal, ++moveIdx
        ) {
            moves[moveIdx] = movesLocal[moveIdxLocal];
        }
   
        // clean your memory
        free(movesLocal);
        swResultDelete(swResultLocal);
        chainDelete(rowSubchain);
        chainDelete(columnSubchain);
    }

    VERBOSE = verbose;

    length = moveIdx;

    moves = (SWResultMove*) realloc(moves, length * sizeof(SWResultMove));

    swResultSetLength(swResult, length);

    swHirschbergDataDelete(data);

    return moves;
}

static void windowMoves(SWResultMove* moves, int length, SWPrefs* swPrefs) {

    SWResultMove move;
    int moveIdx;

    int maximumGaps = swPrefsGetMaximumGaps(swPrefs);
    int windowSize = swPrefsGetWindowSize(swPrefs);

    if (windowSize == 0) {
        return;
    }

    int currentHoles = 0;
    int* valid = malloc(length * sizeof(int));
    memset(valid, 0, length * sizeof(int));

    for (moveIdx = 0; moveIdx < length; ++moveIdx) {

        move = moves[moveIdx];

        if (moveIdx >= windowSize) {
            if (moves[moveIdx - windowSize] != SW_RESULT_MOVE_DIAGONAL) {
                currentHoles--;
            }
        }

        if (moves[moveIdx] != SW_RESULT_MOVE_DIAGONAL) {
            currentHoles++;
        }

        if (moveIdx >= (windowSize - 1) && currentHoles <= windowSize - maximumGaps) {
            valid[moveIdx - (windowSize - 1)] = 1;
        }
    }

    int ignore = 0;
    int idx;

    for (moveIdx = 0; moveIdx < length; ++moveIdx) {

        if (!valid[moveIdx] && ignore == 0) {

            idx = moveIdx;

            while (moves[idx] != SW_RESULT_MOVE_DIAGONAL && idx > 0) {
                idx--;
            }

            moves[idx] += MOVE_START_ERASE * MOVE_ERASE_MASK;
        }

        if (valid[moveIdx]) {

            if (ignore < 0) {

                while (moves[moveIdx] != SW_RESULT_MOVE_DIAGONAL && moveIdx < length - 1) {
                    moveIdx++;
                }

                moves[moveIdx] += MOVE_STOP_ERASE * MOVE_ERASE_MASK;
            }

            ignore = windowSize;
        }

        ignore--; 
    }

    int tgl = MOVE_NONE;

    for (moveIdx = 0; moveIdx < length; ++moveIdx) {

        if(moves[moveIdx] / MOVE_ERASE_MASK == MOVE_START_ERASE) { 

            if (tgl == MOVE_START_ERASE) {  
                printf("error in windowing\n"); 
                exit(-1); 
            } 

            tgl = MOVE_START_ERASE; 
        }

        if(moves[moveIdx] / MOVE_ERASE_MASK == MOVE_STOP_ERASE) { 

            if (tgl == MOVE_STOP_ERASE) { 
                printf("error in windowing\n"); 
                exit(-1); 
            }  

            tgl = MOVE_STOP_ERASE; 
        }
    }

    free(valid);
}
