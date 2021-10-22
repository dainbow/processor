#define IF_JUMP_(symbol)                                    \
    StackElem secondOne = StackPop(stack);                  \
    StackElem firstOne = StackPop(stack);                   \
                                                            \
    if (firstOne symbol secondOne) {                        \
        commandPointer = SIGNATURE_SIZE + argumentValue;    \
        break;                                              \
    }

//printf("First is %d, second is %d, symbol is %s\n", firstOne, secondOne, #symbol);
//printf("MOVING TO %d\n", commandPointer); 


DEF_CMD_(push, 1, ((comArgs.argFlags.bytes & LABEL_FLAG) || !(comArgs.argFlags.bytes & (MEM_FLAG | REG_FLAG | CONST_FLAG))),
    if (commands->buffer[commandPointer + 1] & (MEM_FLAG << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;

        StackPush(stack, *(StackElem*)(stack->memory + argumentValue));
    }
    else {
        StackPush(stack, argumentValue);
    }
)

DEF_CMD_(drwpc, 4, 1,
    int maxX = txGetExtentX(); 

    for (uint32_t curPixel = 0; *(stack->memory + BEGINNING_OF_GMEM + curPixel) != -1; curPixel += 3) {
        //printf("Drawing %u pixel with value %d\n", curPixel, *(stack->memory + BEGINNING_OF_GMEM + curPixel));

        COLORREF curColor = RGB(*(stack->memory + BEGINNING_OF_GMEM + curPixel), *(stack->memory + BEGINNING_OF_GMEM + curPixel + 1), *(stack->memory + BEGINNING_OF_GMEM + curPixel + 2));
        txSetPixel((curPixel / 3) % maxX, (curPixel / 3) / maxX, curColor);
    }
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

        printf("Please enter value to put in %d adress of RAM\n", argumentValue);
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

DEF_CMD_(ret, 36, 1, 
    commandPointer = StackPop(retStack);
    break;
)

DEF_CMD_(jne, 37, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    IF_JUMP_(!=)
)

DEF_CMD_(mkwdw, 40, 1,  
    int32_t yCoord = *(StackElem*)(stack->memory + BEGINNING_OF_GMEM - 4) / ACCURACY;
    int32_t xCoord = *(StackElem*)(stack->memory + BEGINNING_OF_GMEM - 8) / ACCURACY;

    assert(yCoord > 0);
    assert(xCoord > 0);

    txCreateWindow(xCoord, yCoord);
)

DEF_CMD_(call, 41, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    StackPush(retStack, commandPointer + COMMAND_SIZE + sizeOfArguments);

    commandPointer = SIGNATURE_SIZE + argumentValue;
    break;
)

DEF_CMD_(popbyte, 45, (comArgs.argFlags.bytes & LABEL_FLAG) || (!((!(comArgs.argFlags.bytes & CONST_FLAG)) | (comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG))) || (!((comArgs.argFlags.bytes & CONST_FLAG) | !(comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG))) || (!((comArgs.argFlags.bytes & MEM_FLAG) | (!(comArgs.argFlags.bytes & REG_FLAG)) | (!(comArgs.argFlags.bytes & CONST_FLAG)))), 
    if (commands->buffer[commandPointer + 1] & (MEM_FLAG << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;
        
        *(stack->memory + argumentValue) = (uint8_t)(StackPop(stack) / ACCURACY);
    }
    else if (commands->buffer[commandPointer + 1] & (REG_FLAG << SHIFT_OF_FLAGS)) {
        stack->registers[commands->buffer[commandPointer + 1] & REG_NUM_MASK] = (int8_t)(StackPop(stack) / ACCURACY);
    }
    else
        StackPop(stack);
)

DEF_CMD_(hlt, 0, 1, 
    
)

#undef IF_JUMP_