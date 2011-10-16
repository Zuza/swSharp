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

#ifndef __SW_HIRSCHBERG_DATAH__
#define __SW_HIRSCHBERG_DATAH__

typedef struct SWHirschbergData SWHirschbergData;

extern SWHirschbergData* swHirschbergDataCreate();
extern void swHirschbergDataDelete(SWHirschbergData* data);

extern void swHirschbergDataAddBlock(SWHirschbergData* data, int startRow, 
    int startColumn, int endRow, int endColumn, int frontGap, int backGap);

extern int swHirschbergDataGetBlockNmr(SWHirschbergData* data);
extern int swHirschbergDataGetStartRow(SWHirschbergData* data, int index);
extern int swHirschbergDataGetStartColumn(SWHirschbergData* data, int index);
extern int swHirschbergDataGetEndRow(SWHirschbergData* data, int index);
extern int swHirschbergDataGetEndColumn(SWHirschbergData* data, int index);
extern int swHirschbergDataGetFrontGap(SWHirschbergData* data, int index);
extern int swHirschbergDataGetBackGap(SWHirschbergData* data, int index);
#endif // __SW_HIRSCHBERG_DATAH__
