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
    uint32_t string   : 1;
    uint32_t label    : 1;
    uint32_t constant : 1;
    uint32_t reg      : 1;
    uint32_t mem      : 1;
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

void ImmitCommand(int32_t cmdNum, CompileResult* output);
void ImmitArgs(   int32_t cmdNum, CompileResult* output, Arguments* comArgs, Labels* labels, String* currentString);

const char* ShiftAndCheckArgs(String* string);

bool IfLabel(String* string, Labels* labels, size_t curCommandPointer);
StackElem FindLabelByName(char lblName[], Labels* labels);

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, ...)                                                                                                \
if (((currentString.firstSpaceIdx == strlen(#cmdName)) || (currentString.length == strlen(#cmdName)))                                               \
& !MyStrCmp((const int8_t*)currentString.value, (const int8_t*)#cmdName)) {                                                                         \
    printf("Command name: %s.\n", #cmdName);                                                                                                        \
    ImmitCommand(cmdNum, &output);                                                                                                                  \
                                                                                                                                                    \
    if((cmdNum % MAX_COMMAND_TYPES) % 2) {                                                                                                          \
        comArgs.argFlags.label = 1;                                                                                                       \
        bool isLabel = !(cmdArgFilter);                                                                                                             \
        comArgs.argFlags.label = 0;                                                                                                                 \
                                                                                                                                                    \
        comArgs.argFlags.string = 1;                                                                                                      \
        bool isString = !(cmdArgFilter);                                                                                                            \
        comArgs.argFlags.string = 0;                                                                                                                 \
                                                                                                                                                    \
        ParseArgs(&currentString, &comArgs, isLabel, isString);                                                                                     \
                                                                                                                                                    \
        printf("Arg? %u It is %d\nReg?%u It is %d\nIs to RAM? %u\nLabel is %s\nString is %s\n", comArgs.argFlags.constant, comArgs.argConst, comArgs.argFlags.reg, comArgs.argReg, comArgs.argFlags.mem, (comArgs.argFlags.label) ? comArgs.labelName : "NOLABEL", (comArgs.argFlags.string) ? comArgs.stringName : "NOSTRING");        \
        if (cmdArgFilter) {                                                                                                                         \
            fprintf(stderr, "INCORRECT ARGUMENT FOR %s COMMAND AT %u LINE", #cmdName, curString);                                                   \
            abort();                                                                                                                                \
        }                                                                                                                                           \
        ImmitArgs(cmdNum, &output, &comArgs, &labels, &currentString);                                                                              \
    }                                                                                                                                               \
    printf("CommandPointer is %llu\n", output.bytesCount);                                                                                          \
}                                                                                                                                                   \
else                                                                                                                                                \

