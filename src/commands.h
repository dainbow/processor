#pragma once
#include <stdint.h>

#define DEF_CMD_(cmdName, cmdNum, ...) \
    CMD_##cmdName = cmdNum,

const uint32_t COMMAND_SIZE = 1;
const uint32_t ARGUMENT_SIZE = 4;
const uint32_t MAX_ARGUMENT_AMOUNT = 4;
const uint32_t MAX_FILE_SIZE = 100;
const uint32_t ACCURACY = 1000;

enum Commands {
    #include "cmd_def.h"
};

#undef DEF_CMD_