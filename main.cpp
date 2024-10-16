#include <stdio.h>
#include <string.h>
#include <math.h>

#include "processor.h"
#include "stack.h"

int main(int argc, char* argv[])
{
    PROCESSOR spu = {};

    Run(&spu, argc, argv[1]);

    return 0;
}
