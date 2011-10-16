__constant__ MatcherScore MATCH;
__constant__ MatcherScore MISMATCH;

__device__ inline MatcherScore getMatching(ChainCode first, ChainCode second) {
    return first != second ? MISMATCH : MATCH;
}

static void prepareMatching(SWPrefs* swPrefs) {

    MatcherScore match = matcherGetMatch(swPrefsGetMatcher(swPrefs));
    MatcherScore mismatch = matcherGetMisMatch(swPrefsGetMatcher(swPrefs));

    cudaMemcpyToSymbol(MATCH, &match, sizeof(MatcherScore));
    cudaMemcpyToSymbol(MISMATCH, &mismatch, sizeof(MatcherScore));
}
