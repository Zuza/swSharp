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
