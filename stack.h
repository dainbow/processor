#ifndef _STACK_H_
#define _STACK_H_

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LOW_LEVEL  1
#define MID_LEVEL  2
#define HIGH_LEVEL 3

#define STACK_DEBUG 1

#ifndef STACK_DEBUG
    #define STACK_DEBUG LOW_LEVEL
#endif

#if (STACK_DEBUG >= LOW_LEVEL)
    #define LOCATION(...) , { __FILE__, __FUNCTION__, __LINE__, #__VA_ARGS__ }
#else
    #define LOCATION(...)
#endif

#define StackCtor(stack)                           \
    Stack stack = {};                              \
    StackCtor_(&stack LOCATION (stack));

#define CheckAllStack(stack)                                                                  \
    if (StackError error = IsAllOk(stack)) {                                                  \
        printf("Error %s, read full description in dump file\n", ErrorToString(error));       \
                                                                                              \
        StackDump(stack LOCATION (stack));                                                    \
        assert(!"OK" && "Verify failed");                                                     \
    }

typedef uint32_t canary;
typedef uint64_t hashValue;
typedef int32_t StackElem;

#if (STACK_DEBUG == HIGH_LEVEL)
    const uint32_t SHIFT = sizeof(canary) + 2 * sizeof(hashValue);
    const uint32_t PROTECTION_SIZE = 2 * (sizeof(canary) + sizeof(hashValue));
#endif

#if (STACK_DEBUG == MID_LEVEL)
    const uint32_t SHIFT = sizeof(canary);
    const uint32_t PROTECTION_SIZE = 2 * sizeof(canary);
#endif

#if (STACK_DEBUG == LOW_LEVEL)
    const uint32_t SHIFT = 0;
    const uint32_t PROTECTION_SIZE = 0;
#endif

const uint32_t POISON     = 0xE2202;
const canary CANARY       = 0xDEAD;
const uint32_t FREE_VALUE = 0xF2EE;

const uint32_t STACK_BEGINNING_CAPACITY = 50;
const uint32_t HASH_BASE = 257;

const float INCREASE_MULTIPLIER = 1.5;
const float DECREASE_MULTIPLIER = 2;
const uint32_t ACCURACY = 3;

enum StackError {
    NO_ERROR = 0,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    CAPACITY_NEGATIVE,
    DATA_NULL,
    STACK_FREE,
    STACK_NULL,
    CAPACITY_INFINITE,
    LEFT_STACK_CANARY_IRRUPTION,
    RIGHT_STACK_CANARY_IRRUPTION,
    LEFT_DATA_CANARY_IRRUPTION,
    RIGHT_DATA_CANARY_IRRUPTION,
    STACK_HASH_IRRUPTION,
    DATA_HASH_IRRUPTION
};

struct VarInfo {
    const char* file;
    const char* function;
    int line;

    const char* name;
};

struct Stack {
#if (STACK_DEBUG >= MID_LEVEL)
    canary canaryLeft;
#endif

#if (STACK_DEBUG >= LOW_LEVEL)
    VarInfo creationInfo;
#endif

    int32_t size;
    int32_t capacity;
    uint8_t* data;

#if (STACK_DEBUG >= MID_LEVEL)
    canary canaryRight;
#endif
};

//-------------------------------------------------------------------------------------------
//! Creates stack from StackCtor macro
//!
//! @param [in] stack Pointer to the stack structure which will be created
//!
//! @return 0 if no errors
//!
//! @note You should type in macro StackCtor name of future stack without quotation marks
//!       You can change POISON value if stack element type has unreachable value
//-------------------------------------------------------------------------------------------

#if (STACK_DEBUG >= LOW_LEVEL)
    int32_t StackCtor_(Stack* stack, VarInfo);
#else
    int32_t StackCtor_(Stack* stack);
#endif

//-------------------------------------------------------------------------------------------
//! Destroys stack
//!
//! @param [in] stack Pointer to the stack which will be destroyed
//!
//! @return 0 if no errors
//-------------------------------------------------------------------------------------------

