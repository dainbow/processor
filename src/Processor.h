#pragma once

#include <io.h>
#include <fcntl.h>

#include "stack.h"
#include "Utilities.h"
#include "Text.h"
#include "commands.h"

void ReadCommands(int argc, char* argv[], Text* text);
void ExecuteCommands(Text* commands, Stack* stack);
void ValidateSignature(Text* text);

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, cmdCode)                                \
case CMD_##cmdName: {                                                                   \
    uint32_t sizeOfArguments = 0, argumentValue = 0;                                     \
    if (cmdNum % MAX_ARGUMENT_AMOUNT) {                                                 \
        sizeOfArguments += BYTE_OF_ARGS;                                                \
        if (commands->buffer[commandPointer + 1] & ((CONST_FLAG << SHIFT_OF_FLAGS) | (LABEL_FLAG << SHIFT_OF_FLAGS))) {    \
            sizeOfArguments += CONST_ARGUMENT_SIZE;                                     \
            argumentValue += *(int32_t*)(commands->buffer + commandPointer + 2);        \
        }                                                                               \
                                                                                        \
        if (commands->buffer[commandPointer + 1] & (REG_FLAG << SHIFT_OF_FLAGS)) {      \
            argumentValue += stack->registers[commands->buffer[commandPointer + 1] & REG_NUM_MASK]; \
        }                                                                               \
    }                                                                                   \
    printf("EXECUTE %s WITH ARGUMENT %u AND MOVING RIGHT TO %u\n", #cmdName, argumentValue, sizeOfArguments);                                                                      \
    cmdCode                                                                             \
                                                                                        \
    commandPointer += COMMAND_SIZE + sizeOfArguments;                                   \
    break;                                                                              \
}                                                                                       
