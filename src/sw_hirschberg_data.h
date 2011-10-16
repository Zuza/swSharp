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
