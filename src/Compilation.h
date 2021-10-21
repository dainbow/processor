#pragma once

#include <stdlib.h>

#include "Text.h"
#include "Utilities.h"
#include "commands.h"

const uint32_t MAX_LABEL_NAME = 100;
const uint32_t MAX_LABEL_AMOUNT = 70;

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
    int32_t argConst;
    int32_t argReg;
    char labelName[MAX_LABEL_NAME];
};

struct Label {
    char name[MAX_LABEL_NAME];
    int32_t go = -1;
};

struct Labels {
    Label array[MAX_LABEL_AMOUNT];
    uint32_t curLbl;
    bool isAllDataRead;
};

void ReadOutArgument(int32_t* argc, char *argv[], char** outputFile);
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
int32_t FindLabel(char lblName[], Labels* labels);