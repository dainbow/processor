#include <sys\stat.h>

#include "Utilities.h"

int MyFPuts(const uint8_t *str, FILE *stream) {
    assert(str != nullptr);
    assert(stream != nullptr);

    for (int i = 0; str[i] != '\0' && str[i] != '\n'; i++) {
        fputc(str[i], stream);
    }
    fputc('\n', stream);

    return 1;
}

bool MyStrCmp(const int8_t* str1, const int8_t* str2) {
    size_t strIdx = 0;
    
    while ((str1[strIdx] != '\0') && (str2[strIdx] != '\0')) {
        if (str1[strIdx] != str2[strIdx]) {
            return 1;
        }

        strIdx++;
    }

    return 0;
}

size_t CountFileSize (int fd) {
	assert(fd != -1);
	
	struct stat fileStat = {};
    fstat(fd, &fileStat);
	
	return fileStat.st_size;
}