#ifndef __DNAH__
#define __DNAH__

#include "../matcher.h"

int DNA_ITEM_NMR = 5;

MatcherItem* DNA_ITEMS = "ACTG*";

MatcherScore DNA_SCORES[] = {
         1, -10000, -10000, -10000, -10000,
    -10000,      1, -10000, -10000, -10000,
    -10000, -10000,      1, -10000, -10000,
    -10000, -10000, -10000,      1, -10000,
    -10000, -10000, -10000, -10000, -10000
};

#endif // __DNAH__
