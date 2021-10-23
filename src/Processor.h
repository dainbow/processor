#pragma once

#include <TXLib.h>
#include <io.h>
#include <fcntl.h>

#include "stack.h"
#include "Utilities.h"
#include "Text.h"
#include "commands.h"

void ReadCommands(int argc, char* argv[], Text* text);
void ExecuteCommands(Text* commands, Stack* stack, Stack* retStack);
void ValidateSignature(Text* text);

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, cmdCode)                                                                    \
case CMD_##cmdName: {                                                                                                       \
    uint32_t sizeOfArguments = 0;                                                                                           \
    StackElem argumentValue = 0;                                                                                            \
    if (cmdNum % MAX_COMMAND_TYPES % 2) {                                                                                     \
        sizeOfArguments += BYTE_OF_ARGS;                                                                                    \
        if (commands->buffer[commandPointer + 1] & ((CONST_FLAG << SHIFT_OF_FLAGS) | (LABEL_FLAG << SHIFT_OF_FLAGS))) {     \
            sizeOfArguments += CONST_ARGUMENT_SIZE;                                                                         \
            argumentValue += *(StackElem*)(commands->buffer + commandPointer + 2);                                          \
        }                                                                                                                   \
                                                                                                                            \
        if (commands->buffer[commandPointer + 1] & (REG_FLAG << SHIFT_OF_FLAGS)) {                                          \
            argumentValue += stack->registers[commands->buffer[commandPointer + 1] & REG_NUM_MASK];                         \
        }                                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
    cmdCode                                                                                                                 \
                                                                                                                            \
    commandPointer += COMMAND_SIZE + sizeOfArguments;                                                                       \
    break;                                                                                                                  \
}                                                                                       

//printf("EXECUTE %s WITH ARGUMENT %d AND MOVING RIGHT TO %u\n", #cmdName, argumentValue, COMMAND_SIZE + sizeOfArguments);
