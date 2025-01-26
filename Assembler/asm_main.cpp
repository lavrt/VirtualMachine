#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "asm_interpreter.h"

int main()
{
    Assembler ASM = {};

    asmCtor(&ASM);

    twoPassCompilation(&ASM);

    asmDtor(&ASM);

    return 0;
}
