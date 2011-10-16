#include <stdlib.h>
#include <math.h>

#include "utils.h"

extern FILE* fileOpen(char* filePath) {

    FILE* file = fopen(filePath, "r");

    if (file == NULL) {
        printf("Cannot open file %s.\n", filePath);
        exit(-1);
    }

    return file;
}

extern int fileGetLength(FILE* file) {

    int fileLength;

    // Use end file pointer to find file length.
    fseek(file, 0L, SEEK_END);
    fileLength = ftell(file);
    // Reset file pointer to the file start.
    fseek(file, 0L, SEEK_SET);

    return fileLength;
}

extern char* fileReadToBuffer(char* filePath) {

    FILE* file = fileOpen(filePath);
    int fileLength = fileGetLength(file);

    char* fileBuffer = (char*) malloc(sizeof(char) * (fileLength + 1));
    fileBuffer[fileLength] = '\0';

    int bytesRead = fread(fileBuffer, 1, fileLength, file);
    if (bytesRead == -1) {
        fileReadError(filePath);
    }

    fclose(file);

    return fileBuffer;
}

extern void fileReadError(char* filePath) {
    printf("Invalid input file %s.\n", filePath);
    exit(-1);
}

extern void printPercentageBar(int percent) {

    char line[80] = "";
    int charIdx;

    for (charIdx = 0; charIdx < percent * 78 / 100 - 1; ++charIdx) {
        line[charIdx] = '=';
    }

    line[charIdx] = '>';
    line[charIdx + 1] = '\0';

    printf("[%-78s]\r", line);

    if (percent == 100) {
        printf("\n");
    }

    fflush(stdout);
}

extern int preciseEqual(double a, double b) {
    return fabs(a - b) < 10e-5;
}
