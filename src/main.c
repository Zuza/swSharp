#include <stdio.h>
#include <stdlib.h>

#include "sw.h"

int main(int argc, char *argv[]) {

    SW* sw = swCreate(argc, argv);
    swRun(sw);
    swDelete(sw);

    return 0;
}
