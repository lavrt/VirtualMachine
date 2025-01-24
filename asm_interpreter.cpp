#include "asm_interpreter.h"

#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "instructions.h"
#include "asm_labels.h"

// FIXME разбить на папки

// static --------------------------------------------------------------------------------------------------------------

static const char* NAME_OF_ASM_CODE_FILE = "asm_code_in.txt"; // FIXME добавб define DEBUG_PRINT
static const char* NAME_OF_MACHINE_CODE_FILE = "machine_code_in.txt";

static void interpreter(Assembler* const ASM);
static void displaySyntaxError(Assembler* const ASM);
static void сommandStreamCtor(Assembler* const ASM);
static enum REGISTERS checkRegisterName(const char* const name);
static enum INSTRUCTIONS checkCommandName(const char* const name);
static int setbit(const int value, const int position);

// global --------------------------------------------------------------------------------------------------------------

void asmCtor(Assembler* const ASM)
{
    assert(ASM);

    ASM->sntxerr = false;

    сommandStreamCtor(ASM);
    LabelsCtor(ASM);
}

void asmDtor(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);
    assert(ASM->cmd.instruction);

    LabelsDtor(ASM);

    FREE(ASM->commands.code);
    FREE(ASM->cmd.instruction);
}

void asmRun(Assembler* const ASM)
{
    assert(ASM);

    char string[32] = {};

    ASM->asm_file = fopen(NAME_OF_ASM_CODE_FILE, "rb");
    assert(ASM->asm_file);

    while (fgets(string, 32, ASM->asm_file))
    {
        if (strchr(string, '\n'))
        {
            *strchr(string, '\n') = '\0';
        }

        char* ptr1 = strtok(string, " ");

        if (string[0] == '\0')
        {
            continue;
        }
        else if (strchr(string, ':'))
        {
            ASM->cmd.presence_label = true;
            *strchr(string, ':') = '\0';
            strcpy(ASM->cmd.name_of_label, string);
        }
        else if (checkCommandName(ptr1))
        {
            strcpy(ASM->cmd.instruction, ptr1);

            if (!strcmp(ASM->cmd.instruction, PUSH) || !strcmp(ASM->cmd.instruction, POP))
            {
                char* ptr = strtok(NULL, " ");
                if (!ptr)
                {
                    displaySyntaxError(ASM);
                    assert(0);
                }

                if (strchr(ptr, '[') && strchr(ptr, ']'))
                {
                    ASM->cmd.presence_ram = true;
                    ASM->cmd.ram_address = atoi(strtok(strchr(ptr, '['), "[]"));
                }
                else if (ASM->cmd.name_of_register = checkRegisterName(ptr))
                {
                    ASM->cmd.presence_reg = true;
                }
                else if (atoi(ptr))
                {
                    ASM->cmd.value = atoi(ptr);
                }
            }
            else if (!strcmp(ASM->cmd.instruction, JMP)
                || !strcmp(ASM->cmd.instruction, JA) || !strcmp(ASM->cmd.instruction, JAE)
                || !strcmp(ASM->cmd.instruction, JB) || !strcmp(ASM->cmd.instruction, JBE)
                || !strcmp(ASM->cmd.instruction, JE) || !strcmp(ASM->cmd.instruction, JNE))
            {
                char* ptr = strtok(NULL, " ");
                if (!ptr)
                {
                    displaySyntaxError(ASM);
                    assert(0);
                }

                strcpy(ASM->cmd.name_of_label, ptr);
            }
        }
        else
        {
            displaySyntaxError(ASM);
            assert(0);
        }

        interpreter(ASM);

        ASM->cmd.presence_reg = false;
        ASM->cmd.presence_ram = false;
        ASM->cmd.presence_label = false;
        memset(ASM->cmd.instruction, '\0', 32);
    }

    FCLOSE(ASM->asm_file);

    ASM->code_file = fopen(NAME_OF_MACHINE_CODE_FILE, "wb");
    assert(ASM->code_file);

    fwrite(&ASM->commands.size, sizeof(int), 1, ASM->code_file);
    fwrite(ASM->commands.code, sizeof(int), ASM->commands.size, ASM->code_file);

    FCLOSE(ASM->code_file);
}

// static --------------------------------------------------------------------------------------------------------------

