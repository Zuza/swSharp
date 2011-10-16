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
