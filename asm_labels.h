#ifndef ASM_LABELS_H
#define ASM_LABELS_H

static const int   MAX_LENGTH_OF_LABELS = 10;
static const int   MAX_NUMBER_OF_LABELS = 10;

struct Label
{
    char name[MAX_LENGTH_OF_LABELS];
    int position;
};

struct LabelSystem
{
    int cmd_counter;
    int number_of_labels;
    struct Label labels[MAX_NUMBER_OF_LABELS];
};

void LabelsCtor(LabelSystem* current_labels);
int label_search(LabelSystem* current_labels, char* name_of_potential_label);
void LabelsDtor(LabelSystem* current_labels);

#endif // ASM_LABELS_H
