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

void ParseBrackets(   const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseRegister(   const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseConst(      const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseSeveralArgs(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseLabel(      const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);
void ParseString(     const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs);

void ImmitCommand(int32_t cmdNum, CompileResult* output);
void ImmitArgs(   int32_t cmdNum, CompileResult* output, Arguments* comArgs, Labels* labels, String* currentString);

const char* ShiftAndCheckArgs(String* string);

bool IfLabel(String* string, Labels* labels, size_t curCommandPointer);
StackElem FindLabelByName(char lblName[], Labels* labels);

bool PushArgsFilter(Flags argFlags);
bool DbArgsFilter(  Flags argFlags);
bool PopArgsFilter( Flags argFlags);
bool JumpArgsFilter(Flags argFlags);
bool NoArgsFilter  (Flags argFlags);
