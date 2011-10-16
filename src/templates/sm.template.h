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
