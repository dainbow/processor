#include "commands.h"
#include "Processor.h"

int main(int argc, char* argv[]) {
    StackCtor(procStack);

    Text commands = {};
    ReadCommands(argc, argv, &commands);
    ExecuteCommands(&commands, &procStack);

    printf("OK\n");
}

void ReadCommands(int argc, char* argv[], Text* text) {
    assert(argc > 1 && "PLEASE WRITE AT LEAST 1 COMPILED FILE");

    ReadTextFromFile(text, argv[1]);
    ValidateSignature(text);
}

void ExecuteCommands(Text* commands, Stack* stack) {
    assert(commands != nullptr);

    uint32_t commandPointer = SIGNATURE_SIZE;

    while(commands->buffer[commandPointer] != 0) {
        switch(commands->buffer[commandPointer]) {
            #include "cmd_def.h"

            default:
                printf("UNKNOWN COMMAND %d ON %u POINTER PLACE\n", commands->buffer[commandPointer], commandPointer);
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