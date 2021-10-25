#pragma once

#include <TXLib.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>

#include "stack.h"
#include "Utilities.h"
#include "Text.h"
#include "commands.h"

void ReadCommands(int argc, char* argv[], Text* text);
bool ExecuteCommands(Text* commands, Stack* procStack, Stack* retStack);
void ValidateSignature(Text* text);

//This define calculates shift of command pointer, execute code and shifts

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, cmdCode)                                                                    \
case CMD_##cmdName: {                                                                                                       \
    uint32_t sizeOfArguments = 0;                                                                                           \
    StackElem argumentValue = 0;                                                                                            \
    if (cmdNum % MAX_COMMAND_TYPES % 2) {                                                                                   \
        sizeOfArguments += BYTE_OF_ARGS;                                                                                    \
        if (commands->buffer[commandPointer + 1] & ((1 << CONST_SHIFT << SHIFT_OF_FLAGS) | (1 << LABEL_SHIFT << SHIFT_OF_FLAGS))) {     \
            sizeOfArguments += CONST_ARGUMENT_SIZE;                                                                         \
            argumentValue += *(StackElem*)(commands->buffer + commandPointer + 2);                                          \
        }                                                                                                                   \
                                                                                                                            \
        if (commands->buffer[commandPointer + 1] & (1 << REG_SHIFT << SHIFT_OF_FLAGS)) {                                          \
            argumentValue += procStack->regs[commands->buffer[commandPointer + 1] & REG_NUM_MASK];                         \
        }                                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
    cmdCode                                                                                                                 \
                                                                                                                            \
    commandPointer += COMMAND_SIZE + sizeOfArguments;                                                                       \
    break;                                                                                                                  \
}                                                                                       