int32_t StackDtor(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Pushes element in stack
//!
//! @param [in] stack Pointer to the stack where element will be pushed
//! @param [in] pushedValue StackElem variable which will be pushed
//!
//! @return 0 if no errors
//!
//! @note Memory of stack can be increased if size >= value by StackIncrease function
//-------------------------------------------------------------------------------------------

int32_t StackPush(Stack* stack, StackElem pushedValue);

//-------------------------------------------------------------------------------------------
//! Pops element from stack
//!
//! @param [in] stack Pointer to the stack where from element will be popped
//!
//! @return StackElem  - popped value
//-------------------------------------------------------------------------------------------

StackElem StackPop(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Checks if stack structure is OK
//!
//! @param [in] stack Pointer to the stack which will be checked
//!
//! @return One of StackError
//!
//! @note Returns NO_ERROR if stack is OK (For all check functions)
//-------------------------------------------------------------------------------------------

StackError IsStackOk(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Checks if data of stack is OK
//!
//! @param [in] stack Pointer to the stack which data will be checked
//!
//! @return One of StackError
//-------------------------------------------------------------------------------------------

StackError IsDataOk(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Checks if capacity of stack is OK
//!
//! @param [in] stack Pointer to the stack which capacity will be checked
//!
//! @return One of StackError
//-------------------------------------------------------------------------------------------

StackError IsCapacityOk(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Checks if size of stack is OK
//!
//! @param [in] stack Pointer to the stack which size will be checked
//!
//! @return One of StackError
//-------------------------------------------------------------------------------------------

StackError IsSizeOk(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Checks if canaries of stack are OK
//!
//! @param [in] stack Pointer to the stack which canaries will be checked
//!
//! @return One of StackError
//-------------------------------------------------------------------------------------------

StackError IsCanariesOk(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Checks if hashes of stack are OK
//!
//! @param [in] stack Pointer to the stack which hashes will be checked
//!
//! @return One of StackError
//-------------------------------------------------------------------------------------------

StackError IsHashesOk(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Checks if all components of stack are OK
//!
//! @param [in] stack Pointer to the stack which will be checked
//!
//! @return One of StackError
//-------------------------------------------------------------------------------------------

StackError IsAllOk(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Gets hash from size bytes after pointer
//!
//! @param [in] pointer Pointer to the beginning of hash calculating
//! @param [in] size Amount of bytes which will be included in hash calculating
//!
//! @return Calculated hash
//-------------------------------------------------------------------------------------------

hashValue GetHash(uint8_t* pointer, uint64_t size);

//-------------------------------------------------------------------------------------------
//! Gets hash from stack structure
//!
//! @param [in] stack Pointer to the stack which structure will be hashed
//!
//! @return Calculated stack structure hash
//-------------------------------------------------------------------------------------------

hashValue GetStackHash(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Gets hash from stack data
//!
//! @param [in] stack Pointer to the stack which data will be hashed
//!
//! @return Calculated stack data hash
//-------------------------------------------------------------------------------------------

hashValue GetDataHash(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Calculates new stack and data hashed and writes their values in storage (located in data)
//!
//! @param [in] stack Pointer to the stack which fully will be hashed
//-------------------------------------------------------------------------------------------

void WriteAllStackHash(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Writes full stack dump in outstream
//!
//! @param [in] stack Pointer to the stack which will be dumped
//! @param [in] outstream Pointer to the stream where dump will be written (default is stdout)
//!
//! @return 0 if all OK
//-------------------------------------------------------------------------------------------

int StackDump(Stack* stack, VarInfo dumpInfo = {}, FILE* outstream = stdout);

//-------------------------------------------------------------------------------------------
//! Converts StackError variable to its string representation
//!
//! @param [in] error StackError variable
//!
//! @return String which describes error
//-------------------------------------------------------------------------------------------

const char* ErrorToString(StackError error);

//-------------------------------------------------------------------------------------------
//! Increases stack memory if it ends
//!
//! @param [in] stack Pointer to the stack which memory will be increased
//!
//! @return Pointer to the increased memory area
//-------------------------------------------------------------------------------------------

uint8_t* StackIncrease(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Decreases stack memory if values take little space (Lower than DECREASE_MULTIPLIER times)
//!
//! @param [in] stack Pointer to the stack which memory will be decreased
//!
//! @return Pointer to the decreased memory area
//-------------------------------------------------------------------------------------------

uint8_t* StackDecrease(Stack* stack);

//-------------------------------------------------------------------------------------------
//! Multiply base to itself power times
//!
//! @param [in] base Base of power
//! @param [in] power Power of power
//!
//! @return Powered value
//!
//! @note Returns uint64_t value to store bigger values than standart function
//-------------------------------------------------------------------------------------------

uint64_t powllu(int32_t base, int32_t power);

void StackAdd(Stack* stack);

void StackSub(Stack* stack);

void StackMul(Stack* stack);

void StackDiv(Stack* stack);

void StackOut(Stack* stack);
#endif