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

#include "sw_reconstruct_cpu.h"

#include "sw_prefs.h"
#include "sw_result.h"
#include "chain.h"
#include "matcher.h"
#include "utils.h"

typedef struct MoveData {
    SWResultMove move;
    int verticalGaps;
    int horizontalGaps;
} MoveData;

static MoveData* getAllMoves(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, int frontGap);

static SWResultMove* getMoves(MoveData* allMoves, SWResult* swResult, int rows, 
    int columns, int backGap);

extern SWResultMove* swReconstructCPU(SWResult* swResult, Chain* rowChain, 
    Chain* columnChain, SWPrefs* swPrefs, int frontGap, int backGap) {

    MoveData* moves = getAllMoves(rowChain, columnChain, swPrefs, frontGap);

    int rows = chainGetLength(rowChain);
    int columns = chainGetLength(columnChain);

    return getMoves(moves, swResult, rows, columns, backGap);
}

static SWResultMove* getMoves(MoveData* allMoves, SWResult* swResult, int rows, 
    int columns, int backGap) {

    SWResultMove* moves = 
        (SWResultMove*) malloc((columns + rows) * sizeof(SWResultMove));
    int moveIdx = columns + rows;

    int row = rows;
    int column = columns;

    int allmoveIdx;
    SWResultMove move;

    int extendedGap;
    int firstMove = 1;

    do {

        allmoveIdx = row * (columns + 1) + column;
        move = allMoves[allmoveIdx].move;

        if (backGap && firstMove) { 
            move = SW_RESULT_MOVE_UP; 
            firstMove = 0; 
        }
 
        moves[--moveIdx] = move;

        switch (move) {
        case SW_RESULT_MOVE_DIAGONAL:
            row--;
            column--;
            break;
        case SW_RESULT_MOVE_LEFT:

			for (
                extendedGap = 0; 
                extendedGap < allMoves[allmoveIdx].horizontalGaps; 
                extendedGap++
            ) {
                moves[--moveIdx] = SW_RESULT_MOVE_LEFT;
			}

            column -= allMoves[allmoveIdx].horizontalGaps + 1;
            break;
        case SW_RESULT_MOVE_UP:

			for (
                extendedGap = 0; 
                extendedGap < allMoves[allmoveIdx].verticalGaps; 
                extendedGap++
            ) {
                moves[--moveIdx] = SW_RESULT_MOVE_UP;
			}

            row -= allMoves[allmoveIdx].verticalGaps + 1;
            break;
        }
    } while (move != SW_RESULT_MOVE_STOP);

    // restore the last index
    moveIdx++;

    int length = columns + rows - moveIdx; 

    int shiftIdx;
    for (shiftIdx = 0; shiftIdx < length; ++shiftIdx) {
        moves[shiftIdx] = moves[shiftIdx + moveIdx];
    }

    moves = (SWResultMove*) realloc(moves, length * sizeof(SWResultMove));

    swResultSetLength(swResult, length);

    free(allMoves);

    return moves;
}

static MatcherScore biggerScore(MatcherScore first, MatcherScore second) {

    if (first > second) {
        return first;
    }

    return second;
}

