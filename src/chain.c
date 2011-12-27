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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "chain.h"
    
#include "utils.h"
#include "matcher.h"
#include "sw_prefs.h"

#define COMPLEMENT_LABEL "(-)strand "
#define REVERSE_LABEL "reversed "

struct Chain {
    int length;
    int subchain;
    char* name;
    ChainCode* codes;
    ChainItem* items;
    ChainCode* reverseCodes;
    ChainItem* reverseItems;
};

static void addReversedChain(Chain* chain);

static void readFromBuffer(Chain* chain, char* fileBuffer, SWPrefs* swPrefs);

extern Chain* chainCreateFromFile(char* filePath, SWPrefs* swPrefs) {

    if (!TEST_MODE) {
        printf("%s query: \n", filePath);
    }


    char* buffer = fileReadToBuffer(filePath);

    Chain* chain = chainCreateFromBuffer(buffer, swPrefs);

    free(buffer);

    return chain;
}

extern Chain* chainCreateFromBuffer(char* fileBuffer, SWPrefs* swPrefs) {

    Chain* chain = (Chain*) malloc(sizeof(struct Chain));
    chain->subchain = 0;

    readFromBuffer(chain, fileBuffer, swPrefs);

    if (swPrefsSolveOnly(swPrefs)) {
        chain->reverseCodes = NULL;
        chain->reverseItems = NULL;
    } else {
        addReversedChain(chain);
    }

    return chain;
}

extern Chain* chainCreateComplement(Chain* chain, SWPrefs* swPrefs) {

    Chain* complement = (Chain*) malloc(sizeof(struct Chain));

    complement->subchain = 0;
    complement->length = chain->length;

    int nameLen = strlen(chain->name) + strlen(COMPLEMENT_LABEL) + 1;
    complement->name = (char*) malloc(nameLen * sizeof(char));
    sprintf(complement->name, "%s%s", COMPLEMENT_LABEL, chain->name);

    complement->codes = (ChainCode*) malloc(complement->length * sizeof(ChainCode));
    complement->items = (ChainItem*) malloc(complement->length * sizeof(ChainItem));

    int normal;
    int reverse;
    ChainItem item;
    ChainItem newItem;

    Matcher* matcher = swPrefsGetMatcher(swPrefs);

    for (
        normal = 0, reverse = chain->length - 1; 
        normal < chain->length; 
        ++normal, --reverse
    ) {

        item = chain->items[reverse];
        
        switch (item) {
        case 'A':
            newItem = 'T';
            break;
        case 'C':
            newItem = 'G';
            break;
        case 'T':
            newItem = 'A';
            break;
        case 'G':
            newItem = 'C';
            break;
        default:
            newItem = item;
        }

        complement->items[normal] = newItem;
        complement->codes[normal] = matcherGetCode(matcher, newItem, 
            swPrefsShotgun(swPrefs));
    }

    if (swPrefsSolveOnly(swPrefs)) {
        complement->reverseCodes = NULL;
        complement->reverseItems = NULL;
    } else {
        addReversedChain(complement);
    }

    return complement;
}

extern Chain* chainCreateReverse(Chain* chain) {

    Chain* reverse = (Chain*) malloc(sizeof(struct Chain));

    reverse->subchain = 1;
    reverse->length = chain->length;

    int nameLen = strlen(chain->name) + strlen(REVERSE_LABEL) + 1;
    reverse->name = (char*) malloc(nameLen * sizeof(char));
    sprintf(reverse->name, "%s%s", REVERSE_LABEL, chain->name);

    if (chain->reverseCodes == NULL) {
        addReversedChain(chain);
    }

    reverse->codes = chain->reverseCodes;
    reverse->items = chain->reverseItems;
    reverse->reverseCodes = chain->codes;
    reverse->reverseItems = chain->items;

    return reverse;
}


