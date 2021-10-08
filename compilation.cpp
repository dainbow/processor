#include <stdlib.h>

#include "Text.h"
#include "Utilities.h"

struct CompileResult {
    size_t bytesCount;
    uint8_t* bytesArray;
};

void Compile(Text* text, const char* outName = "b.txt");
bool ValidateDigitArg(String* string);

int main() {
    Text input;
    ReadTextFromFile(&input, "input.txt");
    MakeStrings(&input);
    ProcessStrings(&input);

    printf("%d\n", (unsigned char)128);;

    Compile(&input);

    printf("OK");
    return 0;
}

void Compile(Text* text, const char* outName) {
    CompileResult output = {};
    output.bytesCount = 0;
    output.bytesArray = (uint8_t*)calloc(text->bufSize, sizeof(output.bytesArray[0]));

    uint8_t* operationName = 0;
    int32_t operationArg = 0;

    for(size_t curString = 0; curString < text->strAmount; curString++) {
        String comparingString = text->strings[curString];

        printf("%d %d\n", comparingString.length, comparingString.firstSpaceIdx);

        if ((comparingString.firstSpaceIdx == 4 | comparingString.length == 5) 
        & !MyStrCmp((const int8_t*)comparingString.value, (const int8_t*)"push")) {
            if(ValidateDigitArg(&comparingString)) {
                operationArg = atoi((const char*)(comparingString.value + 6)); //! 6 is the length of word "push"

                *(output.bytesArray + output.bytesCount) = (uint8_t)1;
                *(uint32_t*)(output.bytesArray + output.bytesCount + 1) = operationArg;

                output.bytesCount += 1 + 1*4; //1 command takes 1 byte, 1 argument takes 4 bytes       
            }
            else {
                printf("UNVALID ARGUMENT OF PUSH ON %d LINE\n", curString + 1);
                abort();
            }
        }
        else {
            printf("UNKNOWN OPERATION ON %d LINE\n", curString + 1);
            abort();
        }
    }
}

bool ValidateDigitArg(String* string) {
    if (string->firstSpaceIdx == 0 | string->length - 2 == string->firstSpaceIdx) {
        return 0;
    }

    for(size_t curChar = string->firstSpaceIdx + 1; curChar < string->length; curChar++) {
        if (string->value[curChar] != '\0') {
            if (!isdigit(string->value[curChar])) {
                return 0;
            }
        }
        else {
            break;
        }
    }
    return 1;
}