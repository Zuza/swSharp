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
