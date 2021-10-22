#pragma once

#include <stdlib.h>

#include "Text.h"
#include "Utilities.h"
#include "commands.h"
#include "stack.h"

struct CompileResult {
    int32_t bytesCount;
    uint8_t* bytesArray;
};

struct Command {
    const char* name;
    uint32_t length;
    uint32_t number;
};

struct Flags {
    uint32_t bytes: 4;
};

struct Arguments {
    Flags argFlags;
    StackElem argConst;
    int32_t argReg;
    char labelName[MAX_LABEL_NAME];
};


void Compile(Text* text, const char* outName);
void ParseArgs(String* string, Arguments* comArg, bool isLabel);
bool ProcessCommands(const Command* curCommand, String* curString, CompileResult* output);

void ParseBrackets(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseRegister(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseConst(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseSeveralArgs(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseLabel(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);

const char* ShiftAndCheckArgs(String* string);

bool IfLabel(String* string, Labels* labels, int32_t curCommandPointer);
StackElem FindLabelByName(char lblName[], Labels* labels);

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, ...)                                                                                                \
if (((currentString.firstSpaceIdx == strlen(#cmdName)) | (currentString.length == strlen(#cmdName)))                                                \
& !MyStrCmp((const int8_t*)currentString.value, (const int8_t*)#cmdName)) {                                                                         \
    printf("Command name: %s.\n", #cmdName);                                                                                                        \
                                                                                                                                                    \
    *(output.bytesArray + output.bytesCount) = (int8_t)cmdNum;                                                                                     \
    output.bytesCount += COMMAND_SIZE;                                                                                                              \
    if(cmdNum % MAX_ARGUMENT_AMOUNT) {                                                                                                                                \
        comArgs.argFlags.bytes |= LABEL_FLAG;                                                                                                       \
        ParseArgs(&currentString, &comArgs, !(cmdArgFilter));                                                                                       \
                                                                                                                                                    \
        printf("Arg? %d It is %d\nReg?%d It is %d\nIs to RAM? %d\nLabel is %s\n", comArgs.argFlags.bytes & CONST_FLAG, comArgs.argConst, comArgs.argFlags.bytes & REG_FLAG, comArgs.argReg, comArgs.argFlags.bytes & MEM_FLAG, (comArgs.argFlags.bytes & LABEL_FLAG) ? comArgs.labelName : "0");        \
        if (cmdArgFilter) {                                                                                                                         \
            printf("INCORRECT ARGUMENT FOR %s COMMAND AT %u LINE", #cmdName, curString);                                                            \
            abort();                                                                                                                                \
        }                                                                                                                                           \
                                                                                                                                                    \
        printf("Bytes: %u\n", comArgs.argFlags.bytes);                                                                                              \
                                                                                                                                                    \
        for (uint32_t curArg = 0; curArg < cmdNum % 4; curArg++) {                                                                                  \
            *(output.bytesArray + output.bytesCount) = (uint8_t)(comArgs.argFlags.bytes << SHIFT_OF_FLAGS | comArgs.argReg);                        \
            output.bytesCount += BYTE_OF_ARGS;                                                                                                      \
                                                                                                                                                    \
            if (comArgs.argFlags.bytes & CONST_FLAG) {                                                                                              \
                *(StackElem*)(output.bytesArray + output.bytesCount) = comArgs.argConst;                                                             \
                output.bytesCount += CONST_ARGUMENT_SIZE;                                                                                           \
            }                                                                                                                                       \
                                                                                                                                                    \
            if (comArgs.argFlags.bytes & LABEL_FLAG) {                                                                                              \
                *(StackElem*)(output.bytesArray + output.bytesCount) = (labels.isAllDataRead) ? FindLabelByName(comArgs.labelName, &labels) : 0;     \
                output.bytesCount += CONST_ARGUMENT_SIZE;                                                                                           \
            }                                                                                                                                       \
        }                                                                                                                                           \
    }                                                                                                                                               \
    printf("CommandPointer is %d\n", output.bytesCount);                                                                                            \
}                                                                                                                                                   \
else                                                                                                                                                \

