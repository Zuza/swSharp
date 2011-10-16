#ifndef __SW_RECONSTRUCT_H__
#define __SW_RECONSTRUCT_H__

#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"

typedef struct Moves {
    SWResultMove* moves;
    int length;
} Moves;

extern void swReconstruct(SWData* swData, Chain* rowChain,
    Chain* columnChain, SWPrefs* swPrefs);

#endif // __SW_RECONSTRUCT_H__
