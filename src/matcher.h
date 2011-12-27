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

#ifndef __MATCHERH__
#define __MATCHERH__

#define MATCHER_MATRIX 0
#define MATCHER_MATCH_MISMATCH 1

#define MATCHER_MAX_ITEMS 80
#define MATCHER_MAX_LEN MATCHER_MAX_ITEMS * MATCHER_MAX_ITEMS

#define MATCHER_CODE_NOT_FOUND -1

#define MATCHER_SCORE_MIN -10e10
#define MATCHER_SCORE_MAX -MATCHER_SCORE_MIN

// for shotgun usage
#define MATCHER_RESET_CHAR '_'
#define MATCHER_RESET_CODE MATCHER_MAX_ITEMS + 3

typedef struct Matcher Matcher;
typedef double MatcherScore;
typedef char MatcherCode; // must always be a decimal type
typedef char MatcherItem; // must always be a decimal type

extern Matcher* matcherCreateFromFile(char* filePath);
extern Matcher* matcherCreateEmbedded(char* type);
extern Matcher* matcherCreateMM(MatcherScore match, MatcherScore mismatch);
extern void matcherDelete(Matcher* matcher);

extern int matcherGetDummyCode(Matcher* matcher);

extern int matcherGetItemNmr(Matcher* matcher);
extern void matcherGetScores(Matcher* matcher, MatcherScore* scores);
extern MatcherScore matcherGetScore(Matcher* matcher, MatcherCode firstItem,
        MatcherCode secondItem);
extern int matcherGetType(Matcher* matcher);
extern MatcherCode matcherGetCode(Matcher* matcher, MatcherItem item, 
        int shotgun);

extern void matcherAddItem(Matcher* matcher, MatcherItem item);

extern MatcherScore matcherGetMatch(Matcher* matcher);
extern MatcherScore matcherGetMisMatch(Matcher* matcher);

extern void matcherPrint(Matcher* matcher);

#endif // __MATCHERH__
