#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <stdint.h>

#define DEF_CMD_(cmdName, cmdNum, ...) \
    CMD_##cmdName = cmdNum,

const int32_t COMMAND_SIZE          = 1;
const int32_t CONST_ARGUMENT_SIZE   = 4;
const int32_t MAX_FILE_SIZE         = 100;
const int32_t ACCURACY              = 10;
const int32_t SIGNATURE_SIZE        = 5;
const int32_t FAIL                  = 0;
const int32_t BYTE_OF_ARGS          = 1;
const int32_t MAX_REGISTER_AMOUNT   = 4;
const int32_t MAX_MEMORY_SIZE       = 1000000;
const int32_t MAX_OUTPUTFILE_LENGTH = 100;
const int32_t MAX_NUMBER_SIZE       = 400;
const int32_t MAX_REG_NAME          = 3;
const int32_t MAX_LABEL_NAME        = 100;
const int32_t MAX_LABEL_AMOUNT      = 70;
const int32_t BEGINNING_OF_GMEM     = 500000;
const int32_t MAX_STRING_NAME       = 100;
const int32_t STRING_DIVIDER_SIZE   = 1;
const int32_t QUOTE_SIZE            = 1;
const int32_t BRACKET_BUFFER_SIZE   = 10;
const int32_t TRASH_BUFFER_SIZE     = 100;
const int32_t BRACKETS_SIZE         = 2;

const int32_t ARGUMENT_TYPE_MASK     = 1;
const int32_t PREPROCESSOR_TYPE_MASK = 2;
const int32_t COMMAND_NUMBER_SHIFT   = 2;

const char SIGNATURE[]    = "DAIN7"; 
const char STRING_DIVIDER = '$';

const int32_t SHIFT_OF_FLAGS = 3;
const int32_t MEM_SHIFT      = 0;

const int32_t REG_SHIFT      = 1;
const int32_t CONST_SHIFT    = 2;
const int32_t LABEL_SHIFT    = 3;
const int32_t STRING_SHIFT   = 4;

const int32_t MAX_COMMAND_TYPES = 4;

const int32_t REG_NUM_MASK   = (1) | (1 << 1) | (1 << 2);

struct Label {
    uint8_t* name;
    int64_t go;
};

struct Labels {
    Label array[MAX_LABEL_AMOUNT];
    uint32_t curLbl;
    bool isAllDataRead;
};

enum Commands {
    #include "cmd_def.h"
};

void FillLabelsPoison(Labels* labels);

#undef DEF_CMD_
#endif