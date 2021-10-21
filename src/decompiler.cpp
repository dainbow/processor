#include "Utilities.h"
#include "Text.h"
#include "commands.h"
#include "decompiler.h"

int main(int argc, char* argv[]) {
    char* outputFile = 0;
    DetermiteOutputFile(&argc, argv, &outputFile);

    Text input = {};
    ReadTextFromFile(&input, argv[1]);
    Decompile(&input, outputFile);
    Decompile(&input, outputFile);

    printf("Decompiling... %d", input.buffer[0]);
}

void Decompile(Text* input, char* outputfile) {
    FILE* output = fopen(outputfile, "wb");

    static uint32_t NUM_OF_DECOMPILING = 1;

    static Labels labels = {};
    
    if (NUM_OF_DECOMPILING > 1) {
        labels.isAllDataRead = 1;
    }

    for (uint32_t commandPointer = SIGNATURE_SIZE; commandPointer < input->bufSize; ) {
        switch (input->buffer[commandPointer]) {
            #include "cmd_def.h"
            default:
                printf("INVALID COMMAND ON %u COMMAND POINTER", commandPointer);
                abort();
                break;
        }
    }

    fclose(output);
    NUM_OF_DECOMPILING++;
}

int32_t FindLabelByCmdPtr(int32_t cmdPtr, Labels* labels) {
    for (uint32_t curLbl = 0; (labels->array[curLbl].go != -1) && (curLbl < MAX_LABEL_AMOUNT); curLbl++) {
        if(cmdPtr == labels->array[curLbl].go) {
            printf("Label %s goes to %d ip(Its number's %u)\n", labels->array[curLbl].name, labels->array[curLbl].go, curLbl);

            return curLbl;
        }
    }

    return -1;
}
