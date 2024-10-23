#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "processor.h"
#include "stack.h"

static const char* DEFAULT_DATA_FILE = "default_machine_code_in.txt";

int main(const int argc, const char* argv[])
{
    PROCESSOR spu = {};

    FILE* data_file = NULL;
    if (argc == 1) { data_file = fopen(DEFAULT_DATA_FILE, "rb"); }
    if (argc >= 2) { data_file = fopen(argv[1], "rb"); }
    // FILE* data_file = fopen(argc >= 2 ? argv[1] : DEFAULT_DATA_FILE, "rb");
    assert(data_file);

    spuCtor(&spu, data_file);
    spuRun(&spu);
    spuDtor(&spu);

    return 0;
}
