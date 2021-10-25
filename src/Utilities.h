#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------------
//! Checks if symbol is cyrillic
//!
//! @param [in] sym Symbol which will be checked
//!
//! @return True if cyrillic, false in other cases
//-------------------------------------------------------------------------------------------------------

bool IsCyrillic(const uint16_t sym);

//-------------------------------------------------------------------------------------------------------
//! Writes string which ends on '\0' or '\n' in stream
//!
//! @param [in] str Pointer to the beginning of the string
//! @param [in] stream Pointer to the stream
//!
//! @return Returns 1 in case of success or EOF
//-------------------------------------------------------------------------------------------------------

int MyFPuts(const uint8_t *str, FILE *stream);

bool MyStrCmp(const int8_t*  str1, const int8_t* str2);

//-------------------------------------------------------------------------------------------------------
//! Counts size of file
//!
//! @param [in] fd File destructor
//!
//! @return Size of file in bytes
//-------------------------------------------------------------------------------------------------------

size_t CountFileSize (int fd);

void ScanIn(float* scannedValue);

void DetermiteOutputFile(int* argc, char *argv[], char** outputFile); 

size_t strLenWithoutSpaces(char* countingString);

bool MyLblCmp(const int8_t* str1, const int8_t* str2);

#endif
