#ifndef ASM_INTERPRETER_H
#define ASM_INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>

#include "asm_labels.h"

// FIXME изменить CurrentCommand
struct CurrentCommand
{
    FILE* code_file;
    FILE* asm_file;
    char instruction[50]; // FIXME динамически выделять и лучше использовать не char а uint8_t
    int value; // FIXME
    int number_of_argument;
    bool sntxerr;
};

// FIXME добавить Assembler струтктуру, которая хранит файлы, массив меток, команд стрим ..., написать для него ctor dtor

struct CommandStream
{
    int* code;
    int size;
    int capacity;
};

const int ADD_SIZE_OF_CMD_ARRAY = 50;

#define FREE(ptr_) \
    do { free(ptr_); ptr_ = NULL; } while(0)

void asmRun(CurrentCommand* cmd, LabelSystem* current_labels, CommandStream* commands);
void interpreter(CurrentCommand* cmd, LabelSystem* current_labels, CommandStream* commands);
void display_syntax_error(CurrentCommand* cmd);
void CommandStreamCtor(CommandStream* commands);
void memory_expansion(CommandStream* commands);
void CommandStreamDtor(CommandStream* commands);

#endif // ASM_INTERPRETER_H
