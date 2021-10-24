#include "Processor.h"

int main(int argc, char* argv[]) {
    StackCtor(procStack);
    StackCtor(retStack);   //Stack for return positions of functions

    Text commands = {};
    ReadCommands(argc, argv, &commands);
    ExecuteCommands(&commands, &procStack, &retStack);

    printf("OK\n");
}

void ReadCommands(int argc, char* argv[], Text* text) {
    assert(argc > 1 && "PLEASE WRITE AT LEAST 1 COMPILED FILE");
    assert(text != nullptr);

    ReadTextFromFile(text, argv[1]);
    ValidateSignature(text);
}

void ExecuteCommands(Text* commands, Stack* stack, Stack* retStack) {
    assert(commands != nullptr);
    assert(stack    != nullptr);
    assert(retStack != nullptr);

    stack->memory            = (int8_t*)calloc(MAX_MEMORY_SIZE, sizeof(stack->memory[0]));
    StackElem commandPointer = SIGNATURE_SIZE;

    while(commands->buffer[commandPointer] != 0) {
        switch(commands->buffer[commandPointer]) {
            #include "cmd_def.h"

            default:
                printf("UNKNOWN COMMAND %d ON %d POINTER PLACE\n", commands->buffer[commandPointer], commandPointer);
                abort();
                break;
        }
    }

    #undef DEF_CMD_
}

void ValidateSignature(Text* text) {
    assert(text != nullptr);

    if (MyStrCmp((const int8_t*)text->buffer, (const int8_t*)SIGNATURE) != 0) {
        assert(FAIL && "INVALID SIGNATURE, PLEASE CHECK BUILD NUMBER OR MAKER OF FILE");
    }
}