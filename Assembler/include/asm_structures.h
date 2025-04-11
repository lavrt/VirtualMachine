#ifndef ASM_STRUCTURES_H
#define ASM_STRUCTURES_H

#include <stdio.h>

#include "instructions.h"
#include "stack.h"

static const int MAX_LENGTH_OF_LABELS = 64;
static const int MAX_NUMBER_OF_LABELS = 10;
static const int MAX_NUMBER_OF_REGISTERS = 8;

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
    char* instruction;
    int value;

    bool presence_label = false;
    char name_of_label[32]; // FIXME const

    bool presence_ram = false;
    size_t ram_address;

    bool presence_reg = false;
    enum REGISTERS name_of_register;
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
    struct Stack_t callStack;
};

#endif // ASM_STRUCTURES_H
