#ifndef __CHIARH__
#define __CHIARH__

#include "../matcher.h"

int CHIAR_ITEM_NMR = 5;

MatcherItem* CHIAR_ITEMS = "ACTG*";

MatcherScore CHIAR_SCORES[] = {
        91,   -114,    -31,   -123,      0,
      -114,    100,   -125,    -31,      0,
       -31,   -125,    100,   -114,      0,
      -123,    -31,   -114,     91,      0,
         0,      0,      0,      0,      0
};

#endif // __CHIARH__
