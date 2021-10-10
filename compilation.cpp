#include <stdlib.h>

#include "Text.h"
#include "Utilities.h"

struct CompileResult {
    size_t bytesCount;
    uint8_t* bytesArray;
};

struct Command {
    const char* name;
    uint32_t length;
    uint32_t number;
};

const uint32_t ARGUMENT_SIZE = 4; //size of each argument in bytes
const uint32_t MAX_ARGUMENT_AMOUNT = 4;
const uint32_t COMMANDS_AMOUNT = 9;
const uint32_t ACCURACY = 3;

const Command allCommands[9] = 
{{"push", 4, 1}, {"pop", 3, 4}, {"add", 3, 8}, {"sub", 3, 12}, {"mul", 3, 16}, {"div", 3, 20}, {"out", 3, 24}, {"dmp", 3, 28}, {"hlt", 3, 0}};

const uint8_t PUSH_LEN = 4;

char* ReadOutArgument(int32_t* argc, char *argv[]);
void Compile(Text* text, const char* outName);
bool ValidateDigitArgs(String* string, uint32_t argAmount, uint32_t** argIdxs);
bool ProcessCommands(const Command* curCommand, String* curString, CompileResult* output);

int main(int argc, char** argv) {
    char* outputFile = ReadOutArgument(&argc, argv);

    for (uint32_t curArgument = 1; curArgument < argc; curArgument++) {
        Text input = {};
        
        ReadTextFromFile(&input, argv[curArgument]);
        MakeStrings(&input);
        ProcessStrings(&input);
        Compile(&input, outputFile);
    }

    printf("OK");
    return 0;
}

char* ReadOutArgument(int32_t* argc, char *argv[]) {
	assert(argc != nullptr);
	assert(argv != nullptr);

    char* outputFile = 0;

    for (uint32_t curArgument = 1; curArgument < *argc; curArgument++) {
        if (!strcmp(argv[curArgument], "-o")) {
            outputFile = argv[curArgument + 1];
            *argc = curArgument;

            return outputFile;
        }
    }

    return "b.txt";
}

void Compile(Text* text, const char* outName) {
    assert(text    != nullptr);
    assert(outName != nullptr);

    CompileResult output = {0, (uint8_t*)calloc(text->bufSize + 4, sizeof(output.bytesArray[0]))};

    for(size_t curString = 0; curString < text->strAmount; curString++) {        
        for (size_t curCommand = 0; curCommand < COMMANDS_AMOUNT; curCommand++) {
            if (ProcessCommands(&allCommands[curCommand], &text->strings[curString], &output))
                break;

            if (curCommand == COMMANDS_AMOUNT - 1) {
                printf("INVALID COMMANDS IN %d LINE\n", curString + 1);
            }
        }   
    }

    int outputd = open(outName, O_WRONLY | O_BINARY | O_CREAT);
    printf("I wrote 4 signature bytes\n",  write(outputd, "DAIN", 4));
    printf("I wrote %d bytes from array\n", write(outputd, output.bytesArray, output.bytesCount));

    close(outputd);
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

    for(size_t curChar = string->firstSpaceIdx; curChar < string->length; curChar++) {
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
bool ProcessCommands(const Command* curCommand, String* curString, CompileResult* output) {
    int32_t operationArg = 0;

    if ((curString->firstSpaceIdx == curCommand->length | curString->length - 1 == curCommand->length)
        & !MyStrCmp((const int8_t*)curString->value, (const int8_t*)curCommand->name)) {
        uint32_t* argIdxs = (uint32_t*)calloc(MAX_ARGUMENT_AMOUNT, sizeof(argIdxs[0]));
               
        if(ValidateDigitArgs(curString, curCommand->number % 4, &argIdxs)) {
            printf("Command name%s\n", curCommand->name);

            *(output->bytesArray + output->bytesCount) = (uint8_t)curCommand->number;
            output->bytesCount += 1;
            
            for (uint32_t curArg = 0; curArg < curCommand->number % 4; curArg++) {
                operationArg = (int32_t)(ACCURACY * atof((const char*)(curString->value + argIdxs[curArg])));
                *(uint32_t*)(output->bytesArray + output->bytesCount) = operationArg;

                output->bytesCount += ARGUMENT_SIZE;
            }

            return 1;   
        }
    }

    return 0;
}