//! DON'T USE 36 COMMAND NUMBER - IT IS USED TO STORE STRINGS

//! Choose new command number correctly: cmdNum % (amount of command types) means:
//! 0 - processor command without arguments
//! 1 - processor command with arguments
//! 2 - assembler command without arguments
//! 3 - assembler command with argument


//This define helps to easily make if jumps
#define IF_JUMP_(symbol)                                    \
    StackElem secondOne = StackPop(procStack);                  \
    StackElem firstOne = StackPop(procStack);                   \
                                                            \
    if (firstOne symbol secondOne) {                        \
        commandPointer = SIGNATURE_SIZE + argumentValue;    \
        break;                                              \
    }

DEF_CMD_(push, 1, ((comArgs.argFlags.label) || 
                  (comArgs.argFlags.string) || 
                  !(comArgs.argFlags.mem    || comArgs.argFlags.reg || comArgs.argFlags.constant)),

    if (commands->buffer[commandPointer + 1] & (1 << MEM_SHIFT << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;

        StackPush(procStack, *(StackElem*)(procStack->memory + argumentValue));
    }
    else {
        StackPush(procStack, argumentValue);
    }
)

DEF_CMD_(db, 3, (!(comArgs.argFlags.string) || 
                   (comArgs.argFlags.label) || 
                   (comArgs.argFlags.reg)   || 
                   (comArgs.argFlags.mem)   || 
                   (comArgs.argFlags.constant)), 
)

DEF_CMD_(drwpc, 4, 1,
    int32_t maxX = txGetExtentX(); 

    for (uint32_t curPixel = 0; *(procStack->memory + BEGINNING_OF_GMEM + curPixel) != -1; curPixel += 3) {
        COLORREF curColor = RGB(*(procStack->memory + BEGINNING_OF_GMEM + curPixel), *(procStack->memory + BEGINNING_OF_GMEM + curPixel + 1), *(procStack->memory + BEGINNING_OF_GMEM + curPixel + 2));
        txSetPixel((curPixel / 3) % maxX, (curPixel / 3) / maxX, curColor);
    }
)

DEF_CMD_(pop, 5, (comArgs.argFlags.label)  || 
                 (comArgs.argFlags.string) || 
                 (!((!(comArgs.argFlags.constant)) ||  (comArgs.argFlags.mem)  || (  comArgs.argFlags.reg))) || 
                 (!((comArgs.argFlags.constant)    || !(comArgs.argFlags.mem)  || (  comArgs.argFlags.reg))) || 
                 ((!(comArgs.argFlags.constant))   || !((comArgs.argFlags.mem) || (!(comArgs.argFlags.reg)))), 
    if (commands->buffer[commandPointer + 1] & (1 << MEM_SHIFT << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;
        
        *(StackElem*)(procStack->memory + argumentValue) = StackPop(procStack);
    }
    else if (commands->buffer[commandPointer + 1] & (1 << REG_SHIFT << SHIFT_OF_FLAGS)) {
        procStack->regs[commands->buffer[commandPointer + 1] & REG_NUM_MASK] = StackPop(procStack);
    }
    else
        StackPop(procStack);
)

DEF_CMD_(add, 8, 1,  
    StackAdd(procStack);
)

DEF_CMD_(jump, 9, (!(comArgs.argFlags.label)    || 
                    (comArgs.argFlags.string)   || 
                    (comArgs.argFlags.reg) || 
                    (comArgs.argFlags.mem)      || 
                    (comArgs.argFlags.constant)),

    commandPointer = SIGNATURE_SIZE + argumentValue;
    break;
)

DEF_CMD_(sub, 12, 1,  
    StackSub(procStack);
)

DEF_CMD_(in, 13, (comArgs.argFlags.label) || 
                (comArgs.argFlags.string) || 
                (!((!(comArgs.argFlags.constant))   ||  (comArgs.argFlags.mem) || (comArgs.argFlags.reg))) || 
                (!((  comArgs.argFlags.constant)    || !(comArgs.argFlags.mem) || (comArgs.argFlags.reg)))   || 
                (!((!(comArgs.argFlags.constant))   ||  (comArgs.argFlags.mem) || (!(comArgs.argFlags.reg)))), 
    float scannedValue = 0;
    
    if (commands->buffer[commandPointer + 1] & (1 << MEM_SHIFT << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;

        ScanIn(&scannedValue);

        *(int32_t*)(procStack->memory + argumentValue) = (int32_t)(scannedValue * ACCURACY);
    }
    else if (commands->buffer[commandPointer + 1] & (1 << REG_SHIFT << SHIFT_OF_FLAGS)) {
        ScanIn(&scannedValue);

        procStack->regs[commands->buffer[commandPointer + 1] & REG_NUM_MASK] = (int32_t)(scannedValue * ACCURACY);
    }
    else {
        ScanIn(&scannedValue);

        StackPush(procStack, (StackElem)(scannedValue * ACCURACY));
    }
)

DEF_CMD_(mul, 16, 1, 
    StackMul(procStack);
)

DEF_CMD_(ja, 17, (!(comArgs.argFlags.label)    || 
                  (comArgs.argFlags.string)    || 
                  (comArgs.argFlags.reg)  || 
                  (comArgs.argFlags.mem)       || 
                  (comArgs.argFlags.constant)), 
    IF_JUMP_(>)
)

DEF_CMD_(div, 20, 1, 
    StackDiv(procStack);
)

DEF_CMD_(jae, 21, (!(comArgs.argFlags.label)    || 
                    (comArgs.argFlags.string)   || 
                    (comArgs.argFlags.reg) || 
                    (comArgs.argFlags.mem)      || 
                    (comArgs.argFlags.constant)), 
    IF_JUMP_(>=)
)

DEF_CMD_(out, 24, 1, 
    StackOut(procStack);
)

DEF_CMD_(jb, 25, (!(comArgs.argFlags.label)     || 
                    (comArgs.argFlags.string)   || 
                    (comArgs.argFlags.reg) || 
                    (comArgs.argFlags.mem)      || 
                    (comArgs.argFlags.constant)), 
    IF_JUMP_(<)
)

DEF_CMD_(dump, 28, 1, 
    StackDump(procStack LOCATION());
)

DEF_CMD_(jbe, 29, (!(comArgs.argFlags.label)    || 
                    (comArgs.argFlags.string)   || 
                    (comArgs.argFlags.reg) || 
                    (comArgs.argFlags.mem)      || 
                    (comArgs.argFlags.constant)), 
    IF_JUMP_(<=)
)

DEF_CMD_(exeDump, 32, 1, 
    StackExeDump(commands->buffer, commands->bufSize, commandPointer);
)

DEF_CMD_(je, 33, (!(comArgs.argFlags.label)     || 
                    (comArgs.argFlags.string)   || 
                    (comArgs.argFlags.reg) || 
                    (comArgs.argFlags.mem)      || 
                    (comArgs.argFlags.constant)), 
    IF_JUMP_(==)
)

//! DON'T USE 36 COMMAND NUMBER - IT IS USED TO STORE STRINGS
//! DON'T USE 36 COMMAND NUMBER - IT IS USED TO STORE STRINGS
//! DON'T USE 36 COMMAND NUMBER - IT IS USED TO STORE STRINGS

DEF_CMD_(jne, 37, (!(comArgs.argFlags.label)    || 
                    (comArgs.argFlags.string)   || 
                    (comArgs.argFlags.reg) || 
                    (comArgs.argFlags.mem)      || 
                    (comArgs.argFlags.constant)), 
    IF_JUMP_(!=)
)

DEF_CMD_(mkwdw, 40, 1,  
    int32_t yCoord = *(StackElem*)(procStack->memory + BEGINNING_OF_GMEM - 4) / ACCURACY;
    int32_t xCoord = *(StackElem*)(procStack->memory + BEGINNING_OF_GMEM - 8) / ACCURACY;

    assert(yCoord > 0);
    assert(xCoord > 0);

    txCreateWindow(xCoord, yCoord);
)

DEF_CMD_(call, 41, (!(comArgs.argFlags.label)   || 
                    (comArgs.argFlags.string)   || 
                    (comArgs.argFlags.reg) || 
                    (comArgs.argFlags.mem)      || 
                    (comArgs.argFlags.constant)), 
    StackPush(retStack, commandPointer + COMMAND_SIZE + sizeOfArguments);

    commandPointer = SIGNATURE_SIZE + argumentValue;
    break;
)

DEF_CMD_(popbyte, 45, (comArgs.argFlags.string) || 
                       (comArgs.argFlags.label) || 
                       (!((!(comArgs.argFlags.constant)) ||   (comArgs.argFlags.mem) ||   (comArgs.argFlags.reg))) || 
                       (!((comArgs.argFlags.constant)    ||  !(comArgs.argFlags.mem) ||   (comArgs.argFlags.reg))) || 
                       ((!(comArgs.argFlags.constant))   || !((comArgs.argFlags.mem) || (!(comArgs.argFlags.reg)))), 
    if (commands->buffer[commandPointer + 1] & (1 << MEM_SHIFT << SHIFT_OF_FLAGS)) {
        argumentValue /= ACCURACY;
        
        *(procStack->memory + argumentValue) = (uint8_t)(StackPop(procStack) / ACCURACY);
    }
    else if (commands->buffer[commandPointer + 1] & (1 << REG_SHIFT << SHIFT_OF_FLAGS)) {
        procStack->regs[commands->buffer[commandPointer + 1] & REG_NUM_MASK] = (int8_t)(StackPop(procStack) / ACCURACY);
    }
    else
        StackPop(procStack);
)

DEF_CMD_(ret, 44, 1, 
    commandPointer = StackPop(retStack);
    break;
)

DEF_CMD_(froot, 48, 1, 
    double root = StackPop(procStack) / ACCURACY;
    root = sqrt(root);
    StackPush(procStack, (StackElem)(root * ACCURACY));
)

DEF_CMD_(strout, 49, (!(comArgs.argFlags.label)   || 
                      (comArgs.argFlags.string)   || 
                      (comArgs.argFlags.reg) || 
                      (comArgs.argFlags.mem)      || 
                      (comArgs.argFlags.constant)), 
    StackPush(retStack, commandPointer + COMMAND_SIZE + sizeOfArguments);
    commandPointer = SIGNATURE_SIZE + argumentValue + STRING_DIVIDER_SIZE;
    char curChar = 0;

    while ((curChar = commands->buffer[commandPointer]) != '$') {
        putchar(curChar);
        commandPointer++;
    }
    putchar('\n');

    commandPointer = StackPop(retStack);
    break;
)

DEF_CMD_(jumpDED, 53, (!(comArgs.argFlags.label)     || 
                    (comArgs.argFlags.string)        || 
                    (comArgs.argFlags.reg)      || 
                    (comArgs.argFlags.mem)           || 
                    (comArgs.argFlags.constant)), 
    time_t curTimeSec = time(NULL);
    struct tm *curTime = localtime(&curTimeSec);
    printf("Today weeday is %d\n", curTime->tm_wday);
    
    if ((curTime->tm_wday == 1) || (curTime->tm_wday == 3) || (curTime->tm_wday == 5)) {
        commandPointer = SIGNATURE_SIZE + argumentValue;
        break;
    }
)

DEF_CMD_(hlt, 0, 1, 
)

#undef IF_JUMP_