static void addReversedChain(Chain* chain) {
    
    chain->reverseCodes = (ChainCode*) malloc(chain->length * sizeof(ChainCode));
    chain->reverseItems = (ChainItem*) malloc(chain->length * sizeof(ChainItem));

    int normal;
    int reverse;

    for (
        normal = 0, reverse = chain->length - 1; 
        normal < chain->length; 
        ++normal, --reverse
    ) {
        chain->reverseCodes[reverse] = chain->codes[normal];
        chain->reverseItems[reverse] = chain->items[normal];
    }
}

static void readFromBuffer(Chain* chain, char* fileBuffer, SWPrefs* swPrefs) {

    Matcher* matcher = swPrefsGetMatcher(swPrefs);
    int shotgun = swPrefsShotgun(swPrefs);
    int fileLength = strlen(fileBuffer);

    // Maximum chain length is equal to input file length.
    chain->items = (ChainItem*) malloc(sizeof(ChainItem) * fileLength);
    chain->codes = (ChainCode*) malloc(sizeof(ChainCode) * fileLength);

    chain->length = 0;

    int fileCharIdx;
    int isSpace; // boolean
    ChainItem chainItem;
    ChainItem chainCode;

    int fillItems = (matcherGetType(matcher) == MATCHER_MATCH_MISMATCH);

    int start = 0;
    while (fileBuffer[start] != '\n') start++;

    chain->name = malloc((start + 1) * sizeof(char));
    chain->name = strncpy(chain->name, fileBuffer, start);
    chain->name[start] = '\0';

    // Read all items into the item array. Use matcher object to get the item
    // codes and write them to code array.
    for (fileCharIdx = start; fileCharIdx < fileLength; ++fileCharIdx) {

        isSpace = isspace(fileBuffer[fileCharIdx]);

        if (isSpace) {
            continue;
        }

        chainItem = (ChainItem) toupper(fileBuffer[fileCharIdx]);
        chain->items[chain->length] = chainItem;
        
        if (fillItems) matcherAddItem(matcher, chainItem);

        chainCode = (ChainCode) matcherGetCode(matcher, chainItem, shotgun);
        chain->codes[chain->length] = chainCode;

        if (chain->codes[chain->length] == MATCHER_CODE_NOT_FOUND) {
            printf("Item not in matching table %c.\n", chainItem);
            exit(-1);
        }

        chain->length++;
    }

    if (!TEST_MODE) {   
        printf("%.55s... length:%14d\n", chain->name, chain->length);
    }
}

extern void chainDelete(Chain* chain) {

    if (!chain->subchain) {
        free(chain->codes);
        free(chain->items);
        free(chain->reverseCodes);
        free(chain->reverseItems);
        free(chain->name);
    }

    free(chain);
}

extern int chainGetLength(Chain* chain) {
    return chain->length;
}

extern char* chainGetName(Chain* chain) {
    return chain->name;
}

extern ChainCode* chainGetCodes(Chain* chain) {
    return chain->codes;
}

extern ChainCode chainGetCode(Chain* chain, int index) {
    return chain->codes[index];
}

extern ChainItem chainGetItem(Chain* chain, int index) {
    return chain->items[index];
}


extern Chain* reverseSubchain(Chain* chain, int start, int end) {

    Chain* subchain = (Chain*) malloc(sizeof(struct Chain));

    subchain->subchain = 1;
    subchain->length = (end - start) + 1;
    subchain->codes = chain->reverseCodes + (chain->length - end - 1);
    subchain->items = chain->reverseItems + (chain->length - end - 1);
    subchain->reverseCodes = chain->codes + start;
    subchain->reverseItems = chain->items + start;

    return subchain;
}

extern Chain* subchain(Chain* chain, int start, int end) {

    Chain* subchain = (Chain*) malloc(sizeof(Chain));

    subchain->subchain = 1;
    subchain->length = (end - start) + 1;
    subchain->codes = chain->codes + start;
    subchain->items = chain->items + start;
    subchain->reverseCodes = chain->reverseCodes + (chain->length - end - 1);
    subchain->reverseItems = chain->reverseItems + (chain->length - end - 1);

    return subchain;
}

extern void chainPrint(Chain* chain) {

    if (TEST_MODE) return;
    printf("%.80s\n", chain->name);
}
