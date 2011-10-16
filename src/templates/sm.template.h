__constant__ int MATCHER_ITEMS;
__constant__ MatcherScore MATCHER[MATCHER_MAX_LEN];

__device__ inline MatcherScore getMatching(ChainCode first, ChainCode second) {
    return MATCHER[first * MATCHER_ITEMS + second];
}

static void prepareMatching(SWPrefs* swPrefs) {

    int itemNmr =  matcherGetItemNmr(swPrefsGetMatcher(swPrefs));

    MatcherScore matcher[MATCHER_MAX_LEN];
    matcherGetScores(swPrefsGetMatcher(swPrefs), matcher);

    cudaMemcpyToSymbol(MATCHER_ITEMS, &itemNmr, sizeof(int));
    cudaMemcpyToSymbol(MATCHER, matcher, sizeof(MatcherScore) * MATCHER_MAX_LEN);
}
