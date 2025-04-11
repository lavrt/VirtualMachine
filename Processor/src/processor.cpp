#include "processor.h"

#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "stack.h"
#include "instructions.h"

// FIXME делай сложные аргмуенты push ax + 3 например

// static --------------------------------------------------------------------------------------------------------------

static void ReadData(PROCESSOR* spu, FILE* data_file);
static int numbersEqual(StackElem_t first_number, StackElem_t second_number);
static constexpr int setbit(const int value, const int position);

static void executePushFromRegister(PROCESSOR* spu);
static void executePopIntoRegister(PROCESSOR* spu);
static void executePushFromRam(PROCESSOR* spu);
static void executePopIntoRam(PROCESSOR* spu);
static void stopExecution(PROCESSOR* spu);
static void executeDump(PROCESSOR* spu);
static void executeSqrt(PROCESSOR* spu);
static void executeCall(PROCESSOR* spu);
static void executePush(PROCESSOR* spu);
static void executePop(PROCESSOR* spu);
static void executeOut(PROCESSOR* spu);
static void executeAdd(PROCESSOR* spu);
static void executeSub(PROCESSOR* spu);
static void executeMul(PROCESSOR* spu);
static void executeDiv(PROCESSOR* spu);
static void executeSin(PROCESSOR* spu);
static void executeCos(PROCESSOR* spu);
static void executeJmp(PROCESSOR* spu);
static void executeJae(PROCESSOR* spu);
static void executeJbe(PROCESSOR* spu);
static void executeJne(PROCESSOR* spu);
static void executeRet(PROCESSOR* spu);
static void executeHlt(PROCESSOR* spu);
static void executeIn(PROCESSOR* spu);
static void executeJa(PROCESSOR* spu);
static void executeJe(PROCESSOR* spu);
static void executeJb(PROCESSOR* spu);

// global --------------------------------------------------------------------------------------------------------------

void spuCtor(PROCESSOR* spu, FILE* data_file)
{
    assert(spu);
    assert(data_file);

    STACKCTOR(&spu->stack);

    spu->code = NULL;
    spu->size = 0;
    spu->ip = 0;
    spu->run = true;

    spu->ram = (StackElem_t*)calloc(AMOUNT_OF_RAM, sizeof(StackElem_t));

    ReadData(spu, data_file);
}

void spuDtor(PROCESSOR* spu)
{
    StackDtor(&spu->stack);

    FREE(spu->ram);
    FREE(spu->code);

    spu->size = 0;
    spu->ip = 0;
}

// static --------------------------------------------------------------------------------------------------------------

static constexpr int setbit(const int value, const int position)
{
    assert(position >= 0);

    return (value | (1 << position));
}

// global --------------------------------------------------------------------------------------------------------------

void spuRun(PROCESSOR* spu)
{
    assert(spu);

    while(spu->run)
    {
        switch(spu->code[spu->ip])
        {
            case setbit(CMD_PUSH, USING_REGISTER): executePushFromRegister(spu); break;
            case setbit(CMD_POP , USING_REGISTER): executePopIntoRegister(spu);  break;
            case setbit(CMD_PUSH, USING_RAM):      executePushFromRam(spu);      break;
            case setbit(CMD_POP , USING_RAM):      executePopIntoRam(spu);       break;

            case CMD_HLT:  executeHlt(spu);  break;
            case CMD_PUSH: executePush(spu); break;
            case CMD_POP:  executePop(spu);  break;
            case CMD_IN:   executeIn(spu);   break;
            case CMD_OUT:  executeOut(spu);  break;
            case CMD_DUMP: executeDump(spu); break;
            case CMD_ADD:  executeAdd(spu);  break;
            case CMD_SUB:  executeSub(spu);  break;
            case CMD_MUL:  executeMul(spu);  break;
            case CMD_DIV:  executeDiv(spu);  break;
            case CMD_SQRT: executeSqrt(spu); break;
            case CMD_SIN:  executeSin(spu);  break;
            case CMD_COS:  executeCos(spu);  break;
            case CMD_JMP:  executeJmp(spu);  break;
            case CMD_JA:   executeJa(spu);   break;
            case CMD_JAE:  executeJae(spu);  break;
            case CMD_JB:   executeJb(spu);   break;
            case CMD_JBE:  executeJbe(spu);  break;
            case CMD_JE:   executeJe(spu);   break;
            case CMD_JNE:  executeJne(spu);  break;
            case CMD_RET:  executeRet(spu);  break;
            case CMD_CALL: executeCall(spu); break;

            default: stopExecution(spu);
        }
    }
}

// static --------------------------------------------------------------------------------------------------------------

static void ReadData(PROCESSOR* spu, FILE* data_file)
{
    assert(spu);
    assert(data_file);

    fread(&spu->size, sizeof(int), 1, data_file);

    spu->code = (int*)calloc(spu->size, sizeof(int));
    assert(spu->code);

    fread(spu->code, sizeof(int), spu->size, data_file);

    FCLOSE(data_file);
}

