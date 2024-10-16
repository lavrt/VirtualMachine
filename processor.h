#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>

enum INSTRUCTIONS
{
    CMD_HLT  = 0,
    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_IN   = 3,
    CMD_OUT  = 4,
    CMD_DUMP = 5,
    CMD_ADD  = 6,
    CMD_SUB  = 7,
    CMD_MUL  = 8,
    CMD_DIV  = 9,
    CMD_SQRT = 10,
    CMD_SIN  = 11,
    CMD_COS  = 12,
};

struct PROCESSOR
{
    int* code;
    size_t size;
    size_t ip;
    struct Stack_t* stack;
    int* registers;
};

static const int ADD_SIZE_OF_CMD_ARRAY = 50;
static const char* DEFAULT_DATA_FILE = "default.txt";

void Run(PROCESSOR* spu, int argc, char* argv);
void ReadData(PROCESSOR* spu, int argc, char* argv);

#endif // PROCESSOR_H
