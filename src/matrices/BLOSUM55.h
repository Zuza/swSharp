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

#ifndef __BLOSUM55H__
#define __BLOSUM55H__

#include "../matcher.h"

int BLOSUM55_ITEM_NMR = 24;

MatcherItem* BLOSUM55_ITEMS = "ABCDEFGHIKLMNPQRSTVWXYZ*";

MatcherScore BLOSUM55_SCORES[] = {
     5, -2,  0, -2, -1, -3,  0, -2, -2, -1, -2, -1, -2, -1, -1, -2,  2,  0,  0, -4, -1, -2, -1, -5, 
    -2,  5, -4,  5,  1, -5, -1,  0, -4,  0, -4, -3,  4, -2,  0, -1,  0, -1, -4, -5, -1, -3,  2, -5, 
     0, -4, 13, -4, -4, -3, -3, -4, -2, -4, -2, -2, -3, -3, -4, -4, -1, -1, -1, -4, -2, -3, -4, -5, 
    -2,  5, -4,  8,  2, -5, -2, -1, -4, -1, -5, -4,  2, -2,  0, -2,  0, -1, -4, -5, -2, -3,  1, -5, 
    -1,  1, -4,  2,  7, -4, -3, -1, -4,  1, -4, -3,  0, -1,  2,  0,  0, -1, -3, -3, -1, -2,  5, -5, 
    -3, -5, -3, -5, -4,  9, -4, -1,  0, -4,  1,  0, -4, -5, -4, -3, -3, -3, -1,  2, -2,  4, -4, -5, 
     0, -1, -3, -2, -3, -4,  8, -2, -5, -2, -5, -3,  0, -3, -2, -3,  0, -2, -4, -3, -2, -4, -3, -5, 
    -2,  0, -4, -1, -1, -1, -2, 11, -4,  0, -3, -2,  1, -3,  1,  0, -1, -2, -4, -3, -1,  2,  0, -5, 
    -2, -4, -2, -4, -4,  0, -5, -4,  6, -4,  2,  2, -4, -3, -4, -4, -3, -1,  4, -3, -1, -1, -4, -5, 
    -1,  0, -4, -1,  1, -4, -2,  0, -4,  6, -3, -2,  0, -1,  2,  3,  0, -1, -3, -4, -1, -2,  1, -5, 
    -2, -4, -2, -5, -4,  1, -5, -3,  2, -3,  6,  3, -4, -4, -3, -3, -3, -2,  1, -3, -1, -1, -3, -5, 
    -1, -3, -2, -4, -3,  0, -3, -2,  2, -2,  3,  8, -3, -3,  0, -2, -2, -1,  1, -2, -1, -1, -2, -5, 
    -2,  4, -3,  2,  0, -4,  0,  1, -4,  0, -4, -3,  8, -2,  0, -1,  1,  0, -4, -5, -1, -2,  0, -5, 
    -1, -2, -3, -2, -1, -5, -3, -3, -3, -1, -4, -3, -2, 10, -1, -3, -1, -1, -3, -5, -2, -4, -1, -5, 
    -1,  0, -4,  0,  2, -4, -2,  1, -4,  2, -3,  0,  0, -1,  7,  1,  0, -1, -3, -2, -1, -1,  4, -5, 
    -2, -1, -4, -2,  0, -3, -3,  0, -4,  3, -3, -2, -1, -3,  1,  8, -1, -1, -3, -3, -1, -2,  0, -5, 
     2,  0, -1,  0,  0, -3,  0, -1, -3,  0, -3, -2,  1, -1,  0, -1,  5,  2, -2, -4, -1, -2,  0, -5, 
     0, -1, -1, -1, -1, -3, -2, -2, -1, -1, -2, -1,  0, -1, -1, -1,  2,  6,  0, -3, -1, -2, -1, -5, 
     0, -4, -1, -4, -3, -1, -4, -4,  4, -3,  1,  1, -4, -3, -3, -3, -2,  0,  5, -4, -1, -2, -3, -5, 
    -4, -5, -4, -5, -3,  2, -3, -3, -3, -4, -3, -2, -5, -5, -2, -3, -4, -3, -4, 15, -3,  3, -3, -5, 
    -1, -1, -2, -2, -1, -2, -2, -1, -1, -1, -1, -1, -1, -2, -1, -1, -1, -1, -1, -3, -1, -1, -1, -5, 
    -2, -3, -3, -3, -2,  4, -4,  2, -1, -2, -1, -1, -2, -4, -1, -2, -2, -2, -2,  3, -1,  9, -2, -5, 
    -1,  2, -4,  1,  5, -4, -3,  0, -4,  1, -3, -2,  0, -1,  4,  0,  0, -1, -3, -3, -1, -2,  5, -5, 
    -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5,  1
};

#endif // __BLOSUM55H__
