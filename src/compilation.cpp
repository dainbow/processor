#include "Compilation.h"

#define DEF_CMD_(cmdName, cmdNum, ...)                                                                                  \
if (((currentString.firstSpaceIdx == strlen(#cmdName)) | (currentString.length - 1 == strlen(#cmdName)))                \
& !MyStrCmp((const int8_t*)currentString.value, (const int8_t*)#cmdName)) {                                             \
    uint32_t* argIdxs = (uint32_t*)calloc(MAX_ARGUMENT_AMOUNT, sizeof(argIdxs[0]));                                     \
    if(ValidateDigitArgs(&currentString, cmdNum % 4, &argIdxs)) {                                                       \
        printf("Command name: %s.\n", #cmdName);                                                                        \
                                                                                                                        \
        *(output.bytesArray + output.bytesCount) = (uint8_t)cmdNum;                                                     \
        output.bytesCount += 1;                                                                                         \
                                                                                                                        \
        for (uint32_t curArg = 0; curArg < cmdNum % 4; curArg++) {                                                      \
            int32_t operationArg = (int32_t)(ACCURACY * atof((const char*)(currentString.value + argIdxs[curArg])));    \
            printf("Arg is %d\n", operationArg);                                                                        \
            *(uint32_t*)(output.bytesArray + output.bytesCount) = operationArg;                                         \
                                                                                                                        \
            output.bytesCount += ARGUMENT_SIZE;                                                                         \
            }                                                                                                           \
        }                                                                                                               \
    }                                                                                                                   \
else                                                                                                               
  

int main(int32_t argc, char** argv) {
    char* outputFile = 0;
    ReadOutArgument(&argc, argv, &outputFile);

    printf("%s\n", outputFile);

    for (int32_t curArgument = 1; curArgument < argc; curArgument++) {
        Text input = {};
        printf("Readed string: %s", argv[curArgument]);
    
        ReadTextFromFile(&input, argv[curArgument]);
        MakeStrings(&input);
        ProcessStrings(&input);
        Compile(&input, outputFile);  //TODO При одинакомых названиях в скобочках номер копии.
    }

    printf("OK");
    return 0;
}

void ReadOutArgument(int32_t* argc, char *argv[], char** outputFile) {
	assert(argc != nullptr);
	assert(argv != nullptr);

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

void Compile(Text* text, const char* outName) {
    assert(text    != nullptr);
    assert(outName != nullptr);

    CompileResult output = {0, (uint8_t*)calloc(text->bufSize + 4, sizeof(output.bytesArray[0]))};

    for(uint32_t curString = 0; curString < text->strAmount; curString++) {   
        String currentString = text->strings[curString];

        #include "cmd_def.h"    

        printf("UNKNOWN COMMAND ON %u LINE\n", curString); //!expands macro's else!
    }
    #undef DEF_CMD_

    FILE* outputd = fopen(outName, "wb");
    printf("I wrote %llu signature bytes\n",  fwrite("DAIN", 1, 4, outputd));
    printf("I wrote %llu bytes from array\n", fwrite(output.bytesArray, 1, output.bytesCount, outputd));

    fclose(outputd);
    free(output.bytesArray);
}

bool ValidateDigitArgs(String* string, uint32_t argAmount, uint32_t** argIdxs) {
    if(argAmount == 0) {
        return 1;
    }

    if (string->firstSpaceIdx == 0) {
        return 0;
    }

    bool spaceFlag = 0;
    uint32_t spacesAmount = 0;

    for(uint32_t curChar = string->firstSpaceIdx; curChar < string->length; curChar++) {
        if (string->value[curChar] != '\0') {
            if (string->value[curChar] == ' ') {
                spaceFlag = 1;
            }
            else if (spaceFlag) {
                (*argIdxs)[spacesAmount++] = curChar;
                spaceFlag = 0;

                if (!isdigit(string->value[curChar])) {
                    return 0;
                }
            }
        }
        else {
            break;
        }
    }

    if (spacesAmount != argAmount)
        return 0;
    return 1;
}