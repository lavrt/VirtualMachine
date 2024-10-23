#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "asm_interpreter.h"
#include "asm_labels.h"

#define FCLOSE(file_) \
    do { fclose(file_); file_ = NULL; } while(0)
#define $ fprintf(stderr, "%s:%d in function: %s\n", __FILE__, __LINE__, __func__);

static const char* NAME_OF_ASM_CODE_FILE = "asm_code_in.txt"; // FIXME добавб define DEBUG_PRINT
static const char* NAME_OF_MACHINE_CODE_FILE = "machine_code_in.txt";

int main()
{
    CurrentCommand cmd = {};
    cmd.asm_file = fopen(NAME_OF_ASM_CODE_FILE, "rb");
    assert(cmd.asm_file);
    cmd.code_file = fopen(NAME_OF_MACHINE_CODE_FILE, "wb");
    assert(cmd.code_file);

    CommandStream commands = {};
    CommandStreamCtor(&commands);
    LabelSystem current_labels = {};
    LabelsCtor(&current_labels);

    asmRun(&cmd, &current_labels, &commands);

    LabelsDtor(&current_labels);
    CommandStreamDtor(&commands);
    FCLOSE(cmd.asm_file);
    FCLOSE(cmd.code_file);

    return 0;
}
