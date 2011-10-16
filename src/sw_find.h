#ifndef __SW_FINDH__
#define __SW_FINDH__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"
#include "matcher.h"

extern SWData* swFind(Chain* firstChain, Chain* secondChain, SWPrefs* swPrefs,
    MatcherScore score);

#endif // __SW_FINDH__
