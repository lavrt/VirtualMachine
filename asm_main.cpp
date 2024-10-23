#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "asm_interpreter.h"

#define $ fprintf(stderr, "%s:%d in function: %s\n", __FILE__, __LINE__, __func__);

int main()
{
    Assembler ASM = {};
    asmCtor(&ASM);

    asmRun(&ASM);

    asmDtor(&ASM);

    return 0;
}
