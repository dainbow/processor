DEF_CMD_(push, 1, ((comArgs.argFlags.bytes & LABEL_FLAG) || !(comArgs.argFlags.bytes & (MEM_FLAG | REG_FLAG | CONST_FLAG))),
    if (commands->buffer[commandPointer + 1] & (MEM_FLAG << SHIFT_OF_FLAGS)) {
        StackPush(stack, stack->memory[argumentValue]);
    }
    else {
        StackPush(stack, argumentValue);
    }
    
    printf("I'M IN PUSH\n");
)

DEF_CMD_(pop, 5, (comArgs.argFlags.bytes & LABEL_FLAG) || (!((!(comArgs.argFlags.bytes & CONST_FLAG)) | (comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG))) || (!((comArgs.argFlags.bytes & CONST_FLAG) | !(comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG))) || (!((comArgs.argFlags.bytes & MEM_FLAG) | (!(comArgs.argFlags.bytes & REG_FLAG)) | (!(comArgs.argFlags.bytes & CONST_FLAG)))), 
    if (commands->buffer[commandPointer + 1] & (MEM_FLAG << SHIFT_OF_FLAGS)) {
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
        printf("Please enter value to put in %u adress of RAM\n", argumentValue);
        while (scanf("%g", &scannedValue) != 1) {
            printf("Please enter correct value\n");
            fflush(stdin);
        }

        *(int32_t*)(stack->memory + argumentValue) = (int32_t)(scannedValue * ACCURACY);
    }
    else if (commands->buffer[commandPointer + 1] & (REG_FLAG << SHIFT_OF_FLAGS)) {
        printf("Please enter value to put in %d register\n", commands->buffer[commandPointer + 1] & REG_NUM_MASK);
        while (scanf("%g", &scannedValue) != 1) {
            printf("Please enter correct value\n");
            fflush(stdin);
        }

        stack->registers[commands->buffer[commandPointer + 1] & REG_NUM_MASK] = (int32_t)(scannedValue * ACCURACY);
    }
    else {
        printf("Please enter value to put in stack\n");
        while (scanf("%g", &scannedValue) != 1) {
            printf("Please enter correct value\n");
            fflush(stdin);
        }

        StackPush(stack, (StackElem)(scannedValue * ACCURACY));
    }
)

DEF_CMD_(mul, 16, 1, 
    StackMul(stack);
)

DEF_CMD_(div, 20, 1, 
    StackDiv(stack);
)

DEF_CMD_(out, 24, 1, 
    StackOut(stack);
)

DEF_CMD_(dump, 28, 1, 
    StackDump(stack LOCATION());
)

DEF_CMD_(exeDump, 32, 1, 
    StackExeDump(commands->buffer, commands->bufSize, commandPointer);
)

DEF_CMD_(hlt, 0, 1, 
    
)
