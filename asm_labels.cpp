#include "asm_labels.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

// FIXME константы, например LabelSystem* -> LabelSystem* const

void LabelsCtor(LabelSystem* current_labels)
{
    assert(current_labels);

    current_labels->cmd_counter = 0;
    current_labels->number_of_labels = 0;

    for (int i = 0; i < MAX_NUMBER_OF_LABELS; i++) { current_labels->labels[i].position = -1; }
}

int label_search(LabelSystem* current_labels, char* name_of_potential_label)
{
    assert(current_labels);
    assert(name_of_potential_label);

    for (int i = 0; i < MAX_NUMBER_OF_LABELS; i++)
    {
        if (!strcmp(current_labels->labels[i].name, name_of_potential_label)) { return i; }
    }
    return -1;
}

void LabelsDtor(LabelSystem* current_labels)
{
    assert(current_labels);

    current_labels->number_of_labels = 0;
    current_labels->cmd_counter = 0;
    for (int i = 0; i < MAX_NUMBER_OF_LABELS; i++)
    {
        current_labels->labels[i].position = 0;
        memset(current_labels->labels[i].name, 0, MAX_LENGTH_OF_LABELS);
    }
}
