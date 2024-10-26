#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>

#include "stack.h"
#include "instructions.h"

struct PROCESSOR
{
    int* code;
    size_t size;
    size_t ip;
    struct Stack_t stack;
    int registers[NUMBER_OF_REGISTERS];
    bool run;
};

static const double kEpsilon = 1e-6;
const int ADD_SIZE_OF_CMD_ARRAY = 50;

void spuCtor(PROCESSOR* spu, FILE* data_file);
void spuRun(PROCESSOR*);
void spuDtor(PROCESSOR* spu);
int numbers_equal(StackElem_t first_number, StackElem_t second_number);
constexpr int setbit(const int value, const int position);
//void spuDump(PROCESSOR* spu);

#endif // PROCESSOR_H
