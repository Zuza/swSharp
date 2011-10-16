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
