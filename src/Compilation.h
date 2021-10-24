#pragma once

#include <stdlib.h>

#include "Text.h"
#include "Utilities.h"
#include "commands.h"
#include "stack.h"

struct CompileResult {
    size_t bytesCount;
    uint8_t* bytesArray;
};

struct Command {
    const char* name;
    uint32_t length;
    uint32_t number;
};

struct Flags {
    uint32_t bytes: 5;
};

struct Arguments {
    Flags argFlags;
    StackElem argConst;
    int32_t argReg;
    char labelName[MAX_LABEL_NAME];
    char stringName[MAX_STRING_NAME];
};


void Compile(Text* text, const char* outName);
void ParseArgs(String* string, Arguments* comArg, bool isLabel, bool isString);
bool ProcessCommands(const Command* curCommand, String* curString, CompileResult* output);

void ParseBrackets(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseRegister(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseConst(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseSeveralArgs(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseLabel(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseString(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);

const char* ShiftAndCheckArgs(String* string);

bool IfLabel(String* string, Labels* labels, size_t curCommandPointer);
StackElem FindLabelByName(char lblName[], Labels* labels);

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, ...)                                                                                                \
if (((currentString.firstSpaceIdx == strlen(#cmdName)) | (currentString.length == strlen(#cmdName)))                                                \
& !MyStrCmp((const int8_t*)currentString.value, (const int8_t*)#cmdName)) {                                                                         \
    printf("Command name: %s.\n", #cmdName);                                                                                                        \
    if (cmdNum % MAX_COMMAND_TYPES < 2) {                                                                                                           \
    *(output.bytesArray + output.bytesCount) = (int8_t)cmdNum;                                                                                      \
    output.bytesCount += COMMAND_SIZE;                                                                                                              \
    }                                                                                                                                               \
                                                                                                                                                    \
    if((cmdNum % MAX_COMMAND_TYPES) % 2) {                                                                                                          \
        comArgs.argFlags.bytes |= LABEL_FLAG;                                                                                                       \
        bool isLabel = !(cmdArgFilter);                                                                                                             \
        comArgs.argFlags.bytes = 0;                                                                                                                 \
                                                                                                                                                    \
        comArgs.argFlags.bytes |= STRING_FLAG;                                                                                                      \
        bool isString = !(cmdArgFilter);                                                                                                            \
        comArgs.argFlags.bytes = 0;                                                                                                                 \
                                                                                                                                                    \
        ParseArgs(&currentString, &comArgs, isLabel, isString);                                                                                     \
                                                                                                                                                    \
        printf("Arg? %d It is %d\nReg?%d It is %d\nIs to RAM? %d\nLabel is %s\nString is %s\n", comArgs.argFlags.bytes & CONST_FLAG, comArgs.argConst, comArgs.argFlags.bytes & REG_FLAG, comArgs.argReg, comArgs.argFlags.bytes & MEM_FLAG, (comArgs.argFlags.bytes & LABEL_FLAG) ? comArgs.labelName : "NOLABEL", (comArgs.argFlags.bytes & STRING_FLAG) ? comArgs.stringName : "NOSTRING");        \
        if (cmdArgFilter) {                                                                                                                         \
            printf("INCORRECT ARGUMENT FOR %s COMMAND AT %u LINE", #cmdName, curString);                                                            \
            abort();                                                                                                                                \
        }                                                                                                                                           \
                                                                                                                                                    \
        printf("Bytes: %u\n", comArgs.argFlags.bytes);                                                                                              \
                                                                                                                                                    \
        if (cmdNum % MAX_COMMAND_TYPES < 2) {                                                                                                       \
            *(output.bytesArray + output.bytesCount) = (uint8_t)(comArgs.argFlags.bytes << SHIFT_OF_FLAGS | comArgs.argReg);                        \
            output.bytesCount += BYTE_OF_ARGS;                                                                                                      \
        }                                                                                                                                           \
                                                                                                                                                    \
        if (comArgs.argFlags.bytes & CONST_FLAG) {                                                                                                  \
            *(StackElem*)(output.bytesArray + output.bytesCount) = comArgs.argConst;                                                                \
            output.bytesCount += CONST_ARGUMENT_SIZE;                                                                                               \
        }                                                                                                                                           \
                                                                                                                                                    \
        if (comArgs.argFlags.bytes & LABEL_FLAG) {                                                                                                  \
            *(StackElem*)(output.bytesArray + output.bytesCount) = (labels.isAllDataRead) ? FindLabelByName(comArgs.labelName, &labels) : 0;        \
            output.bytesCount += CONST_ARGUMENT_SIZE;                                                                                               \
        }                                                                                                                                           \
                                                                                                                                                    \
        if (comArgs.argFlags.bytes & STRING_FLAG) {                                                                                                 \
            *(output.bytesArray + output.bytesCount) = STRING_DIVIDER;                                                                              \
            output.bytesCount += STRING_DIVIDER_SIZE;                                                                                               \
                                                                                                                                                    \
            for (size_t curChar = 0; curChar < currentString.lenOfArgs - 2*STRING_DIVIDER_SIZE - 2*QUOTE_SIZE; curChar++) {                         \
                *(output.bytesArray + output.bytesCount) = comArgs.stringName[curChar];                                                             \
                output.bytesCount++;                                                                                                                \
            }                                                                                                                                       \
                                                                                                                                                    \
            *(output.bytesArray + output.bytesCount) = STRING_DIVIDER;                                                                              \
            output.bytesCount += STRING_DIVIDER_SIZE;                                                                                               \
        }                                                                                                                                           \
                                                                                                                                                    \
    }                                                                                                                                               \
    printf("CommandPointer is %llu\n", output.bytesCount);                                                                                          \
}                                                                                                                                                   \
else                                                                                                                                                \

