//! DON'T USE 36 COMMAND NUMBER - IT IS USED TO STORE STRINGS

//! Command code (8 bit) format: 
//!      
//!      76543210
//!      nnnnnnPA, where: 
//!
//! nnnnnn - command code number,
//! P - preprocessor phase flag,
//! A - command with argument flag. 

//This define helps to easily make if jumps
#define IF_JUMP_(symbol)                                    \
    StackElem secondOne = POPPED_VALUE;                     \
    StackElem firstOne  = POPPED_VALUE;                     \
                                                            \
    if (firstOne symbol secondOne) {                        \
        JUMP_TO(COMMAND_ARGUMENT);                          \
    }

#define IF_USE_MEMORY   if (commands->buffer[commandPointer + 1] & (1 << MEM_SHIFT << SHIFT_OF_FLAGS))
#define IF_USE_REGISTER if (commands->buffer[commandPointer + 1] & (1 << REG_SHIFT << SHIFT_OF_FLAGS))
#define IF_USE_CONSTANT if (commands->buffer[commandPointer + 1] & (1 << CONST_SHIFT << SHIFT_OF_FLAGS))

#define COMMAND_ARGUMENT argumentValue

#define GET_DATA_FROM_MEMORY(MEMORY_ADRESS) *(StackElem*)(procStack->memory + MEMORY_ADRESS / ACCURACY)
#define GET_BYTE_FROM_MEMORY(MEMORY_ADRESS) *(procStack->memory + MEMORY_ADRESS / ACCURACY)

#define GET_REGISTER_NUMBER commands->buffer[commandPointer + 1] & REG_NUM_MASK
#define GET_DATA_FROM_REGISTER(REGISTER_NUMBER) procStack->regs[REGISTER_NUMBER]

#define PUSH_TO_STACK(VALUE) StackPush(procStack, VALUE)

#define STACK_ADD      StackAdd(procStack)
#define STACK_SUB      StackSub(procStack)
#define STACK_MUL      StackMul(procStack)
#define STACK_DIV      StackDiv(procStack)
#define STACK_POW      StackPow(procStack)

#define STACK_OUT(amount)      StackOut(procStack, amount)
#define STACK_DUMP     StackDump(procStack LOCATION())
#define STACK_EXE_DUMP StackExeDump(commands->buffer, commands->bufSize, commandPointer)

#define X_WINDOW *(StackElem*)(procStack->memory + BEGINNING_OF_GMEM - 8) / ACCURACY
#define Y_WINDOW *(StackElem*)(procStack->memory + BEGINNING_OF_GMEM - 4) / ACCURACY
#define CREATE_WINDOW(X, Y) txCreateWindow(X, Y)

#define POPPED_VALUE                StackPop(procStack) 
#define BYTE_POPPED_VALUE (uint8_t)(StackPop(procStack) / ACCURACY)

#define STR_OUT(STRING_ADRESS)                                                  \
    REMEMBER_WHERE_TO_RETURN;                                                    \
    commandPointer = SIGNATURE_SIZE + STRING_ADRESS + STRING_DIVIDER_SIZE + 1;      \
    char curChar = 0;                                                           \
                                                                                \
    while ((curChar = commands->buffer[commandPointer]) != '$') {               \
        putchar(curChar);                                                       \
        commandPointer++;                                                       \
    }                                                                           \
    putchar('\n');                                                              \
                                                                                \
    RETURN                                                                      \

#define SQRT_FROM_STACK                                 \
    double root = StackPop(procStack) / ACCURACY;       \
    root = sqrt(root);                                  \
    StackPush(procStack, (StackElem)(root * ACCURACY))


#define REMEMBER_WHERE_TO_RETURN StackPush(retStack, commandPointer + COMMAND_SIZE + sizeOfArguments)
#define RETURN                              \
    commandPointer = StackPop(retStack);    \
    break

#define SCANNED_VALUE (int32_t)(scannedValue * ACCURACY)
#define INPUT_SCAN                                      \
    float scannedValue = 0;                             \
    ScanIn(&scannedValue)

#define JUMP_TO(VALUE)                                  \
    commandPointer = SIGNATURE_SIZE + argumentValue;    \
    break 

#define IF_TODAY_DED                                                                    \
    time_t curTimeSec  = time(NULL);                                                    \
    struct tm *curTime = localtime(&curTimeSec);                                        \
    if ((curTime->tm_wday == 1) || (curTime->tm_wday == 3) || (curTime->tm_wday == 5))  

#define DISPLAY_GMEMORY                                                                                            \
    int32_t maxX = txGetExtentX();                                                                               \
    printf("X of window is %d\n", maxX); \
    printf("Y of window is %d\n", txGetExtentY());\
    for (uint32_t curPixel = 0; *(procStack->memory + BEGINNING_OF_GMEM + curPixel) != -1; curPixel += 3) {      \
        COLORREF curColor = RGB(*(procStack->memory + BEGINNING_OF_GMEM + curPixel),                             \
                                *(procStack->memory + BEGINNING_OF_GMEM + curPixel + 1),                         \
                                *(procStack->memory + BEGINNING_OF_GMEM + curPixel + 2));                        \
        txSetPixel((curPixel / 3) % maxX, (curPixel / 3) / maxX, curColor);                                      \
    }

