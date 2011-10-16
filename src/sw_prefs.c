#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "sw_prefs.h"

#include "matcher.h"

#define CPU_RECONSTRUCT_BLOCK_LIMIT 1800

struct SWPrefs {
    Matcher* matcher;
    SWPrefsPenalty gapOpen;
    SWPrefsPenalty gapExtend;
    int cudaThreads;
    int cudaBlocks;
    int reconstructBlockLimit;
    int paramSearch;
    int forceCPU;
    int solveOnly;
    int global;
    int complement;
    int maximumGaps;
    int windowSize;
    char* queryFile;
    char* databaseFile;
    MatcherScore threshold;
};

int VERBOSE = DEFAULT_VERBOSE;
int TEST_MODE = DEFAULT_TEST_MODE;

static void help();

extern SWPrefs* swPrefsCreate(int argc, char* argv[]) {

    SWPrefs* swPrefs = (SWPrefs*) malloc(sizeof(struct SWPrefs));

    swPrefs->gapOpen = DEFAULT_GAP_OPEN;
    swPrefs->gapExtend = DEFAULT_GAP_EXTEND;
    
    swPrefs->reconstructBlockLimit = CPU_RECONSTRUCT_BLOCK_LIMIT;
    swPrefs->cudaThreads = CUDA_THREADS_DEFUALT;
    swPrefs->cudaBlocks = CUDA_BLOCKS_DEFUALT;

    swPrefs->forceCPU = 0;
    swPrefs->solveOnly = 0;
    swPrefs->global = 0;
    swPrefs->paramSearch = 0;
    swPrefs->threshold = NO_THRESHOLD;
    swPrefs->complement = 0;
    swPrefs->maximumGaps = 0;
    swPrefs->windowSize = 0;

    swPrefs->queryFile = NULL;
    swPrefs->databaseFile = NULL;

    MatcherScore match = 0;
    MatcherScore mismatch = 0;
    int matchDefined = 0;
    int mismatchDefined = 0;
    char* embeededArg = QUOTE(DEFUALT_MATCHER);
    char* matcherArg = NULL;

    char argument;

    while (1) {

        static struct option options[] = {
            {"query", required_argument, 0, 'i'},
            {"target", required_argument, 0, 'j'},
            {"matrix-file", required_argument, 0, 'f'},
            {"matrix-table", required_argument, 0, 'm'},
            {"match", required_argument, 0, 'q'},
            {"mismatch", required_argument, 0, 'r'},
            {"gap-open", required_argument, 0, 'g'},
            {"gap-extend", required_argument, 0, 'e'},
            {"threshold", required_argument, 0, 't'},
            {"complement", no_argument, 0, 'h'},
            {"solve", no_argument, 0, 's'},
            {"cpu", no_argument, 0, 'c'},
            {"verbose", no_argument, 0, 'v'},
            {"param-search", no_argument, 0, 'p'},
            {"global", no_argument, 0, 'n'},
            {"test", no_argument, 0, 'x'},
            {"min-hits", required_argument, 0, 'b'},
            {"window", required_argument, 0, 'w'},
            {0, 0, 0, 0}
        };

        int optionIndex = 0;
     
        argument = getopt_long(argc, argv, "i:j:g:e:", options, &optionIndex);

        if (argument == -1) {
            break;
        }

        switch (argument) {
        case 'i':
            swPrefs->queryFile = optarg;
            break;
        case 'j':
            swPrefs->databaseFile = optarg;
            break;
        case 'f':
            matcherArg = optarg;
            break;
        case 'm':
            embeededArg = optarg;
            break;
        case 'g':
            swPrefs->gapOpen = (SWPrefsPenalty) atof(optarg);
            break;
        case 'e':
            swPrefs->gapExtend = (SWPrefsPenalty) atof(optarg);
            break;
        case 's':
            swPrefs->solveOnly = 1;
            break;
        case 'c':
            swPrefs->forceCPU = 1;
            break;
        case 'p':
            swPrefs->paramSearch = 1;
            break;
        case 'v':
            VERBOSE = 1;
            break;
        case 'q':
            match = atof(optarg);
            matchDefined = 1;
            break;
        case 'r':
            mismatch = atof(optarg);
            mismatchDefined = 1;
            break;
        case 't':
            swPrefs->threshold = atof(optarg);
            break;
        case 'n':
            swPrefs->global = 1;
            break;
        case 'x':
            TEST_MODE = 1;
            break;
        case 'h':
            swPrefs->complement = 1;
            break;
        case 'b':
            swPrefs->maximumGaps = atoi(optarg);
            break;
        case 'w':
            swPrefs->windowSize = atoi(optarg);
            break;
        default:
            help();
        } 
    }

    if (swPrefs->queryFile == NULL || swPrefs->databaseFile == NULL) {
        help();
    }

    if (TEST_MODE) {
        VERBOSE = 0;
    }

    if (swPrefs->global) {
        swPrefs->threshold = NO_THRESHOLD;
    }

    if (mismatchDefined && matchDefined) {
        swPrefs->matcher = matcherCreateMM(match, mismatch);
    } else if (!matcherArg) {
        swPrefs->matcher = matcherCreateEmbedded(embeededArg);
    } else {
        swPrefs->matcher = matcherCreateFromFile(matcherArg);
    }

    return swPrefs;
}

extern void swPrefsDelete(SWPrefs* swPrefs) {
    matcherDelete(swPrefs->matcher);
    free(swPrefs);
}

