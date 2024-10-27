#include "asm_labels.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

// FIXME константы, например LabelSystem* -> LabelSystem* const

void LabelsCtor(Assembler* const ASM)
{
    assert(ASM);

    ASM->current_labels.cmd_counter = 0;
    ASM->current_labels.number_of_labels = 0;

    for (int i = 0; i < MAX_NUMBER_OF_LABELS; i++) { ASM->current_labels.labels[i].position = -1; }
}

int label_search(const Assembler* const ASM, const char* const name_of_potential_label)
{
    assert(ASM);
    assert(name_of_potential_label);

    for (int i = 0; i < MAX_NUMBER_OF_LABELS; i++)
    {
        if (!strcmp(ASM->current_labels.labels[i].name, name_of_potential_label)) { return i; }
    }

    return -1;
}

void LabelsDtor(Assembler* const ASM)
{
    assert(ASM);

    ASM->current_labels.number_of_labels = 0;
    ASM->current_labels.cmd_counter = 0;
    for (int i = 0; i < MAX_NUMBER_OF_LABELS; i++)
    {
        ASM->current_labels.labels[i].position = 0;
        memset(ASM->current_labels.labels[i].name, 0, MAX_LENGTH_OF_LABELS);
    }
}
