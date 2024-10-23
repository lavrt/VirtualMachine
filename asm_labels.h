#ifndef ASM_LABELS_H
#define ASM_LABELS_H

#include "asm_structures.h"

void LabelsCtor(Assembler* ASM);
int label_search(Assembler* ASM, const char* const name_of_potential_label);
void LabelsDtor(Assembler* ASM);

#endif // ASM_LABELS_H
