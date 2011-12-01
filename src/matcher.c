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

#include "matcher.h"

#include "utils.h"
#include "sw_prefs.h"

#include "matrices/DNA.h"
#include "matrices/BLOSUM50.h"
#include "matrices/BLOSUM55.h"
#include "matrices/BLOSUM62.h"
#include "matrices/BLOSUM70.h"
#include "matrices/BLAST.h"
#include "matrices/CHIAR.h"
#include "matrices/SHOTGUN.h"

#define WILDCARD '*'

struct Matcher {
    int itemNumber;
    MatcherItem* items;
    MatcherScore* scores;
    int dummyCode;
    int embedded;
    char* typeName;
    int type;
    MatcherScore match;
    MatcherScore mismatch;
};

static void addDummyCodes(Matcher* matcher, MatcherScore* scores);

static void inputReadMatcher(Matcher* matcher, char* filePath);

extern Matcher* matcherCreateFromFile(char* filePath) {

    Matcher* matcher = (Matcher*) malloc(sizeof(struct Matcher));

    inputReadMatcher(matcher, filePath);

    matcher->typeName = "from file";
    matcher->type = MATCHER_MATRIX;
    matcher->embedded = 0;

    return matcher;
}

static void inputReadMatcher(Matcher* matcher, char* filePath) {

    FILE* file = fileOpen(filePath);

    int itemsRead = fscanf(file, "%d\n", &matcher->itemNumber);

    if (itemsRead != 1) {
        fileReadError(filePath);
    }

    matcher->items = 
        (MatcherItem*) malloc(matcher->itemNumber * sizeof(MatcherItem));

    int itemsIdx;
    for (itemsIdx = 0; itemsIdx < matcher->itemNumber; ++itemsIdx) {

        itemsRead = fscanf(file, "%c ", &matcher->items[itemsIdx]);

        if (itemsRead != 1) {
            fileReadError(filePath);
        }
    }

    int matcherScoreNmr = matcher->itemNumber * matcher->itemNumber;
    MatcherScore* matcherScores = 
        (MatcherScore*) malloc(matcherScoreNmr * sizeof(MatcherScore));

    double matcher_score;

    for (itemsIdx = 0; itemsIdx < matcherScoreNmr; ++itemsIdx) {

        itemsRead = fscanf(file, "%lf", &matcher_score);

        if (itemsRead != 1) {
            fileReadError(filePath);
        }

        matcherScores[itemsIdx] = (MatcherScore) matcher_score;
    }

    fclose(file);

    addDummyCodes(matcher, matcherScores);
}

extern Matcher* matcherCreateEmbedded(char* typeName) {

    int itemNumber;
    MatcherItem* items;
    MatcherScore* scores;

    if (strcmp("DNA", typeName) == 0) {
        itemNumber = DNA_ITEM_NMR;
        items = DNA_ITEMS;
        scores = DNA_SCORES;
    } else if (strcmp("BLOSUM50", typeName) == 0) {
        itemNumber = BLOSUM50_ITEM_NMR;
        items = BLOSUM50_ITEMS;
        scores = BLOSUM50_SCORES;
    } else if (strcmp("BLOSUM55", typeName) == 0) {
        itemNumber = BLOSUM55_ITEM_NMR;
        items = BLOSUM55_ITEMS;
        scores = BLOSUM55_SCORES;
    } else if (strcmp("BLOSUM62", typeName) == 0) {
        itemNumber = BLOSUM62_ITEM_NMR;
        items = BLOSUM62_ITEMS;
        scores = BLOSUM62_SCORES;
    } else if (strcmp("BLOSUM70", typeName) == 0) {
        itemNumber = BLOSUM70_ITEM_NMR;
        items = BLOSUM70_ITEMS;
        scores = BLOSUM70_SCORES;
    } else if (strcmp("BLAST", typeName) == 0) {
        itemNumber = BLAST_ITEM_NMR;
        items = BLAST_ITEMS;
        scores = BLAST_SCORES;
    } else if (strcmp("CHIAR", typeName) == 0) {
        itemNumber = CHIAR_ITEM_NMR;
        items = CHIAR_ITEMS;
        scores = CHIAR_SCORES;
    } else if (strcmp("SHOTGUN", typeName) == 0) {
        itemNumber = SHOTGUN_ITEM_NMR;
        items = SHOTGUN_ITEMS;
        scores = SHOTGUN_SCORES;
    } else {
        printf("Warning: Similarity table %s not embedded. Using %s.\n",
            typeName, QUOTE(DEFUALT_MATCHER));
        itemNumber = CONCATE(DEFUALT_MATCHER, ITEM_NMR);
        items = CONCATE(DEFUALT_MATCHER, ITEMS);
        scores = CONCATE(DEFUALT_MATCHER, SCORES);
        typeName = QUOTE(DEFUALT_MATCHER);
    }

    Matcher* matcher = (Matcher*) malloc(sizeof(struct Matcher));

    matcher->items = items;
    matcher->itemNumber = itemNumber;
    addDummyCodes(matcher, scores);

    matcher->typeName = typeName;
    matcher->type = MATCHER_MATRIX;
    matcher->embedded = 1;

    return matcher;
}

