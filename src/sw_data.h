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
