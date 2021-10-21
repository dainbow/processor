#include "Compilation.h"

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, ...)                                                                    \
if (((currentString.firstSpaceIdx == strlen(#cmdName)) | (currentString.length == strlen(#cmdName)))                    \
& !MyStrCmp((const int8_t*)currentString.value, (const int8_t*)#cmdName)) {                                             \
    uint32_t* argIdxs = (uint32_t*)calloc(MAX_ARGUMENT_AMOUNT, sizeof(argIdxs[0]));                                     \
    printf("Command name: %s.\n", #cmdName);                                                                            \
                                                                                                                        \
    *(output.bytesArray + output.bytesCount) = (uint8_t)cmdNum;                                                         \
    output.bytesCount += COMMAND_SIZE;                                                                                  \
    if(cmdNum % 4) {                                                                                                    \
        comArgs.argFlags.bytes |= LABEL_FLAG;                                                                           \
        ParseArgs(&currentString, &comArgs, !(cmdArgFilter));                                                           \
                                                                                                                        \
        printf("Arg is %d\nReg is %d\nIs to RAM? %d\nLabel is %s\n", comArgs.argConst, comArgs.argReg, comArgs.argFlags.bytes & MEM_FLAG, (comArgs.argFlags.bytes & LABEL_FLAG) ? comArgs.labelName : "0");        \
        if (cmdArgFilter) {                                                                                             \
            printf("UNCORRECT ARGUMENT FOR %s COMMAND AT %u LINE", #cmdName, curString);                                \
            abort();                                                                                                    \
        }                                                                                                               \
                                                                                                                        \
        printf("Bytes: %u\n", comArgs.argFlags.bytes);                                                                  \
                                                                                                                        \
        for (uint32_t curArg = 0; curArg < cmdNum % 4; curArg++) {                                                      \
            *(output.bytesArray + output.bytesCount) = (uint8_t)(comArgs.argFlags.bytes << SHIFT_OF_FLAGS | comArgs.argReg);         \
            output.bytesCount += BYTE_OF_ARGS;                                                                          \
                                                                                                                        \
            if (comArgs.argFlags.bytes & CONST_FLAG) {                                                                  \
                *(uint32_t*)(output.bytesArray + output.bytesCount) = comArgs.argConst;                                 \
                output.bytesCount += CONST_ARGUMENT_SIZE;                                                                     \
            }                                                                                                           \
                                                                                                                        \
            if (comArgs.argFlags.bytes & LABEL_FLAG) {                                        \
                *(uint32_t*)(output.bytesArray + output.bytesCount) = (labels.isAllDataRead) ? FindLabel(comArgs.labelName, &labels) : 0;            \
                output.bytesCount += CONST_ARGUMENT_SIZE;                                                                     \
            }                                                                                                           \
        }                                                                                                               \
    }                                                                                                                   \
}                                                                                                                       \
else                                                                                                               
  

int main(int32_t argc, char** argv) {
    char* outputFile = 0;
    ReadOutArgument(&argc, argv, &outputFile);

    printf("%s\n", outputFile);

    for (int32_t curArgument = 1; curArgument < argc; curArgument++) {
        Text input = {};
        printf("Compiling file: %s...\n", argv[curArgument]);
    
        ReadTextFromFile(&input, argv[curArgument]);
        MakeStrings(&input);
        ProcessStrings(&input);
        Compile(&input, outputFile);  //TODO При одинакомых названиях в скобочках номер копии.
        Compile(&input, outputFile); 
    }

    printf("OK");
    return 0;
}

void ReadOutArgument(int32_t* argc, char *argv[], char** outputFile) {
	assert(argc != nullptr);
	assert(argv != nullptr);

    *outputFile = (char*)calloc(MAX_FILE_SIZE, sizeof(**outputFile));

    for (int32_t curArgument = 1; curArgument < *argc; curArgument++) {
        if (!strcmp(argv[curArgument], "-o")) {
            strcat(*outputFile, argv[curArgument + 1]);
            *argc = curArgument;

            return;
        }
    }

    strcat(*outputFile, "b.txt");
}

void Compile(Text* text, const char* outName) {
    assert(text    != nullptr);
    assert(outName != nullptr);

    static uint32_t NUM_OF_COMPILING = 1;

    CompileResult output = {0, (uint8_t*)calloc(text->bufSize + 4, sizeof(output.bytesArray[0]))};
    
    static Labels labels = {};
    if (NUM_OF_COMPILING > 1) {
        labels.isAllDataRead = 1;
    }

    for(uint32_t curString = 0; curString < text->strAmount; curString++) {  
        Arguments comArgs = {};
        String currentString = text->strings[curString];

        printf("Curstring length is %llu\n", currentString.length);

        if (IfLabel(&currentString, &labels, output.bytesCount) != 1) {
        #include "cmd_def.h"
        {printf("UNKNOWN COMMAND ON %u LINE\n", curString);
        abort();}
        } 
    }
    #undef DEF_CMD_

    FILE* outputd = fopen(outName, "wb");
    printf("I wrote %llu signature bytes\n",  fwrite(SIGNATURE, 1, SIGNATURE_SIZE, outputd));
    printf("I wrote %llu bytes from array\n---------END OF %u COMPILING-------\n", fwrite(output.bytesArray, 1, output.bytesCount, outputd), NUM_OF_COMPILING);

    fclose(outputd);
    free(output.bytesArray);
    NUM_OF_COMPILING++;
}

void ParseArgs(String* string, Arguments* comArg, bool isLabel) {
    size_t sumLenArgs = 0;
    comArg->argFlags.bytes = 0;

    const char* ptrToArgs = ShiftAndCheckArgs(string);

    if (isLabel)
        ParseLabel(ptrToArgs, comArg, &sumLenArgs);
    else {
        ParseBrackets(   ptrToArgs, comArg, &sumLenArgs);
        ParseRegister(   ptrToArgs, comArg, &sumLenArgs);
        ParseConst(      ptrToArgs, comArg, &sumLenArgs);
        ParseSeveralArgs(ptrToArgs, comArg, &sumLenArgs);
    }

    if (sumLenArgs != string->lenOfArgs) {
        printf("I have parsed %llu\nI should %llu\n", sumLenArgs, string->lenOfArgs);

        assert(FAIL && "SOME ARGUMENTS ARE SUBSTRINGS OF ANOTHER STRINGS");
    }
}

void ParseBrackets(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    assert(ptrToArgs != nullptr);
    assert(comArg != nullptr);
    assert(sumLenArgs != nullptr);

    char lbracket[10] = "";
    char rbracket[10] = "";
    char trashLetters[100] = "";

    switch (sscanf(ptrToArgs, "%*1[ ]%1[{]%*[a-dx0-9 +.]%1[}]%[ {}a-dx0-9.+]", lbracket, rbracket, trashLetters)) {  //Checks if want to write in memory
        case 3:                                                 
            assert(FAIL && "SOMETHING AFTER BRACKETS");
            break;
        case 2:
            *sumLenArgs += 2;
            comArg->argFlags.bytes |= MEM_FLAG;
            break;
        case 1:
            assert(FAIL && "RIGHT BRACKET DIDN'T CLOSE OR UNKNOWN SYMBOLs IN BRACKETS");
            break;
        case 0:
            break;
        default:
            assert(FAIL && "UNKNOWN CASE IN BRACKET SWITCH");
            break;
    }
}

void ParseRegister(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    assert(ptrToArgs != nullptr);
    assert(comArg != nullptr);
    assert(sumLenArgs != nullptr);

    char reg[10] = "";

    if(sscanf(    ptrToArgs, "%*[ 0-9+.{}]%[a-dx]", reg) == 1) {            //Checks if there are any register symbols
        if(sscanf(ptrToArgs, "%*[ 0-9+.{}]%1[a-d]%*1[x]", reg) != 1) {      //If there are register symbols, 1 register should be in argument
            assert(FAIL && "UNKNOWN FORMAT OF REGISTER");
        }
        else {
            *sumLenArgs += 2;
            
            comArg->argReg = reg[0] - 'a';
            comArg->argFlags.bytes |= REG_FLAG;
        }
    }
}

void ParseConst(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    assert(ptrToArgs != nullptr);
    assert(comArg != nullptr);
    assert(sumLenArgs != nullptr);

    char constantBeforeDot[100] = "";
    char constantAfterDot[100] = "";
    char trashLetters[100] = "";

    if(sscanf(    ptrToArgs, "%*[ a-dx0-9+{]%[.]", trashLetters) == 1) {                                   //Checks if dot is in argument
        if(sscanf(ptrToArgs, "%*[ a-dx+{]%[0-9]%*1[.]%[0-9]", constantBeforeDot, constantAfterDot) != 2) { //Checks if float number in argument
            assert(FAIL && "UNKNOWN FORMAT OF FLOAT NUMBER");
        }
        else {
            strcat((char*)constantBeforeDot, ".");
            strcat((char*)constantBeforeDot, constantAfterDot);

            comArg->argFlags.bytes |= CONST_FLAG;
        }
    }
    else {
            if(sscanf(ptrToArgs, "%*[ a-dx+{]%[0-9]", constantBeforeDot) == 1) {
                comArg->argFlags.bytes |= CONST_FLAG;
        }
    }

    if (comArg->argFlags.bytes & CONST_FLAG) {
        *sumLenArgs += strlen(constantBeforeDot);
        comArg->argConst = (int32_t)(atof(constantBeforeDot) * ACCURACY);
    }
}

void ParseSeveralArgs(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    char trashLetters[100] = "";

    if ((comArg->argFlags.bytes & CONST_FLAG) && (comArg->argFlags.bytes & REG_FLAG)) {
        if ((sscanf(ptrToArgs, "%*[ a-dx{]%*[+]%[ 0-9.}]", trashLetters)) == (sscanf(ptrToArgs, "%*[ 0-9.{]%*[+]%[ }a-dx]", trashLetters))) {
        assert(FAIL && "WRONG POSITION OF PLUS");
        }

        *sumLenArgs += 1;
    }
}

void ParseLabel(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    if (sscanf(ptrToArgs, "%*1[ ]%[a-zA-Z0-9]", comArg->labelName) == 1) {  
        *sumLenArgs += strlen(comArg->labelName);
        comArg->argFlags.bytes |= LABEL_FLAG;
    }
    else {
        assert(FAIL && "LABEL NOT FOUND");
    }
}

const char* ShiftAndCheckArgs(String* string) {
    char trashLetters[100] = "";
    const char* ptrToArgs = (const char*)string->value + string->lastSpaceBeforeArgs;
    
    if(sscanf(ptrToArgs, "%[ {}a-zA-Zx0-9.+]", trashLetters) & (strlen(trashLetters) != string->length - string->lastSpaceBeforeArgs)) {
        assert(FAIL && "UNKNOWN LETTERS IN ARGUMENT");
    }

    return ptrToArgs;
}

bool IfLabel(String* string, Labels* labels, int32_t curCommandPointer) {
    char trashLetters[100] = "";
    char labelName[MAX_LABEL_NAME] = "";

    if (sscanf((const char*)string->value, "%[a-zA-Z0-9]%1[:]", labelName, trashLetters) == 2) { 
        if (strlen(labelName) + 1 == string->length) {
            printf("Found label\n");
            if (labels->curLbl < MAX_LABEL_AMOUNT) {
                if (labels->isAllDataRead == 0) {
                strcpy(labels->array[labels->curLbl].name, labelName);
                labels->array[labels->curLbl].go = curCommandPointer;
                labels->curLbl++;

                printf("%u label name is \"%s\"\n", labels->curLbl, labels->array[labels->curLbl - 1].name);
                }

                return 1;
            }
            else 
                assert(FAIL && "YOU ARE CRAZY, YOU'VE REACHED MAX AMOUNT OF LABELS");
        }
        else {
            printf("Strlen is %llu\nI counted %llu\n", strlen(labelName) + 1, string->length);

            assert(FAIL && "THERE IS NOTHING SHOULD BE AFTER LABEL");
        }
    }

    return 0;
}

int32_t FindLabel(char lblName[], Labels* labels) {
    for (uint32_t curLbl = 0; (labels->array[curLbl].go != -1) || (curLbl < MAX_LABEL_AMOUNT); curLbl++) {
        if(strcmp(lblName, labels->array[curLbl].name) == 0) {
            printf("Label %s goes to %d ip\n", lblName, labels->array[curLbl].go);

            return labels->array[curLbl].go;
        }
    }

    assert(FAIL && "LABEL NOT FOUND");
    return 0;
}