#include "stack.h"

int32_t StackCtor_(Stack* stack, VarInfo creationInfo) {
    assert(stack != nullptr && "STACK_NULL");

    stack->capacity     = STACK_BEGINNING_CAPACITY;
    stack->size         = 0;
    stack->memory       = (uint8_t*)calloc(MAX_MEMORY_SIZE, sizeof(stack->memory[0]));

    #if (STACK_DEBUG >= LOW_LEVEL)
    stack->creationInfo = creationInfo;
    #endif

    uint64_t sizeOfData           = stack->capacity * sizeof(StackElem);
    uint64_t sizeOfAllData        = sizeOfData + PROTECTION_SIZE;

    stack->data = (uint8_t*)calloc(sizeOfAllData, sizeof(stack->data[0]));
    assert(stack->data != nullptr && "DATA_NULL");
	stack->data += SHIFT;

    #if (STACK_DEBUG >= MID_LEVEL)
        stack->canaryLeft         = CANARY;
        stack->canaryRight        = CANARY;

        uint8_t* endOfData        = stack->data + sizeOfData;

        *(canary*)(stack->data - SHIFT)   = CANARY;
        *(canary*)(endOfData)             = CANARY;
    #endif


    #if (STACK_DEBUG >= HIGH_LEVEL)
        for (int32_t curIdx = 0; curIdx < stack->capacity; curIdx++) {
            *(StackElem*)(stack->data + curIdx * sizeof(StackElem)) = POISON;
        }

        WriteAllStackHash(stack);
    #endif

    return 0;
}

int32_t StackDtor(Stack* stack) {
    CheckAllStack(stack);
	stack->data -= SHIFT;

    free(stack->data);
    stack->data  = (uint8_t*)FREE_VALUE;
    stack->size  = -1;

    stack->data += SHIFT;

    return 0;
}

int32_t StackPush(Stack* stack, StackElem pushedValue) {
    CheckAllStack(stack);

    if (stack->size >= stack->capacity)
        stack->data  = StackIncrease(stack);

    *(StackElem*)(stack->data + (stack->size) * sizeof(StackElem)) = pushedValue;
    stack->size++;

    #if (STACK_DEBUG >= HIGH_LEVEL)
        WriteAllStackHash(stack);
    #endif

    CheckAllStack(stack);
    return 0;
}

StackElem StackPop(Stack* stack) {
    CheckAllStack(stack);

    if (stack->capacity >= (int32_t)DECREASE_MULTIPLIER * stack->size)
        stack->data      = StackDecrease(stack);

    assert(stack->size != 0 && "STACK_UNDERFLOW");

    --stack->size;
    StackElem poppedValue = *(StackElem*)(stack->data + stack->size * sizeof(StackElem));
    *(StackElem*)(stack->data + stack->size * sizeof(StackElem)) = POISON;

    #if (STACK_DEBUG >= HIGH_LEVEL)
        WriteAllStackHash(stack);
    #endif

    CheckAllStack(stack);
    return poppedValue;
}

StackError IsStackOk(Stack* stack) {
    if (stack == nullptr)                               return STACK_NULL;

    return NO_ERROR;
}

StackError IsDataOk(Stack* stack) {
    if (stack->data - SHIFT == nullptr)                 return  DATA_NULL;
    if (stack->data - SHIFT == (uint8_t*)FREE_VALUE)    return STACK_FREE;

    return NO_ERROR;
}

StackError IsCapacityOk(Stack* stack) {
    if (stack->capacity < 0)             return CAPACITY_NEGATIVE;
    if (!isfinite(stack->capacity))      return CAPACITY_INFINITE;

    return NO_ERROR;
}

StackError IsSizeOk(Stack* stack) {
    if (stack->size > stack->capacity)   return STACK_OVERFLOW;
    if (stack->size < 0)                 return STACK_UNDERFLOW;

    return NO_ERROR;
}

#if (STACK_DEBUG >= MID_LEVEL)
StackError IsCanariesOk(Stack* stack) {
    if (stack->canaryLeft  != CANARY)                   return LEFT_STACK_CANARY_IRRUPTION;
    if (stack->canaryRight != CANARY)                   return RIGHT_STACK_CANARY_IRRUPTION;

    uint64_t sizeOfData = stack->capacity * sizeof(StackElem);
    if (*(canary*)(stack->data - SHIFT)      != CANARY) return LEFT_DATA_CANARY_IRRUPTION;
    if (*(canary*)(stack->data + sizeOfData) != CANARY) return RIGHT_DATA_CANARY_IRRUPTION;

    return NO_ERROR;
}
#endif

