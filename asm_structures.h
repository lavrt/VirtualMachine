#ifndef ASM_STRUCTURES_H
#define ASM_STRUCTURES_H

#include "stdio.h"

static const int MAX_LENGTH_OF_LABELS = 10;
static const int MAX_NUMBER_OF_LABELS = 10;

struct Label
{
    char name[MAX_LENGTH_OF_LABELS];
    int position;
};

struct LabelSystem
{
    int cmd_counter;
    int number_of_labels;
    struct Label labels[MAX_NUMBER_OF_LABELS];
};

struct CurrentCommand
{
    char instruction[50]; // FIXME динамически выделять и лучше использовать не char а uint8_t
    int value;
    int number_of_argument;
};

struct CommandStream
{
    int* code;
    int size;
    int capacity;
};

struct Assembler
{
    FILE* code_file;
    FILE* asm_file;
    bool sntxerr;
    struct CommandStream commands;
    struct CurrentCommand cmd;
    struct LabelSystem current_labels;
};

#endif // ASM_STRUCTURES_H
