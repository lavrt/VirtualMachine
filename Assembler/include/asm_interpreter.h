#ifndef ASM_INTERPRETER_H
#define ASM_INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>

#include "asm_structures.h"
#include "instructions.h"

const int ADD_SIZE_OF_CMD_ARRAY = 50;
const int MAX_LENGTH_OF_INSTRUCTION = 50;

#define FREE(ptr_) \
    do { free(ptr_); ptr_ = NULL; } while(0)
#define FCLOSE(file_) \
    do { fclose(file_); file_ = NULL; } while(0)

void asmCtor(Assembler* const ASM);
void twoPassCompilation(Assembler* const ASM);
void asmDtor(Assembler* const ASM);

#endif // ASM_INTERPRETER_H