StackError IsHashesOk(Stack* stack) {
    hashValue stackHash      = *(hashValue*)(stack->data - 2 * sizeof(hashValue));
    hashValue dataHash       = *(hashValue*)(stack->data - sizeof(hashValue));

    if (GetStackHash(stack) != stackHash) return STACK_HASH_IRRUPTION;
    if (GetDataHash(stack)  != dataHash)  return DATA_HASH_IRRUPTION;

    return NO_ERROR;
}

StackError IsAllOk(Stack* stack) {
    if (StackError error =  IsStackOk(stack))    return error;
    if (StackError error =  IsDataOk(stack))     return error;
    if (StackError error =  IsCapacityOk(stack)) return error;
    if (StackError error =  IsSizeOk(stack))     return error;

    #if (STACK_DEBUG >= MID_LEVEL)
    if (StackError error =  IsCanariesOk(stack)) return error;
    #endif

    #if (STACK_DEBUG >= HIGH_LEVEL)
    if (StackError error =  IsHashesOk(stack))   return error;
    #endif

    return NO_ERROR;
}

hashValue GetHash(uint8_t* pointer, uint64_t size) {
    assert(pointer != nullptr);
    assert(size > 0);

    hashValue hashSum = 0;
    for (uint32_t curByte = 0; curByte < size; curByte++) {
        hashSum += *(pointer + curByte) * powllu(HASH_BASE, curByte);
    }

    return hashSum;
}

hashValue GetStackHash(Stack* stack) {
    uint32_t sizeOfStack = sizeof(Stack) - 2 * sizeof(canary);

    return GetHash((uint8_t*)stack + sizeof(canary), sizeOfStack);
}

hashValue GetDataHash(Stack* stack) {
    uint64_t sizeOfData  = stack->capacity * sizeof(StackElem);

    return GetHash(stack->data, sizeOfData);
}

void WriteAllStackHash(Stack* stack) {
    hashValue* stackHashLocation = (hashValue*)(stack->data - 2 * sizeof(hashValue));
    *stackHashLocation = GetStackHash(stack);

    hashValue* dataHashLocation  = (hashValue*)(stack->data - sizeof(hashValue));
    *dataHashLocation  = GetDataHash(stack);
}

