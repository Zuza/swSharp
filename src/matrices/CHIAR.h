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

#ifndef __CHIARH__
#define __CHIARH__

#include "../matcher.h"

int CHIAR_ITEM_NMR = 5;

MatcherItem* CHIAR_ITEMS = "ACTG*";

MatcherScore CHIAR_SCORES[] = {
        91,   -114,    -31,   -123,      0,
      -114,    100,   -125,    -31,      0,
       -31,   -125,    100,   -114,      0,
      -123,    -31,   -114,     91,      0,
         0,      0,      0,      0,      0
};

#endif // __CHIARH__
