#ifndef __SW_H__
#define __SW_H__

typedef struct SW SW;

extern SW* swCreate(int argc, char* argv[]);
extern void swDelete(SW* sw);

extern void swRun(SW* sw);

#endif // __SW__