int StackDump(Stack* stack, VarInfo dumpInfo, FILE* outstream) {
    setvbuf(outstream, NULL, _IONBF, 0);

    fprintf(outstream, "Dump from %s() at %s(%d) in stack called now \"%s\": IsAllOk() %s\n",
            dumpInfo.function, dumpInfo.file, dumpInfo.line, dumpInfo.name, IsAllOk(stack) == NO_ERROR ? "Ok" : "FAILED");

    fprintf(outstream, "stack <int> [%p] (ok) \"%s\" ",
            &stack, stack->creationInfo.name);
    fprintf(outstream, "from %s (%d), %s(): {\n",
            stack->creationInfo.file,  stack->creationInfo.line, stack->creationInfo.function);

    fprintf(outstream, "size     = %d (%s)\n",
            stack->size,     ErrorToString(IsSizeOk(stack)));
    fprintf(outstream, "capacity = %d (%s)\n\n",
            stack->capacity, ErrorToString(IsCapacityOk(stack)));

    #if (STACK_DEBUG >= MID_LEVEL)
        fprintf(outstream, "Stack canaries:\n");
        fprintf(outstream, "    canaryLeft[%p] = %ud (%s)\n",
                &stack->canaryLeft,  stack->canaryLeft,  (stack->canaryLeft  == CANARY) ?
                "Ok" : "IRRUPTION");
        fprintf(outstream, "    canaryRight[%p] = %ud (%s)\n",
                &stack->canaryRight, stack->canaryRight, (stack->canaryRight == CANARY) ?
                "Ok" : "IRRUPTION");

        canary* leftDataCanaryLocation  = (canary*)(stack->data - SHIFT);
        canary* rightDataCanaryLocation = (canary*)(stack->data + stack->capacity * sizeof(StackElem));
        fprintf(outstream, "Data canaries:\n");
        fprintf(outstream, "    canaryLeft[%p] = %ud (%s)\n",
                leftDataCanaryLocation,  *leftDataCanaryLocation,  (*leftDataCanaryLocation  == CANARY) ?
                "Ok" : "IRRUPTION");
        fprintf(outstream, "    canaryRight[%p] = %ud (%s)\n\n",
                rightDataCanaryLocation, *rightDataCanaryLocation, (*rightDataCanaryLocation == CANARY) ?
                "Ok" : "IRRUPTION");
    #endif

    #if (STACK_DEBUG >= HIGH_LEVEL)
        hashValue* stackHashLocation = (hashValue*)(stack->data - 2 * sizeof(hashValue));
        hashValue curHash = GetStackHash(stack);

        fprintf(outstream, "Stack hashes:\n");
        fprintf(outstream, "    Stored stack hash[%p] = %llud\n",
                stackHashLocation, *stackHashLocation);
        fprintf(outstream, "    Current stack hash = %llud\n", curHash);
        fprintf(outstream, "    %s\n", (*stackHashLocation == curHash) ?
                "(Hashes are equal)" : "(HASHES AREN'T EQUAL)");

        hashValue* dataHashLocation = (hashValue*)(stack->data - sizeof(hashValue));
        curHash = GetDataHash(stack);
        fprintf(outstream, "Data hashes:\n");
        fprintf(outstream, "    Stored data hash[%p] = %llud\n",
                dataHashLocation, *dataHashLocation);
        fprintf(outstream, "    Current data hash = %llud\n", curHash);
        fprintf(outstream, "    %s\n\n",
                (*dataHashLocation == curHash) ?
                "(Hashes are equal)" : "(HASHES AREN'T EQUAL)");
    #endif

    fprintf(outstream, "data[%p] (%s)\n",
            stack->data - SHIFT, ErrorToString(IsDataOk(stack)));
    #if (STACK_DEBUG >= HIGH_LEVEL)
        fprintf(outstream, "{\n");

        for(int32_t curIdx = 0; curIdx < stack->capacity; curIdx++) {
            StackElem* curElement = (StackElem*)(stack->data + curIdx * sizeof(StackElem));

            if (curIdx < stack->size) {
                fprintf(outstream, "   *[%d][%p] = %d (%s)\n",
                        curIdx, curElement, *curElement, (*curElement == POISON) ?
                        "MAYBE POISON" : "Ok");
            }
            else {
                fprintf(outstream, "   [%d][%p] = %d (%s)\n",
                        curIdx, curElement, *curElement, (*curElement == POISON) ?
                        "Poison" : "NOT POISON, BUT SHOULD BE");
            }
        }

        fprintf(outstream, "}\n");
    #endif

    return 0;
}

const char* ErrorToString(StackError error) {
    switch(error) {
        case NO_ERROR:                      return "Ok";
        case STACK_OVERFLOW:                return "OVERFLOW";
        case STACK_UNDERFLOW:               return "UNDERFLOW";
        case CAPACITY_NEGATIVE:             return "NEGATIVE CAPACITY";
        case DATA_NULL:                     return "DATA IS NULL";
        case STACK_FREE:                    return "STACK ALREADY FREE";
        case STACK_NULL:                    return "STACK NULL";
        case CAPACITY_INFINITE:             return "INFINITE CAPCITY";
        case LEFT_STACK_CANARY_IRRUPTION:   return "LEFT STACK CANARY IRRUPTION";
        case RIGHT_STACK_CANARY_IRRUPTION:  return "RIGHT STACK CANARY IRRUPTION";
        case LEFT_DATA_CANARY_IRRUPTION:    return "LEFT DATA CANARY IRRUPTION";
        case RIGHT_DATA_CANARY_IRRUPTION:   return "RIGHT DATA CANARY IRRUPTION";
        case STACK_HASH_IRRUPTION:          return "STACK IRRUPTION";
        case DATA_HASH_IRRUPTION:           return "DATA IRRUPTION";

        default:                            return "UNKNOWN ERROR";
    }
}

