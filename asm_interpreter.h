#ifndef ASM_INTERPRETER_H
#define ASM_INTERPRETER_H

#include <stdio.h>

#include "asm_labels.h"

struct CurrentCommand
{
    FILE* code_file;
    FILE* asm_file;
    char instruction[50];
    int value;
    int number_of_argument;
    bool sntxerr;
};

void asmRun(CurrentCommand* cmd, LabelSystem* current_labels);
void interpreter(CurrentCommand* cmd, LabelSystem* current_labels);
void display_syntax_error(CurrentCommand* cmd);

#endif // ASM_INTERPRETER_H
