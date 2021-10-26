#include "Compilation.h"

#define DEF_CMD_(cmdName, cmdNum, cmdArgFilter, ...)                                                                                                \
if (((currentString.firstSpaceIdx == strlen(#cmdName)) || (currentString.length == strlen(#cmdName)))                                               \
& !MyStrCmp((const int8_t*)currentString.value, (const int8_t*)#cmdName)) {                                                                         \
    printf("Command name: %s.\n", #cmdName);                                                                                                        \
    ImmitCommand(cmdNum, &output);                                                                                                                  \
                                                                                                                                                    \
    if((cmdNum % MAX_COMMAND_TYPES) % 2) {                                                                                                          \
        bool isLabel  = cmdArgFilter({0, 1, 0, 0, 0});                                                                                              \
        bool isString = cmdArgFilter({1, 0, 0, 0, 0});                                                                                              \
        ParseArgs(&currentString, &comArgs, isLabel, isString);                                                                                     \
                                                                                                                                                    \
        printf("Arg? %u It is %d\n"                                                                                                                 \
               "Reg?%u It is %d\n"                                                                                                                  \
               "Is to RAM? %u\n"                                                                                                                    \
               "Label is %s\n"                                                                                                                      \
               "String is %s\n",                                                                                                                    \
                comArgs.argFlags.constant, comArgs.argConst,                                                                                        \
                comArgs.argFlags.reg, comArgs.argReg,                                                                                               \
                comArgs.argFlags.mem,                                                                                                               \
               (comArgs.argFlags.label) ? comArgs.labelName : "NOLABEL",                                                                            \
               (comArgs.argFlags.string) ? comArgs.stringName : "NOSTRING");                                                                        \
        if (cmdArgFilter(comArgs.argFlags) == 0) {                                                                                                 \
            fprintf(stderr, "INCORRECT ARGUMENT FOR %s COMMAND AT %u LINE", #cmdName, curString);                                                   \
            abort();                                                                                                                                \
        }                                                                                                                                           \
        ImmitArgs(cmdNum, &output, &comArgs, &labels, &currentString);                                                                              \
    }                                                                                                                                               \
    printf("CommandPointer is %llu\n", output.bytesCount);                                                                                          \
}                                                                                                                                                   \
else    
                                                                                                              
int main(int32_t argc, char** argv) {
    char* outputFile = 0;
    DetermiteOutputFile(&argc, argv, &outputFile);

    printf("Compiling to %s...\n", outputFile);

    static Text input = {};
    printf("Compiling file: %s...\n", argv[1]);
    
    ReadTextFromFile(&input, argv[1]);
    MakeStrings(&input);   
    ProcessStrings(&input);
    Compile(&input, outputFile);
    Compile(&input, outputFile); 

    return 0;
}

void Compile(Text* text, const char* outName) {
    assert(text    != nullptr);
    assert(outName != nullptr);

    static uint32_t NUM_OF_COMPILING = 1;
    CompileResult output             = {0, (uint8_t*)calloc(text->bufSize + 4, sizeof(output.bytesArray[0]))};
    
    static Labels labels    = {};
    if (NUM_OF_COMPILING > 1) {
        labels.isAllDataRead = 1;
    }
    else {
        FillLabelsPoison(&labels);
    }

    for(uint32_t curString = 0; curString < text->strAmount; curString++) {  
        Arguments comArgs    = {};
        String currentString = text->strings[curString];
        if (currentString.value[0] == '\0') {
            continue;
        }

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

void ParseArgs(String* string, Arguments* comArg, bool isLabel, bool isString) {
    assert(string != nullptr);
    assert(comArg != nullptr);

    size_t sumLenArgs      = 0;
    const char* ptrToArgs  = ShiftAndCheckArgs(string);

    if (isLabel)
        ParseLabel(      ptrToArgs, comArg, &sumLenArgs);
    else if (isString)
        ParseString (    ptrToArgs, comArg, &sumLenArgs);
    else {
        ParseBrackets(   ptrToArgs, comArg, &sumLenArgs);
        ParseRegister(   ptrToArgs, comArg, &sumLenArgs);
        ParseConst(      ptrToArgs, comArg, &sumLenArgs);
        ParseSeveralArgs(ptrToArgs, comArg, &sumLenArgs);
    }

    if (sumLenArgs != string->lenOfArgs) {
        fprintf(stderr, "I have parsed %llu nonspace chars\n"
               "I should got  %llu as sum of lengths\n", sumLenArgs, string->lenOfArgs);

        assert(FAIL && "SOME ARGUMENTS ARE SUBSTRINGS OF ANOTHER STRINGS");
    }
}

void ParseBrackets(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    assert(ptrToArgs  != nullptr);
    assert(comArg     != nullptr);
    assert(sumLenArgs != nullptr);

    char lbracket[BRACKET_BUFFER_SIZE]      = "";      
    char rbracket[BRACKET_BUFFER_SIZE]      = "";
    char trashLetters[TRASH_BUFFER_SIZE]    = "";

    switch (sscanf(ptrToArgs, "%*1[ ]%1[{]%*[a-dx0-9 +.-]%1[}]%[ {}a-dx0-9.+-]", lbracket, rbracket, trashLetters)) {  //Checks if want to write in memory
        case 3:                                                 
            assert(FAIL && "SOMETHING AFTER BRACKETS");
            break;
        case 2:
            *sumLenArgs += BRACKETS_SIZE;
            comArg->argFlags.mem = 1;
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
    assert(ptrToArgs  != nullptr);
    assert(comArg     != nullptr);
    assert(sumLenArgs != nullptr);

    char reg[10] = "";

    if(sscanf(    ptrToArgs, "%*[ 0-9+.{}-]%[a-dx]",       reg) == 1) {            //Checks if there are any register symbols
        if(sscanf(ptrToArgs, "%*[ 0-9+.{}-]%1[a-d]%*1[x]", reg) != 1) {      //If there are register symbols, 1 register should be in argument
            assert(FAIL && "UNKNOWN FORMAT OF REGISTER");
        }
        else {
            *sumLenArgs += 2;
            
            comArg->argReg          = reg[0] - 'a';
            comArg->argFlags.reg    = 1;
        }
    }
}

void ParseConst(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    assert(ptrToArgs  != nullptr);
    assert(comArg     != nullptr);
    assert(sumLenArgs != nullptr);

    double constant = 0;
    char trashLetters[TRASH_BUFFER_SIZE] = "";

    if(sscanf(     ptrToArgs, "%*[ a-dx0-9+{-]%[.]", trashLetters) == 1) {                                   //Checks if dot is in argument
        if (sscanf(ptrToArgs, "%*[ a-dx+{]%lf",         &constant) != 1) { //Checks if float number in argument
            assert(FAIL && "UNKNOWN FORMAT OF FLOAT NUMBER");
        }
        else {
            comArg->argFlags.constant = 1;
        }
    }
    else {
        if(sscanf(ptrToArgs, "%*[ a-dx+{]%lf", &constant) == 1) {
            comArg->argFlags.constant = 1;
        }
    }

    if (comArg->argFlags.constant) {
        char constantStr[MAX_NUMBER_SIZE] = "";
        sprintf(constantStr, "%lf", constant);

        size_t lenOfFloat = strlen(constantStr);
        for (size_t curDigit = lenOfFloat - 1; curDigit > 0; curDigit--) {   //Removes from calculating zeros after dot
            if (constantStr[curDigit] != '0') {
                if (constantStr[curDigit] == '.') lenOfFloat--;
                break;
            }
            lenOfFloat--;
        }

        *sumLenArgs += lenOfFloat;
        comArg->argConst = (StackElem)(constant * ACCURACY);
    }
}

void ParseSeveralArgs(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {   //There is always should be plus between const and register
    assert(ptrToArgs  != nullptr);
    assert(comArg     != nullptr);
    assert(sumLenArgs != nullptr);
    
    char trashLetters[TRASH_BUFFER_SIZE] = "";

    if ((comArg->argFlags.constant) && (comArg->argFlags.reg)) {
        if ((sscanf(ptrToArgs, "%*[ a-dx{]%*[+]%[ 0-9.}-]", trashLetters)) == 
            (sscanf(ptrToArgs, "%*[ 0-9.{-]%*[+]%[ }a-dx]", trashLetters))) {
            assert(FAIL && "WRONG POSITION OF PLUS");
        }

        *sumLenArgs += 1;
    }
}

void ParseLabel(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    assert(ptrToArgs  != nullptr);
    assert(comArg     != nullptr);
    assert(sumLenArgs != nullptr);
    
    if (sscanf(ptrToArgs, "%*1[ ]%[a-zA-Z0-9]", comArg->labelName) == 1) {  
        *sumLenArgs += strlen(comArg->labelName);
        comArg->argFlags.label = 1;
    }
    else {
        assert(FAIL && "LABEL NOT FOUND");
    }
}

void ParseString(const char* ptrToArgs, Arguments* comArg, size_t* sumLenArgs) {
    assert(ptrToArgs  != nullptr);
    assert(comArg     != nullptr);
    assert(sumLenArgs != nullptr);
    char trashLetters[TRASH_BUFFER_SIZE] = "";

    switch (sscanf(ptrToArgs, "%*1[ ]%*1[\"]%*1[$]%[a-zA-Z0-9!'., ]%*1[$]%1[\"]", 
            comArg->stringName, trashLetters)) {  
        case 2:
            *sumLenArgs += strlen(comArg->stringName) + 2 * (QUOTE_SIZE + STRING_DIVIDER_SIZE);
            comArg->argFlags.string = 1;
            break;
        case 1:
            assert(FAIL && "RIGHT QUOTE NOT FOUND OR RIGHT DOLLAR NOT PLACED");
            break;
        case 0:
            assert(FAIL && "UNKNOWN FORMAT OF STRING");
            break;
        default:
            assert(FAIL && "UNKNOWN CASE OF STRING ANALYSING");
            break;
    }
}

const char* ShiftAndCheckArgs(String* string) {   //Shifts beginning of string to beginning of argument
    assert(string != nullptr);
    
    char trashLetters[TRASH_BUFFER_SIZE] = "";
    const char* ptrToArgs = (const char*)string->value + string->lastSpaceBeforeArgs;
    
    if(sscanf(ptrToArgs, "%[ {}a-zA-Zx0-9.+-\"\"!,$']", trashLetters) & 
      (strlen(trashLetters) != string->length - string->lastSpaceBeforeArgs)) {
        assert(FAIL && "UNKNOWN LETTERS IN ARGUMENT");
    }

    return ptrToArgs;
}

bool IfLabel(String* string, Labels* labels, size_t curCommandPointer) {   //Checks if string is a label, not a command
    assert(string != nullptr);
    assert(labels != nullptr);
    assert(isfinite(curCommandPointer));
    
    char trashLetters[TRASH_BUFFER_SIZE] = "";
    char labelName[MAX_LABEL_NAME] = "";
    size_t labelLength = 0;

    printf("Found label\n");
    if (sscanf((const char*)string->value, "%[a-zA-Z0-9]%1[:]", labelName, trashLetters) == 2) { 
        if ((labelLength = strlen(labelName) + 1) == string->length) {
            if (labels->curLbl < MAX_LABEL_AMOUNT) {
                if (labels->isAllDataRead == 0) {
                    labels->array[labels->curLbl].name = string->value;
                    labels->array[labels->curLbl].go   = curCommandPointer;
                    labels->curLbl++;

                    printf("%u label name is \"%s\"\n", labels->curLbl, labels->array[labels->curLbl - 1].name);
                }

                return 1;
            }
            else 
                assert(FAIL && "YOU ARE CRAZY, YOU'VE REACHED MAX AMOUNT OF LABELS");
        }
        else {
            fprintf(stderr, "Strlen is %llu\nI counted %llu\n", strlen(labelName) + 1, string->length);
            assert(FAIL && "THERE IS NOTHING SHOULD BE AFTER LABEL");
        }
    }

    return 0;
}

StackElem FindLabelByName(char lblName[], Labels* labels) {
    assert(lblName != nullptr);
    assert(labels  != nullptr);

    for (uint32_t curLbl = 0; (labels->array[curLbl].go != -1) && (curLbl < MAX_LABEL_AMOUNT); curLbl++) {
        if (MyLblCmp((const int8_t*)lblName, (const int8_t*)labels->array[curLbl].name) == 0) {
            printf("Label %s goes to %lld ip\n", lblName, labels->array[curLbl].go);

            return (StackElem)labels->array[curLbl].go;
        }
    }

    assert(FAIL && "LABEL NOT FOUND");
    return 0;
}

void ImmitCommand(int32_t cmdNum, CompileResult* output) {
    assert(isfinite(cmdNum));
    assert(output != nullptr);

    if (cmdNum % MAX_COMMAND_TYPES < 2) {                                                                                                           
        *(output->bytesArray + output->bytesCount) = (int8_t)cmdNum;                                                                                      
          output->bytesCount += COMMAND_SIZE;                                                                                                              
    } 
}

void ImmitArgs(int32_t cmdNum, CompileResult* output, Arguments* comArgs, Labels* labels, String* currentString) {
    assert(isfinite(cmdNum));
    assert(output        != nullptr);
    assert(comArgs       != nullptr);
    assert(labels        != nullptr);
    assert(currentString != nullptr);  
    
    uint32_t argumentFlags = (comArgs->argFlags.string   << STRING_SHIFT) | 
                             (comArgs->argFlags.label    << LABEL_SHIFT)  |
                             (comArgs->argFlags.constant << CONST_SHIFT)  |
                             (comArgs->argFlags.reg      << REG_SHIFT)    |
                             (comArgs->argFlags.mem      << MEM_SHIFT); 

    //!ImmitByteOfArg
    if (cmdNum % MAX_COMMAND_TYPES < 2) {                                                                                                       
        *(output->bytesArray + output->bytesCount) = (uint8_t)(argumentFlags << SHIFT_OF_FLAGS | comArgs->argReg);                        
          output->bytesCount += BYTE_OF_ARGS;                                                                                                      
    }                                                                                                                                           

    //!ImmitConstant                                                                                                                                 
    if (comArgs->argFlags.constant) {                                                                                                  
        *(StackElem*)(output->bytesArray + output->bytesCount) = comArgs->argConst;                                                                
                      output->bytesCount += CONST_ARGUMENT_SIZE;                                                                                               
    }                                                                                                                                           

    //!ImmitLabel                                                                                                                         
    if (comArgs->argFlags.label) {                                                                                                  
        *(StackElem*)(output->bytesArray + output->bytesCount) = (labels->isAllDataRead) ? FindLabelByName(comArgs->labelName, labels) : 0;        
                      output->bytesCount += CONST_ARGUMENT_SIZE;                                                                                               
    }                                                                                                                                           

    //!ImmitString                                                                                                                               
    if (comArgs->argFlags.string) {                                                                                                 
        *(output->bytesArray + output->bytesCount) = STRING_DIVIDER;                                                                              
          output->bytesCount += STRING_DIVIDER_SIZE;                                                                                               
                                                                                                                                                
        for (size_t curChar = 0; curChar < currentString->lenOfArgs - 2*STRING_DIVIDER_SIZE - 2*QUOTE_SIZE; curChar++) {                         
            *(output->bytesArray + output->bytesCount) = comArgs->stringName[curChar];                                                             
              output->bytesCount++;                                                                                                                
        }                                                                                                                                       
                                                                                                                                                
        *(output->bytesArray + output->bytesCount) = STRING_DIVIDER;                                                                              
          output->bytesCount += STRING_DIVIDER_SIZE;                                                                                               
    }
}

bool PushArgsFilter(Flags argFlags) {
    if ((argFlags.label)  || 
        (argFlags.string) || 
       !(argFlags.mem     || argFlags.reg || argFlags.constant))
       return 0;
    return 1;
}

bool DbArgsFilter(Flags argFlags) {
    if ((!argFlags.string) || 
         (argFlags.label)  || 
         (argFlags.reg)    || 
         (argFlags.mem)    || 
         (argFlags.constant))
        return 0;
    return 1;
}

bool PopArgsFilter(Flags argFlags) {
    if ((  argFlags.label)  || 
        (  argFlags.string) || 
        (( argFlags.constant)   &&  (!argFlags.mem)   &&  (!argFlags.reg))  || 
        ((!argFlags.constant)   &&   (argFlags.mem)   &&  (!argFlags.reg))  || 
        ((!argFlags.constant)   &&  (!argFlags.mem)   &&  (!argFlags.reg)))
        return 0;
    return 1;
}

bool JumpArgsFilter(Flags argFlags) {
    if ((!argFlags.label)     || 
         (argFlags.string)    || 
         (argFlags.reg)       || 
         (argFlags.mem)       || 
         (argFlags.constant))
        return 0;
    return 1;
}

bool NoArgsFilter (Flags argFlags) {
    return 1;
}
