#include "processor.h"

#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "stack.h"

void Run(PROCESSOR* spu, int argc, char* argv)
{
    Stack_t stk = {};
    STACKCTOR(&stk);

    ReadData(spu, argc, argv);

    int ip = 0;
    bool run = true;
    while(run)
    {
        switch(spu->code[ip++])
        {
            case CMD_HLT:
            {
                run = false;
                break;
            }
            case CMD_PUSH:
            {
                StackElem_t value = spu->code[ip++];
                push(&stk, value);
                break;
            }
            case CMD_POP:
            {
                pop(&stk);
                break;
            }
            case CMD_IN:
            {
                StackElem_t value = spu->code[ip++];
                push(&stk, value);
                break;
            }
            case CMD_OUT:
            {
                StackElem_t value = pop(&stk);
                push(&stk, value);
                printf("%lg\n", value);
                break;
            }
            case CMD_DUMP:
            {
                StackDump(&stk, __FILE__, __LINE__, __func__);
                break;
            }
            case CMD_ADD:
            {
                StackElem_t a = pop(&stk);
                StackElem_t b = pop(&stk);
                push(&stk, a + b);
                break;
            }
            case CMD_SUB:
            {
                StackElem_t a = pop(&stk);
                StackElem_t b = pop(&stk);
                push(&stk, b - a);
                break;
            }
            case CMD_MUL:
            {
                StackElem_t a = pop(&stk);
                StackElem_t b = pop(&stk);
                push(&stk, a * b);
                break;
            }
            case CMD_DIV:
            {
                StackElem_t a = pop(&stk);
                StackElem_t b = pop(&stk);
                push(&stk, b / a);
                break;
            }
            case CMD_SQRT:
            {
                StackElem_t value = pop(&stk);
                push(&stk, sqrt(value));
                break;
            }
            case CMD_SIN:
            {
                StackElem_t value = pop(&stk);
                push(&stk, sin(value));
                break;
            }
            case CMD_COS:
            {
                StackElem_t value = pop(&stk);
                push(&stk, cos(value));
                break;
            }
            default:
            {
                printf("INVALID INSTRUCTION: \"%d\"\n", spu->code[ip - 1]);
                run = false;
            }
        }
    }

    FREE(spu->code);
    StackDtor(&stk);
}

void ReadData(PROCESSOR* spu, int argc, char* argv)
{
    FILE * data_file = (argc == 1) ? fopen(DEFAULT_DATA_FILE, "rb") : fopen(argv, "rb");
    if (!data_file) { assert(0); } // TODO сделать ноrrrrmальную проверку

    assert(ADD_SIZE_OF_CMD_ARRAY > 0);
    int * code = (int*)calloc(ADD_SIZE_OF_CMD_ARRAY, sizeof(int));
    if (!code) { assert(0); } // TODO нормальную проверку

    spu->code = code;
    spu->size = 0;

    size_t size_of_cmd_array = ADD_SIZE_OF_CMD_ARRAY;
    for (int i = 0; fscanf(data_file, "%d", &code[i]) == 1; i++)
    {
        spu->size++;
        if (spu->size == size_of_cmd_array)
        {
            size_of_cmd_array += ADD_SIZE_OF_CMD_ARRAY;
            code = (int*)realloc(code, size_of_cmd_array * sizeof(int));
            if (!code) { assert(0); } // TODO нормальную првоерку
            spu->code = code;
        }
    }

    FCLOSE(data_file);
}
