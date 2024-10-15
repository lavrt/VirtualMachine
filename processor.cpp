#include "processor.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "stack.h"

void Run(void)
{
    Stack_t stk = {};
    STACKCTOR(&stk);

    while(1)
    {
        char cmd[50] = "";
        scanf("%s", cmd);

        if (!strcmp(cmd, "hlt"))
        {
            break;
        }
        if (!strcmp(cmd, "push"))
        {
            StackElem_t value = 0;
            scanf("%lg", &value);
            push(&stk, value);
            continue;
        }
        if (!strcmp(cmd, "pop"))
        {
            pop(&stk);
            continue;
        }
        if (!strcmp(cmd, "in"))
        {
            StackElem_t value = 0;
            scanf("%lg", &value);
            push(&stk, value);
            continue;
        }
        if (!strcmp(cmd, "out"))
        {
            StackElem_t value = pop(&stk);
            push(&stk, value);
            printf("%lg\n", value);
            continue;
        }
        if (!strcmp(cmd, "dump"))
        {
            StackDump(&stk, __FILE__, __LINE__, __func__);
            continue;
        }
        if (!strcmp(cmd, "add"))
        {
            StackElem_t a = pop(&stk);
            StackElem_t b = pop(&stk);
            push(&stk, a + b);
            continue;
        }
        if (!strcmp(cmd, "sub"))
        {
            StackElem_t a = pop(&stk);
            StackElem_t b = pop(&stk);
            push(&stk, b - a);
            continue;
        }
        if (!strcmp(cmd, "mul"))
        {
            StackElem_t a = pop(&stk);
            StackElem_t b = pop(&stk);
            push(&stk, a * b);
            continue;
        }
        if (!strcmp(cmd, "div"))
        {
            StackElem_t a = pop(&stk);
            StackElem_t b = pop(&stk);
            push(&stk, b / a);
            continue;
        }
        if (!strcmp(cmd, "sqrt"))
        {
            StackElem_t value = pop(&stk);
            push(&stk, sqrt(value));
            continue;
        }
        if (!strcmp(cmd, "sin"))
        {
            StackElem_t value = pop(&stk);
            push(&stk, sin(value));
            continue;
        }
        if (!strcmp(cmd, "cos"))
        {
            StackElem_t value = pop(&stk);
            push(&stk, cos(value));
            continue;
        }

        printf("SNTXERR: \"%s\"\n", cmd);
    }

    StackDtor(&stk);
}