static void interpreter(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);
    assert(ASM->cmd.instruction);

    if (ASM->commands.capacity - ASM->commands.size <= 10)
    {
        ASM->commands.code = (int*)realloc(ASM->commands.code,
            (ASM->commands.size + ADD_SIZE_OF_CMD_ARRAY) * sizeof(int));
        ASM->commands.capacity += ADD_SIZE_OF_CMD_ARRAY;
        assert(ASM->commands.code);
    }

    ASM->current_labels.cmd_counter++;

    if (checkCommandName(ASM->cmd.instruction))
    {
        ASM->commands.code[ASM->commands.size++] = checkCommandName(ASM->cmd.instruction);

        if (!strcmp(ASM->cmd.instruction, PUSH))
        {
            if (ASM->cmd.presence_reg)
            {
                ASM->current_labels.cmd_counter++;
                ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_PUSH, USING_REGISTER);
                ASM->commands.code[ASM->commands.size++] = ASM->cmd.name_of_register;
                ASM->cmd.name_of_register = NO_REG;
            }
            else if (ASM->cmd.presence_ram)
            {
                ASM->current_labels.cmd_counter++;
                ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_PUSH, USING_RAM);
                ASM->commands.code[ASM->commands.size++] = ASM->cmd.ram_address;
            }
            else
            {
                ASM->current_labels.cmd_counter++;
                ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
            }
        }
        else if (!strcmp(ASM->cmd.instruction, POP))
        {
            if (ASM->cmd.presence_reg)
            {
                ASM->current_labels.cmd_counter++;
                ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_POP, USING_REGISTER);
                ASM->commands.code[ASM->commands.size++] = ASM->cmd.name_of_register;
                ASM->cmd.name_of_register = NO_REG;
            }
            else if (ASM->cmd.presence_ram)
            {
                ASM->current_labels.cmd_counter++;
                ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_POP, USING_RAM);
                ASM->commands.code[ASM->commands.size++] = ASM->cmd.ram_address;
            }
            else
            {
                ASM->current_labels.cmd_counter++;
                ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
            }
        }
        else if (!strcmp(ASM->cmd.instruction, IN))
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
        }
        else if (!strcmp(ASM->cmd.instruction, JMP)
                || !strcmp(ASM->cmd.instruction, JA) || !strcmp(ASM->cmd.instruction, JAE)
                || !strcmp(ASM->cmd.instruction, JB) || !strcmp(ASM->cmd.instruction, JBE)
                || !strcmp(ASM->cmd.instruction, JE) || !strcmp(ASM->cmd.instruction, JNE))
        {
            if (atoi(ASM->cmd.name_of_label))
            {
                ASM->commands.code[ASM->commands.size++] = atoi(ASM->cmd.name_of_label);
            }
            else
            {
                int index_of_label = labelSearch(ASM, ASM->cmd.name_of_label);
                if (index_of_label != -1)
                {
                    ASM->commands.code[ASM->commands.size++] = ASM->current_labels.labels[index_of_label].position;
                }
                else
                {
                    displaySyntaxError(ASM);
                    assert(0);
                }
            }
            ASM->current_labels.cmd_counter++;
        }
    }

    if (ASM->cmd.presence_label)
    {
        strcpy(ASM->current_labels.labels[ASM->current_labels.number_of_labels].name, ASM->cmd.name_of_label);
        ASM->current_labels.labels[ASM->current_labels.number_of_labels].position = ASM->current_labels.cmd_counter;
        ASM->current_labels.number_of_labels++;
    }
}

static void displaySyntaxError(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->cmd.instruction);

    ASM->sntxerr = true;
    fprintf(stderr, "Syntax error: \"%s\"\n", ASM->cmd.instruction);
}

static void сommandStreamCtor(Assembler* const ASM)
{
    assert(ASM);

    ASM->cmd.instruction = (char*)calloc(MAX_LENGTH_OF_INSTRUCTION, sizeof(char));
    assert(ASM->cmd.instruction);

    ASM->commands.code = (int*)calloc(ADD_SIZE_OF_CMD_ARRAY, sizeof(int));
    assert(ASM->commands.code);

    ASM->commands.capacity = ADD_SIZE_OF_CMD_ARRAY;
    ASM->commands.size = 0;
}

static enum REGISTERS checkRegisterName(const char* const name)
{
    assert(name);

         if (!strcmp(name, AX)) { return REG_AX; }
    else if (!strcmp(name, BX)) { return REG_BX; }
    else if (!strcmp(name, CX)) { return REG_CX; }
    else if (!strcmp(name, DX)) { return REG_DX; }
    else if (!strcmp(name, EX)) { return REG_EX; }
    else if (!strcmp(name, FX)) { return REG_FX; }
    else if (!strcmp(name, GX)) { return REG_GX; }
    else if (!strcmp(name, HX)) { return REG_HX; }
    else                        { return NO_REG; }
}

static enum INSTRUCTIONS checkCommandName(const char* const name)
{
    assert(name);

         if (!strcmp(name, HLT )) { return CMD_HLT ; }
    else if (!strcmp(name, PUSH)) { return CMD_PUSH; }
    else if (!strcmp(name, POP )) { return CMD_POP ; }
    else if (!strcmp(name, IN  )) { return CMD_IN  ; }
    else if (!strcmp(name, OUT )) { return CMD_OUT ; }
    else if (!strcmp(name, DUMP)) { return CMD_DUMP; }
    else if (!strcmp(name, ADD )) { return CMD_ADD ; }
    else if (!strcmp(name, SUB )) { return CMD_SUB ; }
    else if (!strcmp(name, MUL )) { return CMD_MUL ; }
    else if (!strcmp(name, DIV )) { return CMD_DIV ; }
    else if (!strcmp(name, SQRT)) { return CMD_SQRT; }
    else if (!strcmp(name, SIN )) { return CMD_SIN ; }
    else if (!strcmp(name, COS )) { return CMD_COS ; }
    else if (!strcmp(name, JMP )) { return CMD_JMP ; }
    else if (!strcmp(name, JA  )) { return CMD_JA  ; }
    else if (!strcmp(name, JAE )) { return CMD_JAE ; }
    else if (!strcmp(name, JB  )) { return CMD_JB  ; }
    else if (!strcmp(name, JBE )) { return CMD_JBE ; }
    else if (!strcmp(name, JE  )) { return CMD_JE  ; }
    else if (!strcmp(name, JNE )) { return CMD_JNE ; }
    else                          { return NO_CMD  ; }
}

static int setbit(const int value, const int position)
{
    assert(position >= 0);

    return value | (1 << position);
}
