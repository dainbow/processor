#include <assert.h>
#include <io.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "Text.h"
#include "Utilities.h"

void ReadTextFromFile(struct Text *text, const char* inputFile) {
    assert(text != nullptr);
    assert(inputFile != nullptr);

    int input = open(inputFile, O_RDONLY | O_BINARY, 0);
    assert(input != -1);

    text->bufSize = CountFileSize(input);
    text->buffer = (uint8_t*)calloc(text->bufSize + 1, sizeof(text->buffer[0]));
    assert(text->buffer != nullptr);

    read(input, text->buffer, (uint32_t)text->bufSize);

    close(input);
}

void CountStrAmount(struct Text *text) {
    assert(text != nullptr);

    uint32_t strCount = 0;

    for (uint32_t curChr = 0; curChr < (text->bufSize); curChr++) {
        if (text->buffer[curChr] == '\n') {
            strCount++;
        }
    }

    text->strAmount = strCount;
}

void FillStrings(struct Text *text) {
    assert(text != nullptr);
	
	text->strings = (struct String*)calloc(text->strAmount + 1, sizeof(text->strings[0]));
    assert(text->strings != nullptr);

    text->strings[0].value = &text->buffer[0];
    for (size_t curStrBuf = 1, curStrIdx = 1; curStrBuf < text->bufSize; curStrBuf++) {
        if (text->buffer[curStrBuf - 1] == '\n') {
            text->strings[curStrIdx].firstSpaceIdx = 0;

            text->strings[curStrIdx].value = &text->buffer[curStrBuf];
            text->strings[curStrIdx - 1].length = text->strings[curStrIdx].value - text->strings[curStrIdx - 1].value - 1;

            curStrIdx++;
        }
    }
    text->strings[text->strAmount - 1].length = &text->buffer[text->bufSize] - text->strings[text->strAmount - 1].value - 1;
}

void ProcessStrings(Text* text) { //TODO SKIP COMMENTS IN THE MIDDLE OF STRINGS
    uint32_t lastStrNotSpace = 0;

    for (size_t curString = 0; curString < text->strAmount; curString++) {
        for (uint32_t curChar = 0; text->strings[curString].value[curChar] != '\0'; curChar++) {
            switch (text->strings[curString].value[curChar])
            {
            case '\r':
            case '\n':
            case ';':
                text->strings[curString].value[curChar] = '\0';
                break;
            case ' ':
                break;    
            default:
                lastStrNotSpace = curChar;
                break;
            }
        }

        for (uint32_t curChar = 0; text->strings[curString].value[curChar] != '\0'; curChar++) {
            if (text->strings[curString].value[0] == ' ') {
                text->strings[curString].value++;
                text->strings[curString].length--;

                curChar = 0;
            }

            if (text->strings[curString].value[curChar] == ' ') {
                if (text->strings[curString].firstSpaceIdx == 0)
                    text->strings[curString].firstSpaceIdx = curChar;
                if (curChar > lastStrNotSpace)
                    text->strings[curString].value[curChar] = '\0';
            }
        }
    }
}

int MakeStrings(struct Text *text) {
	CountStrAmount(text);
	FillStrings(text);

	return 1;
}

/*void PrintStrings(const struct Text *text, FILE* output) {
    assert(text != nullptr);
    assert(output != nullptr);

    for (size_t curStr = 0; curStr < text->strAmount; curStr++) {
        MyFPuts(text->strings[curStr].value, output);
    }
    fprintf(output, "%s\n", END_OF_PART);
}*/

void DestroyText(struct Text *text) {
    assert(text != nullptr);

    free(text->buffer);
    text->buffer = nullptr;

    free(text->strings);
    text->strings = nullptr;
}

