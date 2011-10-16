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

#include "sw_nw_find_solve_cpu.h"

#include "sw_prefs.h"
#include "sw_data.h"
#include "chain.h"
#include "matcher.h"
#include "sw_result.h"
#include "utils.h"

#define SW 0
#define NW 1

#define IRELEVANT 0 
#define FIND 1
#define SOLVE 0

static SWData* solveCPU(Chain* rowChain, Chain* columnChain, SWPrefs* swPrefs, 
    int findMode, MatcherScore findScore, int type);

static MatcherScore biggerScore(MatcherScore first, MatcherScore second);

static MatcherScore getScoreSW(MatcherScore matchFactor, MatcherScore insertFactor,
    MatcherScore deleteFactor);

static MatcherScore getScoreNW(MatcherScore matchFactor, MatcherScore insertFactor,
    MatcherScore deleteFactor);

extern SWData* swSolveCPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {
    return solveCPU(rowChain, columnChain, swPrefs, SOLVE, IRELEVANT, SW);
}

extern SWData* nwSolveCPU(Chain* rowChain, Chain* columnChain, 
    SWPrefs* swPrefs) {
    return solveCPU(rowChain, columnChain, swPrefs, SOLVE, IRELEVANT, NW);
}

extern SWData* swFindCPU(Chain* rowChain, Chain* columnChain, SWPrefs* swPrefs,
    MatcherScore score) {
    return solveCPU(rowChain, columnChain, swPrefs, FIND, score, NW);
}

static SWData* solveCPU(Chain* rowChain, Chain* columnChain, SWPrefs* swPrefs, 
    int findMode, MatcherScore findScore, int type) {

    SWPrefsPenalty gapOpen = swPrefsGetGapOpen(swPrefs);
    SWPrefsPenalty gapExtend = swPrefsGetGapExtend(swPrefs);
    Matcher* matcher = swPrefsGetMatcher(swPrefs);

    // Lengths are increased by one because of extra solving row and column.
    int rows = chainGetLength(rowChain) + 1;
    int columns = chainGetLength(columnChain) + 1;

    SWData* swData = swDataCreate(rows - 1, columns - 1, swPrefs);

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

    MatcherScore score = 0;

    // Only relative columns of the score table are the last two so only two
    // rows are being stored.
    MatcherScore scores[2][columns];

    // Initialize result to minimal value.
    MatcherScore resultScore = MATCHER_SCORE_MIN; 
    int resultRow = 0; // Initialize max row to first row.
    int resultColumn = 0; // Initialize max column to first column.

    int row;
    int rowZip; // compressed row index, it alternates between 0 and 1

    SWResult* swResult;

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

            if (type == SW) {
                if (row == 0 || column == 0) {
                    scores[rowZip][column] = 0;
                    continue;
                }
            } else {
                if (row == 0 && column == 0) {
                    scores[rowZip][column] = 0;
                    continue;
                }

                if (row == 0) {
                    scores[rowZip][column] = -gapOpen - (column - 1) * gapExtend;
                    continue;
                }

                if (column == 0) {
                    scores[rowZip][column] = -gapOpen - (row - 1) * gapExtend;
                    continue;
                }
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
            // INSERT END

            // DELETE
            gapOpenFactor = scores[1 - rowZip][column] - gapOpen;
            gapExtendFactor = deleteFactorOld[column] - gapExtend;

            deleteFactor = biggerScore(gapOpenFactor, gapExtendFactor);
            // DELETE END

            if (type == SW) {
                score = getScoreSW(matchFactor, insertFactor, deleteFactor);
            } else {
                score = getScoreNW(matchFactor, insertFactor, deleteFactor);
            }

            if (findMode) {
                if (score == findScore) {
                    swResult = swResultCreate(score, row - 1, column - 1);
                    swDataAddResult(swData, swResult);
                    if (VERBOSE && row != rows - 1 && row * 100 / rows != 100) {
                        printPercentageBar(100);
                    }
                    return swData;
                }
            }

            scores[rowZip][column] = score;

            // New values become the old ones.
            deleteFactorOld[column] = deleteFactor;
            insertFactorOld = insertFactor;

            if (type == NW) {
                continue;
            }

            if (score > resultScore) {
                resultRow = row;
                resultColumn = column;
                resultScore = score;
            }
        }     
    }

    if (VERBOSE && row != rows - 1 && row * 100 / rows != 100) {
        printPercentageBar(100);
    }

    if (type == NW) {
        swResult = swResultCreate(score, rows - 2, columns - 2);
        swDataAddResult(swData, swResult);
        return swData;
    }

    swResult = swResultCreate(resultScore, resultRow - 1, resultColumn - 1);
    swDataAddResult(swData, swResult);

    return swData;
}

static MatcherScore biggerScore(MatcherScore first, MatcherScore second) {

    if (first > second) {
        return first;
    }

    return second;
}

static MatcherScore getScoreSW(MatcherScore matchFactor, MatcherScore insertFactor,
    MatcherScore deleteFactor) {

    if (
        (deleteFactor > insertFactor) && 
        (deleteFactor > matchFactor) && 
        (deleteFactor > 0)
    ) {
        return deleteFactor;
    }

    if (
        (insertFactor > matchFactor) && 
        (insertFactor > 0)
    ) {
        return insertFactor;
    }

    if (matchFactor > 0) {
        return matchFactor;
    }

    return 0;
}

static MatcherScore getScoreNW(MatcherScore matchFactor, MatcherScore insertFactor,
    MatcherScore deleteFactor) {

    if ((deleteFactor > insertFactor) && (deleteFactor > matchFactor)) {
        return deleteFactor;
    }

    if (insertFactor > matchFactor) {
        return insertFactor;
    }

    return matchFactor;
}
