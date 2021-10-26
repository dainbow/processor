#include "Processor.h"

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, cmdCode)                                                                    \
case CMD_##cmdName: {                                                                                                       \
    uint32_t sizeOfArguments = 0;                                                                                           \
    StackElem argumentValue  = 0;                                                                                           \
    if (cmdNum % MAX_COMMAND_TYPES % 2) {                                                                                   \
        sizeOfArguments += BYTE_OF_ARGS;                                                                                    \
        if (commands->buffer[commandPointer + 1] & ((1 << CONST_SHIFT << SHIFT_OF_FLAGS) | (1 << LABEL_SHIFT << SHIFT_OF_FLAGS))) {     \
            sizeOfArguments += CONST_ARGUMENT_SIZE;                                                                         \
            argumentValue   += *(StackElem*)(commands->buffer + commandPointer + 2);                                        \
        }                                                                                                                   \
                                                                                                                            \
        if (commands->buffer[commandPointer + 1] & (1 << REG_SHIFT << SHIFT_OF_FLAGS)) {                                    \
            argumentValue += procStack->regs[commands->buffer[commandPointer + 1] & REG_NUM_MASK];                          \
        }                                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
    cmdCode                                                                                                                 \
                                                                                                                            \
    commandPointer += COMMAND_SIZE + sizeOfArguments;                                                                       \
    break;                                                                                                                  \
}

int main(int argc, char* argv[]) {
    StackCtor(procStack);
    StackCtor(retStack);   //Stack for return positions of functions

    Text commands = {};
    ReadCommands(argc, argv, &commands);
    if (ExecuteCommands(&commands, &procStack, &retStack) == 0) {
        fprintf(stderr, "ERROR IN EXECUTION\n");
        abort();
    }

    StackDtor(&procStack);
    StackDtor(&retStack);
    printf("OK\n");
}

void ReadCommands(int argc, char* argv[], Text* text) {
    assert(argc > 1 && "PLEASE WRITE AT LEAST 1 COMPILED FILE");
    assert(text != nullptr);

    ReadTextFromFile(text, argv[1]);
    ValidateSignature(text);
}

bool ExecuteCommands(Text* commands, Stack* procStack, Stack* retStack) {
    assert(commands  != nullptr);
    assert(procStack != nullptr);
    assert(retStack  != nullptr);

    procStack->memory        = (int8_t*)calloc(MAX_MEMORY_SIZE, sizeof(procStack->memory[0]));
    StackElem commandPointer = SIGNATURE_SIZE;

    while(commands->buffer[commandPointer] != 0) {
        switch(commands->buffer[commandPointer]) {
            #include "cmd_def.h"

            default:
                printf("UNKNOWN COMMAND %d ON %d POINTER PLACE\n", commands->buffer[commandPointer], commandPointer);
                abort();
                return 0;
        }
    }

    return 1;
    #undef DEF_CMD_
}

void ValidateSignature(Text* text) {
    assert(text != nullptr);

    if (MyStrCmp((const int8_t*)text->buffer, (const int8_t*)SIGNATURE) != 0) {
        assert(FAIL && "INVALID SIGNATURE, PLEASE CHECK BUILD NUMBER OR MAKER OF FILE");
    }
}
