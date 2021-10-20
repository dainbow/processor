DEF_CMD_(push, 1, comArgs.argFlags.bytes & LABEL_FLAG, 
    StackPush(stack, *(int32_t*)(&commands->buffer[commandPointer] + 1));
    printf("I'M IN PUSH\n");
)

DEF_CMD_(pop, 5, (comArgs.argFlags.bytes & LABEL_FLAG) || !((comArgs.argFlags.bytes & MEM_FLAG) | (comArgs.argFlags.bytes & REG_FLAG)), 
    StackPop(stack);
)

DEF_CMD_(add, 8, 1,  
    StackAdd(stack);
)

DEF_CMD_(sub, 12, 1,  
    StackSub(stack);
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

DEF_CMD_(hlt, 0, 1, 
    
)

DEF_CMD_(exeDump, 32, 1, 
    StackExeDump(commands->buffer, commands->bufSize, commandPointer);
)

DEF_CMD_(jump, 9, (!(comArgs.argFlags.bytes & LABEL_FLAG) || (comArgs.argFlags.bytes & REG_FLAG) || (comArgs.argFlags.bytes & MEM_FLAG) || (comArgs.argFlags.bytes & CONST_FLAG)), 
    ...........
)
