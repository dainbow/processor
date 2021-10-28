#include "Utilities.h"
#include "Text.h"
#include "commands.h"
#include "decompiler.h"
#include "stack.h"

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

    if (labels.isAllDataRead == 0) {
        FillLabelsPoison(&labels);
    }

    for (uint32_t commandPointer = SIGNATURE_SIZE; commandPointer < input->bufSize; ) {
        printf("Now decompiling %u command\n", commandPointer);
        switch (input->buffer[commandPointer]) {
            #include "cmd_def.h"
            case 36: { //! $ That points on the beginning of string storing
                int32_t foundedNum = 0;
                if ((labels.isAllDataRead == 1) && (foundedNum = FindLabelByCmdPtr(commandPointer - SIGNATURE_SIZE, &labels)) != -1) {      
                    fprintf(output, "%s:\n", labels.array[foundedNum].decName);                                                                
                }

                commandPointer++;
                char curChar = 0;
                fprintf(output, "db $");

                while ((curChar = input->buffer[commandPointer]) != '$') {
                    fputc(curChar, output);
                    commandPointer++;
                }
                fputc('$', output); 
                fputc('\n', output);   

                if (commandPointer != input->bufSize) {
                    commandPointer++;
                }

                break;
            }
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
        printf("%u label goes to %I64d\n", curLbl, labels->array[curLbl].go);
        
        if(cmdPtr == labels->array[curLbl].go) {
            printf("Label %s goes to %I64d ip(Its number's %u)\n", labels->array[curLbl].decName, labels->array[curLbl].go, curLbl);

            return curLbl;
        }
    }

    return -1;
}
