#pragma once
#include <stdint.h>

#define DEF_CMD_(cmdName, cmdNum, ...) \
    CMD_##cmdName = cmdNum,

const uint32_t COMMAND_SIZE = 1;
const uint32_t CONST_ARGUMENT_SIZE = 4;
const uint32_t MAX_ARGUMENT_AMOUNT = 4;
const uint32_t SHIFT_OF_FLAGS = 4;
const uint32_t MAX_FILE_SIZE = 100;
const uint32_t ACCURACY = 1000;
const uint32_t SIGNATURE_SIZE = 5;
const int32_t FAIL = 0;
const uint32_t BYTE_OF_ARGS = 1;
const uint32_t MAX_REGISTER_AMOUNT = 4;
const uint32_t MAX_MEMORY_SIZE = 4000;

const char SIGNATURE[] = "DAIN1"; 

const int32_t MEM_FLAG = 1;
const int32_t REG_FLAG = 1 << 1;
const int32_t CONST_FLAG = 1 << 2;
const int32_t LABEL_FLAG = 1 << 3;
const int32_t REG_NUM_MASK = (1) | (1 << 1) | (1 << 2) | (1 << 3);

enum Commands {
    #include "cmd_def.h"
};

#undef DEF_CMD_