extern SWPrefsPenalty swPrefsGetGapOpen(SWPrefs* swPrefs) {
    return swPrefs->gapOpen;
}

extern SWPrefsPenalty swPrefsGetGapExtend(SWPrefs* swPrefs) {
    return swPrefs->gapExtend;
}

extern Matcher* swPrefsGetMatcher(SWPrefs* swPrefs) {
    return swPrefs->matcher;
}

extern int swPrefsGetReconstructBlockLimit(SWPrefs* swPrefs) {
    return swPrefs->reconstructBlockLimit;
}

extern int swPrefsGetCudaThreads(SWPrefs* swPrefs) {
    return swPrefs->cudaThreads;
}

extern int swPrefsGetCudaBlocks(SWPrefs* swPrefs) {
    return swPrefs->cudaBlocks;
}

extern void swPrefsSetCudaThreads(SWPrefs* swPrefs, int threads) {
    swPrefs->cudaThreads = threads;
}

extern void swPrefsSetCudaBlocks(SWPrefs* swPrefs, int blocks) {
    swPrefs->cudaBlocks = blocks;
}

extern int swPrefsForceCPU(SWPrefs* swPrefs) {
    return swPrefs->forceCPU;
}

extern int swPrefsSolveOnly(SWPrefs* swPrefs) {
    return swPrefs->solveOnly;
}

extern int swPrefsGlobal(SWPrefs* swPrefs) {
    return swPrefs->global;
}

extern int swPrefsParamSearch(SWPrefs* swPrefs) {
    return swPrefs->paramSearch;
}

extern int swPrefsComplement(SWPrefs* swPrefs) {
    return swPrefs->complement;
}

extern void swPrefsSetThreshold(SWPrefs* swPrefs, MatcherScore threshold) {
    swPrefs->threshold = threshold;
}

extern MatcherScore swPrefsGetThreshold(SWPrefs* swPrefs) {
    return swPrefs->threshold;
}

extern char* swPrefsGetQueryFile(SWPrefs* swPrefs) {
    return swPrefs->queryFile;
}

extern char* swPrefsGetDatabaseFile(SWPrefs* swPrefs) {
    return swPrefs->databaseFile;
}

extern int swPrefsGetMaximumGaps(SWPrefs* swPrefs) {
    return swPrefs->maximumGaps;
}

extern int swPrefsGetWindowSize(SWPrefs* swPrefs) {
    return swPrefs->windowSize;
}

extern void swPrefsPrint(SWPrefs* swPrefs) {

    if (TEST_MODE) return;

    printf("Gap open penalty: %.5lf\n", (double) swPrefs->gapOpen);
    printf("Gap extend penalty: %.5lf\n", (double) swPrefs->gapExtend);

    matcherPrint(swPrefs->matcher);
}

static void help() {
    printf(
"\
./sw# -i <first_sequnce> -j <second_sequnce> [OPTIONS] [FLAGS]\n\
\n\
OPTIONS:\n\
    --query, -i <first_sequnce>\n\
        fasta query file path\n\
    --target, -j <second_sequnce>\n\
        fasta database file path\n\
        fasta database are appended fasta files in a single text file\n\
    --matrix-file <file_path> \n\
        (optional)\n\
        similarity matrix file path \n\
        if given the similarity matrix is loaded from the file\n\
    --matrix-table <BLOSUM50|BLOSUM55|BLOSUM62|BLOSUM70|DNA|BLAST|CHIAR>\n\
        embedded similarity matrix\n\
        (optional) \n\
        (default: BLOSUM62) \n\
        if given --matrix-file option won't be used\n\
    --match <float> \n\
        (optional)\n\
        match value  \n\
        if --match and --mismatch are given, --matrix-table and --matrix-file\n\
        options are not used\n\
    --mismatch <float> \n\
        (optional)\n\
        mismacth value  \n\
        if --match and --mismatch are given, --matrix-table and --matrix-file\n\
        options are not used\n\
    --gap-open, -g <float> \n\
        (optional)\n\
        (default: %lf)\n\
        gap open value\n\
    --gap-extend, -e <float> \n\
        (optional)\n\
        (default: %lf)\n\
        gap extend value  \n\
    --threshold <float>\n\
        (optional)\n\
        (default: NO_THRESHOLD)\n\
        score threshold\n\
        program will output not just the max score and its reconstruction, but\n\
        all the scores which are above the given threshold\n\
    --min-hits <int>\n\
        (optional)\n\
        (default: 0)\n\
        the option is used with --window option, it means there must be at \n\
        least min_hits of matchings in every window length segment\n\
    --window <int>\n\
        (optional)\n\
        (default: 0)\n\
        the option is used with --min-hits option, it means there must be at \n\
        least min_hits of matchings in every window length segment\n\
FLAGS:\n\
    --complement \n\
        (optional)\n\
        if input is a nucleotide, search also the other strand \n\
    --solve \n\
        (optional)\n\
        solve only \n\
    --cpu \n\
        (optional)\n\
        force CPU solving\n\
    --param-search \n\
        (optional)\n\
        CUDA block and thread number optimization search\n\
    --verbose\n\
        (optional)\n\
        verbose\n\
    --global \n\
        (optional)\n\
        global aligment \n\
        if given --threshold this option won't be used\n\
"
        ,
        (double) DEFAULT_GAP_OPEN,
        (double) DEFAULT_GAP_EXTEND
    );
    exit(-1);
}
