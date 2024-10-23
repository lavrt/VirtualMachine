#include "processor.h"

#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "stack.h"
#include "instructions.h"

// FIXME делай сложные аргмуенты push ax + 3 например

static void ReadData(PROCESSOR* spu, FILE* data_file);

void spuCtor(PROCESSOR* spu, FILE* data_file)
{
    Stack_t stk = {};
    STACKCTOR(&stk);

    spu->code = NULL;
    spu->size = 0;
    spu->ip = 0;
    spu->stack = stk;
    spu->run = true;

    ReadData(spu, data_file);
}

static void ReadData(PROCESSOR* spu, FILE* data_file)
{
    assert(spu);
    assert(data_file);

    fread(&spu->size, sizeof(int), 1, data_file);

    int* code = (int*)calloc(spu->size + 0, sizeof(int));
    assert(code);
    spu->code = code;

    fread(spu->code, sizeof(int), spu->size, data_file);

    FCLOSE(data_file);
}

void spuRun(PROCESSOR* spu)
{
    assert(spu);

    while(spu->run)
    { // printf("__%d__\n", spu->ip);
        switch(spu->code[spu->ip])
        {
            // NOTE мб вынести каждый кейс в фукнцию и вызывать
            case CMD_HLT:
            {
                spu->run = false;
                break;
            }
            case CMD_PUSH:
            {
                StackElem_t value = spu->code[spu->ip + 1];
                push(&spu->stack, value);
                spu->ip += 2;
                break;
            }
            case CMD_POP:
            {
                pop(&spu->stack);
                spu->ip += 1;
                break;
            }
            case CMD_IN:
            {
                StackElem_t value = 0;
                scanf("%lg", &value);
                push(&spu->stack, value);
                spu->ip += 2;
                break;
            }
            case CMD_OUT:
            {
                StackElem_t value = pop(&spu->stack);
                push(&spu->stack, value);
                printf("%lg\n", value);
                spu->ip += 1;
                break;
            }
            case CMD_DUMP:
            { // FIXME дамп не только стека
                StackDump(&spu->stack, __FILE__, __LINE__, __func__);
                spu->ip += 1;
                break;
            }
            case CMD_ADD:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                push(&spu->stack, a + b);
                spu->ip += 1;
                break;
            }
            case CMD_SUB:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                push(&spu->stack, b - a);
                spu->ip += 1;
                break;
            }
            case CMD_MUL:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                push(&spu->stack, a * b);
                spu->ip += 1;
                break;
            }
            case CMD_DIV:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                assert(a != 0);
                push(&spu->stack, b / a);
                spu-> ip += 1;
                break;
            }
            case CMD_SQRT:
            {
                StackElem_t value = pop(&spu->stack);
                assert(value >= 0);
                push(&spu->stack, sqrt(value));
                spu->ip += 1;
                break;
            }
            case CMD_SIN:
            {
                StackElem_t value = pop(&spu->stack);
                push(&spu->stack, sin(value));
                spu->ip += 1;
                break;
            }
            case CMD_COS:
            {
                StackElem_t value = pop(&spu->stack);
                push(&spu->stack, cos(value));
                spu->ip += 1;
                break;
            }
            case CMD_JMP:
            {
                spu->ip = (unsigned)spu->code[spu->ip + 1];
                break;
            }
            case CMD_JA:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                if (b > a) { spu->ip = (unsigned)spu->code[spu->ip + 1]; }
                else       { spu->ip += 2; }
                break;
            }
            case CMD_JAE:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                if (b >= a) { spu->ip = (unsigned)spu->code[spu->ip + 1]; }
                else        { spu->ip += 2; }
                break;
            }
            case CMD_JB:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                if (b < a) { spu->ip = (unsigned)spu->code[spu->ip + 1]; }
                else       { spu->ip += 2; }
                break;
            }
            case CMD_JBE:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                if (b <= a) { spu->ip = (unsigned)spu->code[spu->ip + 1]; }
                else        { spu->ip += 2; }
                break;
            }
            case CMD_JE:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                if (numbers_equal(a, b)) { spu->ip = (unsigned)spu->code[spu->ip + 1]; }
                else        { spu->ip += 2; }
                break;
            }
            case CMD_JNE:
            {
                StackElem_t a = pop(&spu->stack);
                StackElem_t b = pop(&spu->stack);
                if (!numbers_equal(a, b)) { spu->ip = (unsigned)spu->code[spu->ip + 1]; }
                else                      { spu->ip += 2; }
                break;
            }
            default:
            {
                fprintf(stderr, "INVALID INSTRUCTION: \"%d\"\n", spu->code[spu->ip]);
                spu->run = false;
                assert(0);
            }
        }
    }
}

void spuDtor(PROCESSOR* spu)
{
    StackDtor(&spu->stack);

    FREE(spu->code);
    spu->size = 0;
    spu->ip = 0;
}

int numbers_equal(StackElem_t first_number, StackElem_t second_number)
{
    return fabs(first_number - second_number) < kEpsilon;
}

// void spuDump(PROCESSOR* spu)
// {
//     for (int i = 0; i < spu->size; i++)
//     {
//         printf("__%d__\n", spu->code[i]);
//     }
//     putchar('\n');
// }

