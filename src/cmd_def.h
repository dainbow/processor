#define IF_JUMP_(symbol)                                    \
    StackElem secondOne = StackPop(stack);                  \
    StackElem firstOne = StackPop(stack);                   \
    printf("First is %d, second is %d\n", firstOne, secondOne); \
                                                            \
    if (firstOne symbol secondOne) {                        \
        commandPointer = SIGNATURE_SIZE + argumentValue;    \
        printf("MOVING TO %u\n", commandPointer);           \
        break;                                              \
    }


DEF_CMD_(push, 1, ((comArgs.argFlags.bytes & LABEL_FLAG) || !(comArgs.argFlags.bytes & (MEM_FLAG | REG_FLAG | CONST_FLAG))),
    if (commands->buffer[commandPointer + 1] & (MEM_FLAG << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;

        StackPush(stack, *(StackElem*)(stack->memory + argumentValue));
    }
    else {
        StackPush(stack, argumentValue);
    }
    
    printf("I'M IN PUSH\n");
)

DEF_CMD_(pop, 5, (comArgs.argFlags.bytes & LABEL_FLAG) || (!((!(comArgs.argFlags.bytes & CONST_FLAG)) | (comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG))) || (!((comArgs.argFlags.bytes & CONST_FLAG) | !(comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG))) || (!((comArgs.argFlags.bytes & MEM_FLAG) | (!(comArgs.argFlags.bytes & REG_FLAG)) | (!(comArgs.argFlags.bytes & CONST_FLAG)))), 
    if (commands->buffer[commandPointer + 1] & (MEM_FLAG << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;
        
        *(StackElem*)(stack->memory + argumentValue) = StackPop(stack);
    }
    else if (commands->buffer[commandPointer + 1] & (REG_FLAG << SHIFT_OF_FLAGS)) {
        stack->registers[commands->buffer[commandPointer + 1] & REG_NUM_MASK] = StackPop(stack);
    }
    else
        StackPop(stack);
)

DEF_CMD_(add, 8, 1,  
    StackAdd(stack);
)

DEF_CMD_(jump, 9, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    commandPointer = SIGNATURE_SIZE + argumentValue;
    break;
)

DEF_CMD_(sub, 12, 1,  
    StackSub(stack);
)

DEF_CMD_(in, 13, (comArgs.argFlags.bytes & LABEL_FLAG) || (!((!(comArgs.argFlags.bytes & CONST_FLAG)) | (comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG))) || (!((comArgs.argFlags.bytes & CONST_FLAG) | !(comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG))) || (!((comArgs.argFlags.bytes & MEM_FLAG) | (!(comArgs.argFlags.bytes & REG_FLAG)) | (!(comArgs.argFlags.bytes & CONST_FLAG)))), 
    float scannedValue = 0;
    
    if (commands->buffer[commandPointer + 1] & (MEM_FLAG << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;

        printf("Please enter value to put in %u adress of RAM\n", argumentValue);
        ScanIn(&scannedValue);

        *(int32_t*)(stack->memory + argumentValue) = (int32_t)(scannedValue * ACCURACY);
        printf("I'VE WROTE %d to MEMORY\n", *(int32_t*)(stack->memory + argumentValue));
    }
    else if (commands->buffer[commandPointer + 1] & (REG_FLAG << SHIFT_OF_FLAGS)) {
        printf("Please enter value to put in %d register\n", commands->buffer[commandPointer + 1] & REG_NUM_MASK);
        ScanIn(&scannedValue);

        stack->registers[commands->buffer[commandPointer + 1] & REG_NUM_MASK] = (int32_t)(scannedValue * ACCURACY);
    }
    else {
        printf("Please enter value to put in stack\n");
        ScanIn(&scannedValue);

        StackPush(stack, (StackElem)(scannedValue * ACCURACY));
    }
)

DEF_CMD_(mul, 16, 1, 
    StackMul(stack);
)

DEF_CMD_(ja, 17, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    IF_JUMP_(>)
)

DEF_CMD_(div, 20, 1, 
    StackDiv(stack);
)

DEF_CMD_(jae, 21, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    IF_JUMP_(>=)
)

DEF_CMD_(out, 24, 1, 
    StackOut(stack);
)

DEF_CMD_(jb, 25, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    IF_JUMP_(<)
)

DEF_CMD_(dump, 28, 1, 
    StackDump(stack LOCATION());
)

DEF_CMD_(jbe, 29, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    IF_JUMP_(<=)
)

DEF_CMD_(exeDump, 32, 1, 
    StackExeDump(commands->buffer, commands->bufSize, commandPointer);
)

DEF_CMD_(je, 33, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    IF_JUMP_(==)
)

DEF_CMD_(jne, 37, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    IF_JUMP_(!=)
)

DEF_CMD_(hlt, 0, 1, 
    
)

#undef IF_JUMP_