extern Matcher* matcherCreateMM(MatcherScore match, MatcherScore mismatch) {

    Matcher* matcher = (Matcher*) malloc(sizeof(struct Matcher));

    matcher->typeName = "match/mismatch";
    matcher->type = MATCHER_MATCH_MISMATCH;
    matcher->embedded = 0;

    matcher->match = match;
    matcher->mismatch = mismatch;

    matcher->items = NULL;
    matcher->scores = NULL;
    matcher->itemNumber = 0;
    matcher->dummyCode = -1;

    return matcher;
}

static void addDummyCodes(Matcher* matcher, MatcherScore* scores) {

    // dummy code is the index of the last row/column
    matcher->dummyCode = matcher->itemNumber;

    // increase item number for worst, dummy element
    matcher->itemNumber++;

    matcher->scores = (MatcherScore*) 
        malloc(matcher->itemNumber * matcher->itemNumber * sizeof(MatcherScore));

    int scoreRow;
    int scoreColumn;
    int newScoreIdx;
    int oldScoreIdx;

    for (scoreRow = 0; scoreRow < matcher->itemNumber; ++scoreRow) {
        for (scoreColumn = 0; scoreColumn < matcher->itemNumber; ++scoreColumn) {

            newScoreIdx = scoreRow * matcher->itemNumber + scoreColumn;
            oldScoreIdx = scoreRow * (matcher->itemNumber - 1) + scoreColumn;

            if (
                scoreRow == matcher->dummyCode || 
                scoreColumn == matcher->dummyCode
            ) {
                matcher->scores[newScoreIdx] = MATCHER_SCORE_MIN;
            } else {
                matcher->scores[newScoreIdx] = scores[oldScoreIdx];
            }
        }
    }
}

extern void matcherDelete(Matcher* matcher) {

    if (!matcher->embedded) {
        free(matcher->items);
    }

    free(matcher->scores);
    free(matcher);
}

extern int matcherGetDummyCode(Matcher* matcher) {

    if (matcher->type == MATCHER_MATCH_MISMATCH) {
        matcher->dummyCode++;
    }

    return matcher->dummyCode;
}

extern int matcherGetItemNmr(Matcher* matcher) {
    return matcher->itemNumber;
}

extern void matcherGetScores(Matcher* matcher, MatcherScore* scores) {

    if (matcher->type != MATCHER_MATRIX) {
        printf("Trying to use non existing similarity matrix.\n");
        exit(-1);
    }

    int scoreIdx;
    int scoreNmr = matcher->itemNumber * matcher->itemNumber;

    for (scoreIdx = 0; scoreIdx < scoreNmr; ++scoreIdx) {
        scores[scoreIdx] = matcher->scores[scoreIdx];
    }
}

extern MatcherScore matcherGetScore(Matcher* matcher, MatcherCode firstItem,
        MatcherCode secondItem) {

    if (matcher->type == MATCHER_MATRIX) {
        return matcher->scores[firstItem * matcher->itemNumber + secondItem];
    }

    return firstItem == secondItem ? matcher->match : matcher->mismatch;
}

extern int matcherGetType(Matcher* matcher) {
    return matcher->type;
}

extern MatcherCode matcherGetCode(Matcher* matcher, MatcherItem item) {

    int isWildcard; // boolean
    int isExact; // boolean

    int itemIdx;
    MatcherItem item_cur; // current item

    MatcherCode itemCode = MATCHER_CODE_NOT_FOUND;

    for (itemIdx = 0; itemIdx < matcher->itemNumber; ++itemIdx) {

        if (itemIdx == matcher->dummyCode) {
            continue;
        }

        item_cur = matcher->items[itemIdx];

        isWildcard = (item_cur == WILDCARD);
        isExact = (item == item_cur);

        if (isWildcard || isExact) {

            itemCode = (MatcherCode) itemIdx;

            // If exact matching is found the loop is stopped.
            if (isExact) {
                break;
            }
        }
    }

    return itemCode;
}

extern void matcherAddItem(Matcher* matcher, MatcherItem item) {

    if (matcher->itemNumber == 0) {
        matcher->items = malloc(sizeof(MatcherItem));
        matcher->items[0] = item;
        matcher->itemNumber++;
        return;
    } 

    int itemIdx;
    int found = 0;

    for (itemIdx = 0; itemIdx < matcher->itemNumber; ++itemIdx) {
        
        if (
            matcher->items[itemIdx] == item || 
            matcher->items[itemIdx] == WILDCARD
        ) {
            found = 1;
            break;
        }

    }

    if(found) {
        return;
    }

    matcher->items = 
        realloc(matcher->items, (matcher->itemNumber + 1) * sizeof(MatcherItem));
    matcher->items[matcher->itemNumber] = item;
    matcher->itemNumber++;
}

extern MatcherScore matcherGetMatch(Matcher* matcher) {

    if (matcher->type == MATCHER_MATRIX) {
        printf("Trying to avoid using similarity matrix.\n");
        exit(-1);
    }

    return matcher->match;
}

extern MatcherScore matcherGetMisMatch(Matcher* matcher) {

    if (matcher->type == MATCHER_MATRIX) {
        printf("Trying to avoid using similarity matrix.\n");
        exit(-1);
    }

    return matcher->mismatch;
}

extern void matcherPrint(Matcher* matcher) {

    if (TEST_MODE) return;

    printf("Similarity matrix: %s\n", matcher->typeName);
}
