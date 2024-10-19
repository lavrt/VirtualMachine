#include <stdio.h>
#include <string.h>
#include <math.h>

#include "processor.h"
#include "stack.h"

int main(const int argc, const char* argv[])
{
    PROCESSOR spu = {};

    spuCtor(&spu, argc, argv[1]);
    spuRun(&spu);
    spuDtor(&spu);

    return 0;
}