uint8_t* StackIncrease(Stack* stack) {
    CheckAllStack(stack);
    uint64_t sizeOfData             = stack->capacity * sizeof(StackElem);
    uint64_t sizeOfIncreasedData    = sizeOfData * (uint32_t)INCREASE_MULTIPLIER + 1;

    #if (STACK_DEBUG >= MID_LEVEL)
        canary* rightDataCanaryLocation = (canary*)(stack->data + sizeOfData);

        canary cellCanary               = *rightDataCanaryLocation;
        *rightDataCanaryLocation        = 0;
    #endif

    uint8_t* newPointer = (uint8_t*)realloc(stack->data - SHIFT, PROTECTION_SIZE + sizeOfIncreasedData);
    assert(newPointer  != nullptr && "MEMORY_INCREASE_ERR");
    stack->capacity     = stack->capacity * (int32_t)INCREASE_MULTIPLIER + 1;

    #if (STACK_DEBUG >= MID_LEVEL)
        sizeOfData               = stack->capacity * sizeof(StackElem);

        rightDataCanaryLocation  = (canary*)(stack->data + sizeOfData);
        *rightDataCanaryLocation = cellCanary;
    #endif

    #if (STACK_DEBUG >= HIGH_LEVEL)
        for (int32_t curIdx  = stack->size + 1; curIdx < stack->capacity; curIdx++) {
            *(StackElem*)(stack->data + curIdx * sizeof(StackElem)) = POISON;
        }

        WriteAllStackHash(stack);
    #endif

    CheckAllStack(stack);

    newPointer += SHIFT;
    return newPointer;
}

uint8_t* StackDecrease(Stack* stack) {
    CheckAllStack(stack);
    uint64_t sizeOfData             = stack->capacity * sizeof(StackElem);
    uint64_t sizeOfDecreasedData    = sizeOfData / (uint32_t)DECREASE_MULTIPLIER;

    #if (STACK_DEBUG >= MID_LEVEL)
        canary* rightDataCanaryLocation = (canary*)(stack->data + sizeOfData);

        canary cellCanary               = *rightDataCanaryLocation;
        *rightDataCanaryLocation        = 0;
    #endif

    uint8_t* newPointer = (uint8_t*)realloc(stack->data - SHIFT, PROTECTION_SIZE + sizeOfDecreasedData);
    assert(newPointer  != nullptr && "MEMORY_DECREASE_ERROR");
    stack->capacity    /= (int32_t)DECREASE_MULTIPLIER;

    #if (STACK_DEBUG >= MID_LEVEL)
        sizeOfData               = stack->capacity * sizeof(StackElem);

        rightDataCanaryLocation  = (canary*)(stack->data + sizeOfData);
        *rightDataCanaryLocation = cellCanary;
    #endif

    #if (STACK_DEBUG >= HIGH_LEVEL)
        for (int32_t curIdx  = stack->size + 1; curIdx < stack->capacity; curIdx++) {
            *(StackElem*)(stack->data + curIdx * sizeof(StackElem)) = POISON;
        }

        WriteAllStackHash(stack);
    #endif

    CheckAllStack(stack);

    newPointer += SHIFT;
    return newPointer;
}

uint64_t powllu(int32_t base, int32_t power) {
    uint64_t result = 1;

    for (int32_t curMulti = 0; curMulti < power; curMulti++) {
        result     *= base;
    }

    return result;
}

void StackAdd(Stack* stack) {
    CheckAllStack(stack);

    int32_t firstSummand  = StackPop(stack);
    int32_t secondSummand = StackPop(stack); 

    printf("Pushed add result %d\n", firstSummand + secondSummand);
    StackPush(stack, firstSummand + secondSummand);
}

void StackSub(Stack* stack) {
    CheckAllStack(stack);

    int32_t deductible = StackPop(stack);
    int32_t reduced    = StackPop(stack);

    StackPush(stack, reduced - deductible);
}

void StackMul(Stack* stack) {
    CheckAllStack(stack);

    int32_t firstMult  = StackPop(stack);
    int32_t secondMult = StackPop(stack);

    StackPush(stack, firstMult * secondMult / ACCURACY);
}

void StackDiv(Stack* stack) {
    CheckAllStack(stack);

    int32_t divisible = StackPop(stack);
    int32_t divider   = StackPop(stack);

    StackPush(stack, divisible / divider * ACCURACY);
}

void StackOut(Stack* stack) {
    CheckAllStack(stack);
    printf("IN OUT\n");

    for (int32_t curIdx = stack->size; curIdx > 0; curIdx--) {
        float printOut = (float)StackPop(stack);
        printOut /= ACCURACY;

        printf("[%d]: %f\n", curIdx, printOut);
    }
}

void StackExeDump(uint8_t* buffer, uint64_t bufSize, uint32_t comPtr) {
    int32_t printCount = 0;

    for (uint32_t curByte = 0; curByte < bufSize; curByte++) {
        if (curByte < comPtr)
            printCount += printf("%d " ,buffer[curByte]);
        else
            printf("%d " ,buffer[curByte]);     
    }
    printf("\n");
    printf("%*s!^!\n", printCount - 1, "");
}