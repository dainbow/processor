DEF_CMD_(push, 1,
    StackPush(stack, *(int32_t*)(&commands->buffer[commandPointer] + 1));
    printf("I'M IN PUSH\n");
)

DEF_CMD_(pop, 4, 
    StackPop(stack);
)

DEF_CMD_(add, 8, 
    StackAdd(stack);
)

DEF_CMD_(sub, 12, 
    StackSub(stack);
)

DEF_CMD_(mul, 16, 
    StackMul(stack);
)

DEF_CMD_(div, 20, 
    StackDiv(stack);
)

DEF_CMD_(out, 24, 
    StackOut(stack);
)

DEF_CMD_(dump, 28, 
    StackDump(stack LOCATION());
)

DEF_CMD_(hlt, 0, 
    
)

DEF_CMD_(exeDump, 32, 
    StackExeDump(commands->buffer, commands->bufSize, commandPointer);
)
