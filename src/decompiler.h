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
        DecompileArgs decArgs = {};                                                                                                 \
                                                                                                                                    \
        if (cmdNum & ARGUMENT_TYPE_MASK) {                                                                                          \
            decArgs.bytesOfArgs = input->buffer[commandPointer + 1];                                                                \
            decArgs.sizeOfArguments += BYTE_OF_ARGS;                                                                                \
            decArgs.regName[0] = (char)((decArgs.bytesOfArgs & REG_NUM_MASK) + 'a');                                                \
            decArgs.regName[1] = 'x';                                                                                               \
                                                                                                                                    \
            if (decArgs.bytesOfArgs & ((1 << CONST_SHIFT << SHIFT_OF_FLAGS) | (1 << LABEL_SHIFT << SHIFT_OF_FLAGS))) {              \
                if ((decArgs.bytesOfArgs & (1 << LABEL_SHIFT << SHIFT_OF_FLAGS)) && (labels.isAllDataRead == 0)) {                  \
                    labels.array[labels.curLbl].go = *((StackElem*)(input->buffer + commandPointer + 2));                           \
                    labels.curLbl++;                                                                                                \
                }                                                                                                                   \
                else if ((decArgs.bytesOfArgs & (1 << LABEL_SHIFT << SHIFT_OF_FLAGS)) && (labels.isAllDataRead == 1)) {             \
                    int32_t foundedNum = 0;                                                                                         \
                    if ((foundedNum = FindLabelByCmdPtr(*(StackElem*)(input->buffer + commandPointer + 2), &labels)) != -1) {       \
                        sprintf(decArgs.numberToString, "l%I64d", labels.array[foundedNum].go);                                     \
                    }                                                                                                               \
                    else {                                                                                                          \
                        assert(FAIL && "LABEL NOT FOUND");                                                                          \
                    }                                                                                                               \
                }                                                                                                                   \
                else                                                                                                                \
                    sprintf(decArgs.numberToString, "%lf", ((double)(*((StackElem*)(input->buffer + commandPointer + 2)))) / ACCURACY); \
                decArgs.sizeOfArguments += CONST_ARGUMENT_SIZE;                                                                     \
            }                                                                                                                       \
        }                                                                                                                           \
                                                                                                                                    \
        int32_t foundedNum = 0;                                                                                                     \
        if ((labels.isAllDataRead == 1) && ((foundedNum = FindLabelByCmdPtr(commandPointer - SIGNATURE_SIZE, &labels)) != -1)) {    \
            fprintf(output, "l%I64d:\n", labels.array[foundedNum].go);                                                              \
        }                                                                                                                           \
                                                                                                                                    \
        fprintf(output, #cmdName " ");                                                                                              \
        fprintf(output, "%s%s %s %s%s\n",                                                                                           \
        (decArgs.bytesOfArgs & (1 << MEM_SHIFT << SHIFT_OF_FLAGS)) ? "{" : "",                                                                      \
        (decArgs.bytesOfArgs & (1 << REG_SHIFT << SHIFT_OF_FLAGS)) ? decArgs.regName : "",                                                          \
        ((decArgs.bytesOfArgs & (1 << REG_SHIFT << SHIFT_OF_FLAGS)) && (decArgs.bytesOfArgs & (1 << CONST_SHIFT << SHIFT_OF_FLAGS))) ? "+" : "",    \
        (decArgs.bytesOfArgs & ((1 << CONST_SHIFT << SHIFT_OF_FLAGS) | (1 << LABEL_SHIFT << SHIFT_OF_FLAGS))) ? decArgs.numberToString : "",        \
        (decArgs.bytesOfArgs & (1 << MEM_SHIFT << SHIFT_OF_FLAGS)) ? "}" : "");                                                                     \
                                                                                                                                                    \
        commandPointer += COMMAND_SIZE + decArgs.sizeOfArguments;                                                                                   \
        break;                                                                                                                                      \
    }                                                                                                                                               \


#endif