static MoveData* getAllMoves(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs, int frontGap) {

    // Lengths are increased by one because of extra solving row and column.
    int rows = chainGetLength(rowChain) + 1;
    int columns = chainGetLength(columnChain) + 1;

    MoveData* moves = (MoveData*) malloc(rows * columns * sizeof(MoveData));
    memset(moves, 0, rows * columns * sizeof(MoveData));

    SWPrefsPenalty gapOpen = swPrefsGetGapOpen(swPrefs);
    SWPrefsPenalty gapExtend = swPrefsGetGapExtend(swPrefs);
    Matcher* matcher = swPrefsGetMatcher(swPrefs);


    // Because of the left right passing before top down passing  only one
    // insertion factor needs to be stored and all of the deletion factors must
    // be stored.
    MatcherScore deleteFactorOld[columns]; // old top top value
    MatcherScore insertFactorOld; // old left left value

    int column;
    // Initialize old delete factors to worst case scenario, which is
    // the minimal score value;
    for (column = 0; column < columns; ++column) {
        deleteFactorOld[column] = MATCHER_SCORE_MIN;
    }

    ChainCode rowCode;
    ChainCode columnCode;

    MatcherScore insertFactor; // left
    MatcherScore deleteFactor; // up
    MatcherScore matchFactor; // diagonal

    MatcherScore gapOpenFactor;
    MatcherScore gapExtendFactor;
    MatcherScore similarityFactor;

    MatcherScore score;

    // Only relative columns of the score table are the last two so only two
    // rows are being stored.
    MatcherScore scores[2][columns];

    int row;
    int rowZip; // compressed row index, it alternates between 0 and 1
    int moveIdx;

    if (VERBOSE) {
        printPercentageBar(0);
    }

    for (row = 0, rowZip = 0; row < rows; ++row, rowZip = 1 - rowZip) {

        if (VERBOSE) {

            int percentNew = (row + 1) * 100 / rows;
            int percentOld = row * 100 / rows;

            if (percentNew > percentOld) {
                printPercentageBar(percentNew);
            }
        }

        // Initialize old insert factor to worst case scenario, which is
        // the minimal score value;
        insertFactorOld = MATCHER_SCORE_MIN;

        for (column = 0; column < columns; ++column) {

            moveIdx = row * columns + column;

            if (row == 0 && column == 0) {
                moves[moveIdx].move = SW_RESULT_MOVE_STOP;
                scores[rowZip][column] = 0;
                continue;
            }

            if (column == 0) {
                moves[moveIdx].move = SW_RESULT_MOVE_UP;
                scores[rowZip][column] = -gapOpen - gapExtend * (row - 1);
                continue;
            }

            if (row == 0) {
                moves[moveIdx].move = SW_RESULT_MOVE_LEFT;
                scores[rowZip][column] = -gapOpen - gapExtend * (column - 1);
                continue;
            }

            // MATCHING
            rowCode = chainGetCode(rowChain, row - 1);
            columnCode = chainGetCode(columnChain, column - 1);

            similarityFactor = matcherGetScore(matcher, rowCode, columnCode);

            matchFactor = scores[1 - rowZip][column - 1] + similarityFactor;
            // MATCHING END

            // INSERT
            gapOpenFactor = scores[rowZip][column - 1] - gapOpen;
            gapExtendFactor = insertFactorOld - gapExtend;

            insertFactor = biggerScore(gapOpenFactor, gapExtendFactor);

            if (insertFactor == gapExtendFactor) {
                moves[moveIdx].horizontalGaps = 
                    moves[moveIdx - 1].horizontalGaps + 1;
            }
            // INSERT END

            // DELETE
            gapOpenFactor = scores[1 - rowZip][column] - gapOpen;
            gapExtendFactor = deleteFactorOld[column] - gapExtend;

            deleteFactor = biggerScore(gapOpenFactor, gapExtendFactor);

            if (deleteFactor == gapExtendFactor) {
                moves[moveIdx].verticalGaps = 
                    moves[moveIdx - columns].verticalGaps + 1;
            }
            // DELETE END

            if (frontGap && row == 1)  {
                score = deleteFactor;
                moves[moveIdx].move = SW_RESULT_MOVE_UP;
            } else {

                score = MATCHER_SCORE_MIN;
                moves[moveIdx].move = SW_RESULT_MOVE_STOP;

                if (deleteFactor > score) {
                    score = deleteFactor;
                    moves[moveIdx].move = SW_RESULT_MOVE_UP;
                } 

                if (insertFactor > score) {
                    score = insertFactor;
                    moves[moveIdx].move = SW_RESULT_MOVE_LEFT;
                } 

                if (matchFactor > score) {
                    score = matchFactor;
                    moves[moveIdx].move = SW_RESULT_MOVE_DIAGONAL;
                }
            }

            scores[rowZip][column] = score;

            // New values become the old ones.
            deleteFactorOld[column] = deleteFactor;
            insertFactorOld = insertFactor;
        }
    }

    return moves;
}
