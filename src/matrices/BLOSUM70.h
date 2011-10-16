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

#ifndef __BLOSUM70H__
#define __BLOSUM70H__

#include "../matcher.h"

int BLOSUM70_ITEM_NMR = 24;

MatcherItem* BLOSUM70_ITEMS = "ABCDEFGHIKLMNPQRSTVWXYZ*";

MatcherScore BLOSUM70_SCORES[] = {
     4,-2,-1,-2,-1,-2, 0,-2,-2,-1,-2,-1,-2,-1,-1,-2, 1, 0, 0,-3,-1,-2,-1,-5,
    -2, 4,-4, 4, 1,-4,-1,-1,-4,-1,-4,-3, 3,-2, 0,-1, 0,-1,-3,-4,-1,-3, 0,-5,
    -1,-4, 9,-4,-4,-2,-3,-4,-1,-4,-2,-2,-3,-3,-3,-4,-1,-1,-1,-3,-2,-3,-4,-5,
    -2, 4,-4, 6, 1,-4,-2,-1,-4,-1,-4,-3, 1,-2,-1,-2, 0,-1,-4,-5,-2,-4, 1,-5,
    -1, 1,-4, 1, 5,-4,-2, 0,-4, 1,-3,-2, 0,-1, 2, 0, 0,-1,-3,-4,-1,-3, 4,-5,
    -2,-4,-2,-4,-4, 6,-4,-1, 0,-3, 0, 0,-3,-4,-3,-3,-3,-2,-1, 1,-2, 3,-4,-5,
     0,-1,-3,-2,-2,-4, 6,-2,-4,-2,-4,-3,-1,-3,-2,-3,-1,-2,-4,-3,-2,-4,-2,-5,
    -2,-1,-4,-1, 0,-1,-2, 8,-4,-1,-3,-2, 0,-2, 1, 0,-1,-2,-3,-2,-1, 2, 0,-5,
    -2,-4,-1,-4,-4, 0,-4,-4, 4,-3, 2, 1,-4,-3,-3,-3,-3,-1, 3,-3,-1,-1,-3,-5,
    -1,-1,-4,-1, 1,-3,-2,-1,-3, 5,-3,-2, 0,-1, 1, 2, 0,-1,-3,-3,-1,-2, 1,-5,
    -2,-4,-2,-4,-3, 0,-4,-3, 2,-3, 4, 2,-4,-3,-2,-3,-3,-2, 1,-2,-1,-1,-3,-5,
    -1,-3,-2,-3,-2, 0,-3,-2, 1,-2, 2, 6,-2,-3, 0,-2,-2,-1, 1,-2,-1,-1,-2,-5,
    -2, 3,-3, 1, 0,-3,-1, 0,-4, 0,-4,-2, 6,-2, 0,-1, 0, 0,-3,-4,-1,-2, 0,-5,
    -1,-2,-3,-2,-1,-4,-3,-2,-3,-1,-3,-3,-2, 8,-2,-2,-1,-1,-3,-4,-2,-3,-1,-5,
    -1, 0,-3,-1, 2,-3,-2, 1,-3, 1,-2, 0, 0,-2, 6, 1, 0,-1,-2,-2,-1,-2, 3,-5,
    -2,-1,-4,-2, 0,-3,-3, 0,-3, 2,-3,-2,-1,-2, 1, 6,-1,-1,-3,-3,-1,-2, 0,-5,
     1, 0,-1, 0, 0,-3,-1,-1,-3, 0,-3,-2, 0,-1, 0,-1, 4, 1,-2,-3,-1,-2, 0,-5,
     0,-1,-1,-1,-1,-2,-2,-2,-1,-1,-2,-1, 0,-1,-1,-1, 1, 5, 0,-3,-1,-2,-1,-5,
     0,-3,-1,-4,-3,-1,-4,-3, 3,-3, 1, 1,-3,-3,-2,-3,-2, 0, 4,-3,-1,-2,-3,-5,
    -3,-4,-3,-5,-4, 1,-3,-2,-3,-3,-2,-2,-4,-4,-2,-3,-3,-3,-3,11,-3, 2,-3,-5,
    -1,-1,-2,-2,-1,-2,-2,-1,-1,-1,-1,-1,-1,-2,-1,-1,-1,-1,-1,-3,-1,-2,-1,-5,
    -2,-3,-3,-4,-3, 3,-4, 2,-1,-2,-1,-1,-2,-3,-2,-2,-2,-2,-2, 2,-2, 7,-2,-5,
    -1, 0,-4, 1, 4,-4,-2, 0,-3, 1,-3,-2, 0,-1, 3, 0, 0,-1,-3,-3,-1,-2, 4,-5,
    -5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5, 1
};

#endif // __BLOSUM70H__
