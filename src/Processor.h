#pragma once

#define   TX_CONSOLE_MODE SW_SHOW
#include <TXLib.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>

#include "stack.h"
#include "Utilities.h"
#include "Text.h"
#include "commands.h"

void ReadCommands(int argc, char* argv[], Text* text);
bool ExecuteCommands(Text* commands, Stack* procStack, Stack* retStack);
void ValidateSignature(Text* text);

//This define calculates shift of command pointer, execute code and shifts                                                                                       

