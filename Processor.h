#pragma once

#include <io.h>
#include <fcntl.h>

#include "stack.h"
#include "Utilities.h"
#include "Text.h"
#include "commands.h"

const uint32_t SIGNATURE_SIZE = 4;

void ReadCommands(int argc, char* argv[], Text* text);
void ExecuteCommands(Text* commands, Stack* stack);

#define DEF_CMD_(cmdName, cmdNum, cmdCode)                                              \
case CMD_##cmdName: {                                                                   \
    cmdCode                                                                             \
    commandPointer += COMMAND_SIZE + ARGUMENT_SIZE*(cmdNum % MAX_ARGUMENT_AMOUNT);      \
    break;                                                                              \
}                                                                                       
