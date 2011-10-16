#ifndef __BLASTH__
#define __BLASTH__

#include "../matcher.h"

int BLAST_ITEM_NMR = 5;

MatcherItem* BLAST_ITEMS = "ACTG*";

MatcherScore BLAST_SCORES[] = {
     1, -3, -3, -3, -3,
    -3,  1, -3, -3, -3,
    -3, -3,  1, -3, -3,
    -3, -3, -3,  1, -3,
    -3, -3, -3, -3, -3
};

#endif // __DNAH__
