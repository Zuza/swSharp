#ifndef __CHAINH__
#define __CHAINH__

#include "sw_prefs.h"

typedef struct Chain Chain;
typedef MatcherCode ChainCode;
typedef MatcherItem ChainItem;

extern Chain* chainCreateFromFile(char* filePath, SWPrefs* swPrefs);
extern Chain* chainCreateFromBuffer(char* fileBuffer, SWPrefs* swPrefs);
extern Chain* chainCreateComplement(Chain* chain, SWPrefs* swPrefs);
extern Chain* chainCreateReverse(Chain* chain);

extern void chainDelete(Chain* chain);

extern int chainGetLength(Chain* chain);
extern char* chainGetName(Chain* chain);

extern ChainCode* chainGetCodes(Chain* chain);
extern ChainCode chainGetCode(Chain* chain, int index);
extern ChainItem chainGetItem(Chain* chain, int index);

extern Chain* reverseSubchain(Chain* chain, int start, int end);
extern Chain* subchain(Chain* chain, int start, int end);

extern void chainPrint(Chain* chain);
#endif // __CHAINH__
