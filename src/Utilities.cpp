#include <sys\stat.h>

#include "Utilities.h"
#include "commands.h"

int MyFPuts(const uint8_t *str, FILE *stream) {
    assert(str != nullptr);
    assert(stream != nullptr);

    for (int i = 0; str[i] != '\0' && str[i] != '\n'; i++) {
        fputc(str[i], stream);
    }
    fputc('\n', stream);

    return 1;
}

bool MyLblCmp(const int8_t* str1, const int8_t* str2) {
    size_t strIdx = 0;
    
    while ((str1[strIdx] != '\0') && (str2[strIdx] != '\0')) {
        if (str1[strIdx] != str2[strIdx]) {
            return 1;
        }

        strIdx++;
    }

    if ((str1[strIdx] == ':') || (str2[strIdx] == ':'))
        return 0;
    else
        return 1;
}

bool MyStrCmp(const int8_t* str1, const int8_t* str2) {
    size_t strIdx = 0;
    
    while ((str1[strIdx] != '\0') && (str2[strIdx] != '\0')) {
        if (str1[strIdx] != str2[strIdx]) {
            return 1;
        }

        strIdx++;
    }

    return 0;
}

size_t CountFileSize (int fd) {
	assert(fd != -1);
	
	struct stat fileStat = {};
    fstat(fd, &fileStat);
	
	return fileStat.st_size;
}

void ScanIn(float* scannedValue) {
    while (scanf("%g", scannedValue) != 1) {
        printf("Please enter correct value\n");
        fflush(stdin);
    }
}

void DetermiteOutputFile(int32_t* argc, char *argv[], char** outputFile) {
	assert(argc != nullptr);
	assert(argv != nullptr);
    assert(*argc > 1);

    *outputFile = (char*)calloc(MAX_FILE_SIZE, sizeof(**outputFile));

    for (int32_t curArgument = 1; curArgument < *argc; curArgument++) {
        if (!strcmp(argv[curArgument], "-o")) {
            strcat(*outputFile, argv[curArgument + 1]);
            *argc = curArgument;

            return;
        }
    }

    strcat(*outputFile, "b.txt");
}

size_t strLenWithoutSpaces(char* countingString) {
    size_t counter = 0;
    for (size_t curChar = 0; countingString[curChar] != '\0'; curChar++) {
        if (countingString[curChar] != ' ') counter++;
    }

    return counter;
}
