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

#ifndef __SW_DATAH__
#define __SW_DATAH__

#include "sw_result.h"
#include "sw_prefs.h"

typedef struct SWData SWData;

extern SWData* swDataCreate(int rows, int columns, SWPrefs* swPrefs);
extern void swDataDelete(SWData* swData);

extern int swDataGetRows(SWData* swData);
extern int swDataGetColumns(SWData* swData);

extern void swDataAddResult(SWData* swData, SWResult* result);
extern SWResult* swDataGetResult(SWData* swData, int index);
extern int swDataGetResultNmr(SWData* swData);

extern void swDataSetTime(SWData* swData, double time);

extern SWResult* swDataPopResult(SWData* swData);

extern void swDataPrint(SWData* swData);
#endif // __SW_DATAH__
