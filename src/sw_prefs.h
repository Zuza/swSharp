#ifndef __SW_PREFSH__
#define __SW_PREFSH__

#include "matcher.h"

#define SEPARATOR1 "-----------------------------------------------------------\
---------------------"

#define SEPARATOR2 "###########################################################\
#####################"

#define QUOTE2(x) #x
#define QUOTE(x) QUOTE2(x)

#define CONCATE2(x, y) x ## _ ## y
#define CONCATE1(x, y) CONCATE2(x, y)
#define CONCATE(x, y) CONCATE1(x, y)

#define CPU_RECONSTRUCT_MAX_SIZE 3500l
#define CPU_SOLVE_MAX_SIZE 1000000l

#define CUDA_THREADS_DEFUALT 128
#define CUDA_BLOCKS_DEFUALT 256

#define DEFAULT_GAP_OPEN 400
#define DEFAULT_GAP_EXTEND 30

#define DEFUALT_MATCHER CHIAR

#define DEFAULT_VERBOSE 0
#define DEFAULT_TEST_MODE 0
#define NO_THRESHOLD 0

#define GAP_ITEM '-'

typedef struct SWPrefs SWPrefs;
typedef MatcherScore SWPrefsPenalty;

extern int VERBOSE;
extern int TEST_MODE;

extern SWPrefs* swPrefsCreate(int argc, char* argv[]);
extern void swPrefsDelete(SWPrefs* swPrefs);

extern SWPrefsPenalty swPrefsGetGapOpen(SWPrefs* swPrefs);
extern SWPrefsPenalty swPrefsGetGapExtend(SWPrefs* swPrefs);
extern Matcher* swPrefsGetMatcher(SWPrefs* swPrefs);

extern int swPrefsGetReconstructBlockLimit(SWPrefs* swPrefs);

extern int swPrefsGetCudaThreads(SWPrefs* swPrefs);
extern int swPrefsGetCudaBlocks(SWPrefs* swPrefs);

extern void swPrefsSetCudaThreads(SWPrefs* swPrefs, int threads);
extern void swPrefsSetCudaBlocks(SWPrefs* swPrefs, int blocks);

extern int swPrefsForceCPU(SWPrefs* swPrefs);
extern int swPrefsSolveOnly(SWPrefs* swPrefs);
extern int swPrefsGlobal(SWPrefs* swPrefs);
extern int swPrefsParamSearch(SWPrefs* swPrefs);
extern int swPrefsComplement(SWPrefs* swPrefs);

extern void swPrefsSetThreshold(SWPrefs* swPrefs, MatcherScore threshold);
extern MatcherScore swPrefsGetThreshold(SWPrefs* swPrefs);

extern char* swPrefsGetQueryFile(SWPrefs* swPrefs);
extern char* swPrefsGetDatabaseFile(SWPrefs* swPrefs);

extern int swPrefsGetMaximumGaps(SWPrefs* swPrefs);
extern int swPrefsGetWindowSize(SWPrefs* swPrefs);

extern void swPrefsPrint(SWPrefs* swPrefs);
#endif // __SW_PREFSH__
