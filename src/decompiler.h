#ifndef _DECOMPILER_H_
#define _DECOMPILER_H_

struct DecompileArgs {
    uint32_t sizeOfArguments;                                                                                               
    int32_t bytesOfArgs;                                                                                                    
    char regName[MAX_REG_NAME];                                                                                            
    char numberToString[MAX_NUMBER_SIZE];                                                                                  
};

void Decompile(Text* input, char* outputfile);

int32_t FindLabelByCmdPtr(int32_t cmdPtr, Labels* labels);

#define DEF_CMD_(cmdName, cmdNum, ...)                                                                                              \
    case CMD_##cmdName: {                                                                                                           \
        if (strcmp(#cmdName, "jump") == 0) {                                                                                        \
            printf("I M IN JUMP WITH %d BYTES\n", input->buffer[commandPointer + 1] & (LABEL_FLAG << SHIFT_OF_FLAGS));              \
        }                                                                                                                           \
        DecompileArgs decArgs = {};                                                                                                 \
                                                                                                                                    \
        if (cmdNum % MAX_ARGUMENT_AMOUNT) {                                                                                         \
            decArgs.bytesOfArgs = input->buffer[commandPointer + 1];                                                                \
            decArgs.sizeOfArguments += BYTE_OF_ARGS;                                                                                \
            decArgs.regName[0] = (char)((decArgs.bytesOfArgs & REG_NUM_MASK) + 'a');                                                \
            decArgs.regName[1] = 'x';                                                                                               \
                                                                                                                                    \
            if (decArgs.bytesOfArgs & ((CONST_FLAG << SHIFT_OF_FLAGS) | (LABEL_FLAG << SHIFT_OF_FLAGS))) {                          \
                if ((decArgs.bytesOfArgs & (LABEL_FLAG << SHIFT_OF_FLAGS)) && (labels.isAllDataRead == 0)) {                        \
                    printf("TRYING TO READ LABEL AFTER JUMP\n");                                                                    \
                    labels.array[labels.curLbl].go = *((int32_t*)(input->buffer + commandPointer + 2));                             \
                    sprintf(labels.array[labels.curLbl].name, "l%u", labels.curLbl + 1);                                            \
                    labels.curLbl++;                                                                                                \
                }                                                                                                                   \
                else if ((decArgs.bytesOfArgs & (LABEL_FLAG << SHIFT_OF_FLAGS)) && (labels.isAllDataRead == 1)) {                   \
                    int32_t foundedNum = 0;                                                                                         \
                    if ((foundedNum = FindLabelByCmdPtr(*(int32_t*)(input->buffer + commandPointer + 2), &labels)) != -1) {         \
                        sprintf(decArgs.numberToString, "%s", labels.array[foundedNum].name);                                       \
                    }                                                                                                               \
                    else {                                                                                                          \
                        assert(FAIL && "LABEL NOT FOUND");                                                                          \
                    }                                                                                                               \
                }                                                                                                                   \
                else                                                                                                                \
                    sprintf(decArgs.numberToString, "%g", ((float)(*((int32_t*)(input->buffer + commandPointer + 2)))) / ACCURACY); \
                decArgs.sizeOfArguments += CONST_ARGUMENT_SIZE;                                                                     \
            }                                                                                                                       \
        }                                                                                                                           \
                                                                                                                                    \
        int32_t foundedNum = 0;                                                                                                     \
        if ((labels.isAllDataRead == 1) && (foundedNum = FindLabelByCmdPtr(commandPointer - SIGNATURE_SIZE, &labels)) != -1) {      \
            fprintf(output, "%s:\n", labels.array[foundedNum].name);                                                                \
        }                                                                                                                           \
                                                                                                                                    \
        fprintf(output, #cmdName " ");                                                                                              \
        fprintf(output, "%s%s %s %s%s\n",                                                                                           \
        (decArgs.bytesOfArgs & (MEM_FLAG << SHIFT_OF_FLAGS)) ? "{" : "",                                                            \
        (decArgs.bytesOfArgs & (REG_FLAG << SHIFT_OF_FLAGS)) ? decArgs.regName : "",                                                                \
        ((decArgs.bytesOfArgs & (REG_FLAG << SHIFT_OF_FLAGS)) && (decArgs.bytesOfArgs & (CONST_FLAG << SHIFT_OF_FLAGS))) ? "+" : "",                \
        (decArgs.bytesOfArgs & ((CONST_FLAG << SHIFT_OF_FLAGS) | (LABEL_FLAG << SHIFT_OF_FLAGS))) ? decArgs.numberToString : "",                    \
        (decArgs.bytesOfArgs & (MEM_FLAG << SHIFT_OF_FLAGS)) ? "}" : "");                                                                           \
                                                                                                                                                    \
        commandPointer += COMMAND_SIZE + decArgs.sizeOfArguments;                                                                                   \
        break;                                                                                                                                      \
    }                                                                                                                                               \


#endif