static void executeHlt(PROCESSOR* spu)
{
    spu->run = false;
}

static void executePush(PROCESSOR* spu)
{
    StackElem_t value = spu->code[spu->ip + 1];
    push(&spu->stack, value);
    spu->ip += 2;
}

static void executePushFromRegister(PROCESSOR* spu)
{
    int register_number = spu->code[spu->ip + 1] - 1;
    push(&spu->stack, spu->registers[register_number]);
    spu->ip += 2;
}

static void executePushFromRam(PROCESSOR* spu)
{
    StackElem_t value = spu->ram[spu->code[spu->ip + 1]];
    push(&spu->stack, value);
    spu->ip += 2;
}

static void executePop(PROCESSOR* spu)
{
    pop(&spu->stack);
    spu->ip += 1;
}

static void executePopIntoRegister(PROCESSOR* spu)
{
    int register_number = spu->code[spu->ip + 1] - 1;
    spu->registers[register_number] = pop(&spu->stack);
    spu->ip += 2;
}

static void executePopIntoRam(PROCESSOR* spu)
{
    StackElem_t value = pop(&spu->stack);
    spu->ram[spu->code[spu->ip + 1]] = value;
    spu->ip += 2;
}

static void executeIn(PROCESSOR* spu)
{
    StackElem_t value = spu->code[spu->ip + 1];
    push(&spu->stack, value);
    spu->ip += 2;
}

static void executeOut(PROCESSOR* spu)
{
    StackElem_t value = pop(&spu->stack);
    push(&spu->stack, value);
    printf("%lg\n", value);
    spu->ip += 1;
}

static void executeDump(PROCESSOR* spu) // FIXME дамп не только стека
{
    StackDump(&spu->stack, __FILE__, __LINE__, __func__);
    spu->ip += 1;
}

static void executeAdd(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    push(&spu->stack, a + b);
    spu->ip += 1;
}

static void executeSub(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    push(&spu->stack, b - a);
    spu->ip += 1;
}

static void executeMul(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    push(&spu->stack, a * b);
    spu->ip += 1;
}

static void executeDiv(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    assert(a != 0);
    push(&spu->stack, b / a);
    spu->ip += 1;
}

static void executeSqrt(PROCESSOR* spu)
{
    StackElem_t value = pop(&spu->stack);
    assert(value >= 0);
    push(&spu->stack, sqrt(value));
    spu->ip += 1;
}

static void executeSin(PROCESSOR* spu)
{
    StackElem_t value = pop(&spu->stack);
    push(&spu->stack, sin(value));
    spu->ip += 1;
}

static void executeCos(PROCESSOR* spu)
{
    StackElem_t value = pop(&spu->stack);
    push(&spu->stack, cos(value));
    spu->ip += 1;
}

static void executeJmp(PROCESSOR* spu)
{
    spu->ip = (unsigned)spu->code[spu->ip + 1];
}

static void executeJa(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    if (b > a)
    {
        spu->ip = (unsigned)spu->code[spu->ip + 1];
    }
    else
    {
        spu->ip += 2;
    }
}

static void executeJae(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    if (b >= a)
    {
        spu->ip = (unsigned)spu->code[spu->ip + 1];
    }
    else
    {
        spu->ip += 2;
    }
}

static void executeJb(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    if (b < a)
    {
        spu->ip = (unsigned)spu->code[spu->ip + 1];
    }
    else
    {
        spu->ip += 2;
    }
}

static void executeJbe(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    if (b <= a)
    {
        spu->ip = (unsigned)spu->code[spu->ip + 1];
    }
    else
    {
        spu->ip += 2;
    }
}

static void executeJe(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    if (numbersEqual(a, b))
    {
        spu->ip = (unsigned)spu->code[spu->ip + 1];
    }
    else
    {
        spu->ip += 2;
    }
}

static void executeJne(PROCESSOR* spu)
{
    StackElem_t a = pop(&spu->stack);
    StackElem_t b = pop(&spu->stack);
    if (!numbersEqual(a, b))
    {
        spu->ip = (unsigned)spu->code[spu->ip + 1];
    }
    else
    {
        spu->ip += 2;
    }
}

static void executeRet(PROCESSOR* spu)
{
    assert(spu);

    int value = pop(&spu->stack);
    spu->ip = value;
}

static void executeCall(PROCESSOR* spu)
{
    assert(spu);

    push(&spu->stack, spu->ip + 2);
    spu->ip = (unsigned)spu->code[spu->ip + 1];
}

static void stopExecution(PROCESSOR* spu)
{
    fprintf(stderr, "INVALID INSTRUCTION: \"%d\"\n", spu->code[spu->ip]);
    spu->run = false;
    assert(0);
}

static int numbersEqual(StackElem_t first_number, StackElem_t second_number)
{
    return fabs(first_number - second_number) < kEpsilon;
}



