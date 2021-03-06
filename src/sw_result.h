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

#ifndef __SW_RESULTH__
#define __SW_RESULTH__

#define SW_RESULT_MOVE_STOP 0
#define SW_RESULT_MOVE_DIAGONAL 1
#define SW_RESULT_MOVE_LEFT 2
#define SW_RESULT_MOVE_UP 3

#define SW_RESULT_EQUAL 0
#define SW_RESULT_NOT_EQUAL 1

typedef struct SWResult SWResult;
typedef double SWResultScore; // precise type
typedef char SWResultMove; // must always be a decimal type

extern SWResult* swResultCreate(SWResultScore score, int row, int column);
extern void swResultDelete(SWResult* swResult);

extern void swResultSetLength(SWResult* swResult, int length);
extern void swResultSetIdentity(SWResult* swResult, int identity);
extern void swResultSetReconstruction(SWResult* swResult, char* row, char* column);

extern void swResultSetScore(SWResult* swResult, SWResultScore score);

extern void swResultSetStartRow(SWResult* swResult, int row);
extern void swResultSetStartColumn(SWResult* swResult, int column);

extern SWResultScore swResultGetScore(SWResult* swResult);

extern int swResultGetStartRow(SWResult* swResult);
extern int swResultGetStartColumn(SWResult* swResult);
extern int swResultGetEndRow(SWResult* swResult);
extern int swResultGetEndColumn(SWResult* swResult);
extern int swResultGetLength(SWResult* swResult);

extern void swResultSetComplementary(SWResult* swResult, int length);

extern int swResultCompare(SWResult* a, SWResult* b);

extern void swResultPrint(SWResult* swResult);
#endif // __SW_RESULTH__
