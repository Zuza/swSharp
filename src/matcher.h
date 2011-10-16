#ifndef __MATCHERH__
#define __MATCHERH__

#define MATCHER_MATRIX 0
#define MATCHER_MATCH_MISMATCH 1

#define MATCHER_MAX_ITEMS 80
#define MATCHER_MAX_LEN MATCHER_MAX_ITEMS * MATCHER_MAX_ITEMS

#define MATCHER_CODE_NOT_FOUND -1

#define MATCHER_SCORE_MIN -10e10
#define MATCHER_SCORE_MAX -MATCHER_SCORE_MIN

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
extern MatcherCode matcherGetCode(Matcher* matcher, MatcherItem item);

extern void matcherAddItem(Matcher* matcher, MatcherItem item);

extern MatcherScore matcherGetMatch(Matcher* matcher);
extern MatcherScore matcherGetMisMatch(Matcher* matcher);

extern void matcherPrint(Matcher* matcher);

#endif // __MATCHERH__