DEF_CMD_(push, 1, PushArgsFilter,
    IF_USE_MEMORY {
        PUSH_TO_STACK(GET_DATA_FROM_MEMORY(COMMAND_ARGUMENT));
    }
    else {
        PUSH_TO_STACK(COMMAND_ARGUMENT);
    }
)

DEF_CMD_(db, 3, DbArgsFilter, 
)

DEF_CMD_(drwpc, 4, NoArgsFilter,
    DISPLAY_GMEMORY
)

DEF_CMD_(pop, 5, PopArgsFilter, 
    IF_USE_MEMORY {      
        GET_DATA_FROM_MEMORY(COMMAND_ARGUMENT) = POPPED_VALUE;
    }
    else IF_USE_REGISTER {
        GET_DATA_FROM_REGISTER(GET_REGISTER_NUMBER) = POPPED_VALUE;
    }
    else
        POPPED_VALUE;
)

DEF_CMD_(add, 8, NoArgsFilter,  
    STACK_ADD;
)

DEF_CMD_(jump, 9, JumpArgsFilter,
    JUMP_TO(COMMAND_ARGUMENT);
)

DEF_CMD_(sub, 12, NoArgsFilter,  
    STACK_SUB;
)

DEF_CMD_(in, 13, PopArgsFilter, 
    IF_USE_MEMORY {
        INPUT_SCAN;
        GET_DATA_FROM_MEMORY(COMMAND_ARGUMENT) = SCANNED_VALUE;
    }
    else IF_USE_REGISTER {
        INPUT_SCAN;
        GET_DATA_FROM_REGISTER(GET_REGISTER_NUMBER) = SCANNED_VALUE;
    }
    else {
        INPUT_SCAN;
        PUSH_TO_STACK(SCANNED_VALUE);
    }
)

DEF_CMD_(mul, 16, NoArgsFilter, 
    STACK_MUL;
)

DEF_CMD_(ja, 17, JumpArgsFilter, 
    IF_JUMP_(>)
)

DEF_CMD_(div, 20, NoArgsFilter, 
    STACK_DIV;
)

DEF_CMD_(jae, 21, JumpArgsFilter, 
    IF_JUMP_(>=)
)

DEF_CMD_(jb, 25, JumpArgsFilter, 
    IF_JUMP_(<)
)

DEF_CMD_(dump, 28, NoArgsFilter, 
    STACK_DUMP;
)

DEF_CMD_(jbe, 29, JumpArgsFilter, 
    IF_JUMP_(<=)
)

DEF_CMD_(exeDump, 32, NoArgsFilter, 
    STACK_EXE_DUMP;
)

DEF_CMD_(je, 33, JumpArgsFilter, 
    IF_JUMP_(==)
)

//! DON'T USE 36 COMMAND NUMBER - IT IS USED TO STORE STRINGS
//! DON'T USE 36 COMMAND NUMBER - IT IS USED TO STORE STRINGS
//! DON'T USE 36 COMMAND NUMBER - IT IS USED TO STORE STRINGS

DEF_CMD_(jne, 37, JumpArgsFilter, 
    IF_JUMP_(!=)
)

DEF_CMD_(mkwnd, 40, NoArgsFilter,  
    _txWindowStyle |= WS_THICKFRAME;
    _txConsole = -1;
    CREATE_WINDOW(X_WINDOW, Y_WINDOW);
)

DEF_CMD_(call, 41, JumpArgsFilter, 
    REMEMBER_WHERE_TO_RETURN;
    JUMP_TO(COMMAND_ARGUMENT);
)

DEF_CMD_(popbyte, 45, PopArgsFilter, 
    IF_USE_MEMORY {
        GET_BYTE_FROM_MEMORY(COMMAND_ARGUMENT) = BYTE_POPPED_VALUE;
    }
    else IF_USE_REGISTER {
        GET_DATA_FROM_REGISTER(GET_REGISTER_NUMBER) = BYTE_POPPED_VALUE;
    }
    else
        POPPED_VALUE;
)

DEF_CMD_(ret, 44, NoArgsFilter, 
    RETURN;
)

DEF_CMD_(froot, 48, NoArgsFilter, 
    SQRT_FROM_STACK;
)

DEF_CMD_(strout, 49, JumpArgsFilter, 
    STR_OUT(COMMAND_ARGUMENT);
)

DEF_CMD_(pow, 52, NoArgsFilter,
    STACK_POW;
)

DEF_CMD_(jumpDED, 53, JumpArgsFilter, 
    IF_TODAY_DED {
        JUMP_TO(COMMAND_ARGUMENT);
    }
)

DEF_CMD_(out, 57, OutArgsFilter, 
    IF_USE_MEMORY {
        STACK_OUT(COMMAND_ARGUMENT);
    }
    else IF_USE_REGISTER {
        STACK_OUT(COMMAND_ARGUMENT);
    }
    else IF_USE_CONSTANT {
        STACK_OUT(COMMAND_ARGUMENT); 
    }
    else {
        STACK_OUT(-1);
    }
)

DEF_CMD_(hlt, 0, NoArgsFilter, 
)

#undef IF_JUMP_