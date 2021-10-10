#include <io.h>
#include <fcntl.h>

#include "stack.h"
#include "Utilities.h"

const uint32_t SIGNATURE_SIZE = 4;

char* ReadCommands(char* inputFile);
void ExecuteCommands(char* commands, Stack* stack);

int main(int argc, char* argv[]) {
    StackCtor(procStack);

    char* commands = ReadCommands(argv[1]);
    ExecuteCommands(commands, &procStack);



    printf("OK\n");
}

char* ReadCommands(char* inputFile) {
    assert(inputFile != nullptr);

    int inputd = open(inputFile, O_BINARY | O_RDONLY);  
    char* commands = (char*)calloc(CountFileSize(inputd), sizeof(commands[0]));

    printf("I read %d bytes from %s\n", read(inputd, commands, CountFileSize(inputd)), inputFile);
    
    return commands;
}

void ExecuteCommands(char* commands, Stack* stack) {
    assert(commands != nullptr);

    uint32_t commandPointer = SIGNATURE_SIZE;

    while(commands[commandPointer] != 0) {
        switch(commands[commandPointer]) { //TODO Ебануть красивый дефайн, используя вынесенный в хедер массив инструкций  из компиляции
            case 1:
                StackPush(stack, *(int32_t*)(&commands[commandPointer] + 1));

                commandPointer += 5;
                break;
            case 4:
                StackPop(stack);

                commandPointer += 1;
                break;
            case 8:
                StackAdd(stack);

                commandPointer += 1;
                break;
            case 12:
                StackSub(stack);

                commandPointer += 1;
                break;
            case 16:
                StackMul(stack);

                commandPointer += 1;
                break;
            case 20:
                StackDiv(stack);

                commandPointer += 1;
                break;
            case 24:
                StackOut(stack);

                commandPointer += 1;
                break;
            case 28:
                StackDump(stack LOCATION()); //TODO Сделать красивый дамп массива

                commandPointer += 1;
                break;
            default:
                printf("UNKNOW INSTRUCTION\n");
                break;
        }
    }
}