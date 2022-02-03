#ifndef _STRUCT_WORKS_H_
#define _STRUCT_WORKS_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

struct String {
    uint8_t *value;
    size_t length;

    uint32_t firstSpaceIdx;
    uint32_t lastSpaceBeforeArgs;
    size_t lenOfArgs;
};

struct Text {
    String *strings;
    uint8_t *buffer;
    size_t bufSize;
    uint32_t strAmount;
};

//-------------------------------------------------------------------------------------------------------
//! Reads text from input file to structure Text.
//!
//! @param [in] text Pointer to the Text structure
//! @param [in] inputFile name of input file
//!
//! @note You should use all functions from this file to fill up Text structure properly
//-------------------------------------------------------------------------------------------------------

void ReadTextFromFile(struct Text *text, const char* input);

//-------------------------------------------------------------------------------------------------------
//! Counts strings in structure's buffer and set structure's strAmount.
//!
//! @param [in] text Pointer to the Text structure
//-------------------------------------------------------------------------------------------------------

void CountStrAmount(struct Text*);

//-------------------------------------------------------------------------------------------------------
//! Fills strings of the Text structure with pointers on the beginning of each string and count length
//! of each string.
//!
//! @param [in] text Pointer to the Text structure
//!
//! @note You must allocate memory to strings (array of Strings structures) before using this function
//-------------------------------------------------------------------------------------------------------

void FillStrings(struct Text*);

//-------------------------------------------------------------------------------------------------------
//! Counts strings and fills strings of the Text structure using CountStrAmount and FillStrings
//!
//! @param [in] text Pointer to the Text structure
//-------------------------------------------------------------------------------------------------------

int MakeStrings(struct Text *text);

//-------------------------------------------------------------------------------------------------------
//! Writes each string of the structure strings of Text in output file
//!
//! @param [in] text Pointer to the Text structure
//! @param [in] output Pointer to the output stream
//-------------------------------------------------------------------------------------------------------

void ProcessStrings(Text* text);

void PrintStrings(const struct Text*, FILE* output);

//-------------------------------------------------------------------------------------------------------
//! Frees all dynamic arrays of the Text structure
//!
//! @param [in] text Pointer to the Text structure
//-------------------------------------------------------------------------------------------------------

void DestroyText(struct Text*);
#endif
