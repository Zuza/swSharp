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

#ifndef __BLASTH__
#define __BLASTH__

#include "../matcher.h"

int BLAST_ITEM_NMR = 5;

MatcherItem* BLAST_ITEMS = "ACTG*";

MatcherScore BLAST_SCORES[] = {
     1, -3, -3, -3, -3,
    -3,  1, -3, -3, -3,
    -3, -3,  1, -3, -3,
    -3, -3, -3,  1, -3,
    -3, -3, -3, -3, -3
};

#endif // __DNAH__
