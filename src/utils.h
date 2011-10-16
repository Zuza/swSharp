#ifndef __UTILSH__
#define __UTILSH__

#include <stdio.h>


extern FILE* fileOpen(char* filePath);
extern int fileGetLength(FILE* file);
extern char* fileReadToBuffer(char* filePath);
extern void fileReadError(char* filePath);

extern void printPercentageBar(int percent);

extern int preciseEqual(double a, double b);
#endif // __UTILSH__

