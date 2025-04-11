#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "processor.h"
#include "stack.h"

static const char* DEFAULT_DATA_FILE = "machine_code_in.txt";

int main()
{
    PROCESSOR spu = {};

    FILE* data_file = fopen(DEFAULT_DATA_FILE, "rb");
    assert(data_file);

    spuCtor(&spu, data_file);

    spuRun(&spu);

    spuDtor(&spu);

    return 0;
}
