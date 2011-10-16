#ifndef __CHAIN_BASEH__
#define __CHAIN_BASEH__

#include "chain.h"
#include "sw_prefs.h"

typedef struct ChainBase ChainBase;

extern ChainBase* chainBaseCreate(char* filePath, SWPrefs* swPrefs);
extern void chainBaseDelete(ChainBase* chainBase);

extern Chain* chainBaseGetChain(ChainBase* chainBase, int index);
extern int chainBaseGetLength(ChainBase* chainBase);

extern void chainBaseAddChain(ChainBase* chainBase, Chain* chain);
#endif // __CHAIN_BASEH__
