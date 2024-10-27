#ifndef ASM_LABELS_H
#define ASM_LABELS_H

#include "asm_structures.h"

void LabelsCtor(Assembler* const ASM);
int labelSearch(const Assembler* const ASM, const char* const name_of_potential_label);
void LabelsDtor(Assembler* const ASM);

#endif // ASM_LABELS_H
