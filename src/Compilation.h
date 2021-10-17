#pragma once

#include <stdlib.h>

#include "Text.h"
#include "Utilities.h"
#include "commands.h"

struct CompileResult {
    size_t bytesCount;
    uint8_t* bytesArray;
};

struct Command {
    const char* name;
    uint32_t length;
    uint32_t number;
};

void ReadOutArgument(int32_t* argc, char *argv[], char** outputFile);
void Compile(Text* text, const char* outName);
bool ValidateDigitArgs(String* string, uint32_t argAmount, uint32_t** argIdxs);
bool ProcessCommands(const Command* curCommand, String* curString, CompileResult* output);
