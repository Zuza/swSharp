#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "chain_base.h"

#include "chain.h"
#include "matcher.h"
#include "utils.h"

#define MALLOC_STEP 100

struct ChainBase {
    int length;
    int size;
    Chain** chains;
};

extern ChainBase* chainBaseCreate(char* filePath, SWPrefs* swPrefs) {

    ChainBase* chainBase = (ChainBase*) malloc(sizeof(struct ChainBase));
    chainBase->size = 0;
    chainBase->length = 0;
    chainBase->chains = NULL;

    char* file = fileReadToBuffer(filePath);
    char* start = strchr(file, '>');
    char* end;
    
    if (!TEST_MODE) {
        printf("%s database entries: \n", filePath);
    }

    Chain* chain;

    while (start) {

        end = strchr(start + 1, '>');

        if (end) {
            *end = '\0';
        }

        chain = chainCreateFromBuffer(start, swPrefs);
        chainBaseAddChain(chainBase, chain);

        if (end) { 
            *end = '>';
        }

        start = end;
    }

    free(file);

    return chainBase;
}

extern void chainBaseDelete(ChainBase* chainBase) {

    int chainIdx;
    for (chainIdx = 0; chainIdx < chainBase->length; ++chainIdx) {
        chainDelete(chainBase->chains[chainIdx]);
    }

    free(chainBase->chains);
    free(chainBase);
}

extern Chain* chainBaseGetChain(ChainBase* chainBase, int index) {
    return chainBase->chains[index];
}

extern int chainBaseGetLength(ChainBase* chainBase) {
    return chainBase->length;
}

extern void chainBaseAddChain(ChainBase* chainBase, Chain* chain) {

    if (chainBase->length == chainBase->size) {
        chainBase->size += MALLOC_STEP;
        chainBase->chains = 
            realloc(chainBase->chains, chainBase->size * sizeof(Chain*));
    }

    chainBase->chains[chainBase->length] = chain;
    chainBase->length++;
}
