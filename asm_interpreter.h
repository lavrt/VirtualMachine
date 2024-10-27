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

void asmCtor(Assembler* ASM);
void asmRun(Assembler* ASM);
void interpreter(Assembler* ASM);
void display_syntax_error(Assembler* const ASM);
void CommandStreamCtor(Assembler* ASM);
void asmDtor(Assembler* ASM);
enum REGISTERS check_register_name(const char* const name);
bool check_command_name(const char* const name);
int setbit(const int value, const int position);

#endif // ASM_INTERPRETER_H
