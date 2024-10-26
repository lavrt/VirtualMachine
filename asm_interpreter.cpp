#include "asm_interpreter.h"

#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "instructions.h"
#include "asm_labels.h"

static const char* NAME_OF_ASM_CODE_FILE = "asm_code_in.txt"; // FIXME добавб define DEBUG_PRINT
static const char* NAME_OF_MACHINE_CODE_FILE = "machine_code_in.txt";

#define $ fprintf(stderr, "%s:%d in function: %s\n", __FILE__, __LINE__, __func__);
// FIXME разбить на папки

void asmCtor(Assembler* ASM)
{
    assert(ASM);

    ASM->code_file = fopen(NAME_OF_MACHINE_CODE_FILE, "wb");
    ASM->asm_file = fopen(NAME_OF_ASM_CODE_FILE, "rb");

    CommandStreamCtor(ASM);
    LabelsCtor(ASM);

    assert(ASM->code_file);
    assert(ASM->asm_file);
}

void asmRun(Assembler* ASM)
{
    assert(ASM);

    ASM->sntxerr = false;
    int eof_indicator = 0;
    bool instruction_not_read = true;

    while(!ASM->sntxerr && eof_indicator != EOF && ASM->cmd.number_of_argument != EOF)
    {
        if (instruction_not_read) { eof_indicator = fscanf(ASM->asm_file, "%s", ASM->cmd.instruction); }
        ASM->cmd.number_of_argument = fscanf(ASM->asm_file, "%d", &ASM->cmd.value);
        instruction_not_read = true;
        if (eof_indicator == EOF) { break; }

        if (!strcmp(ASM->cmd.instruction, PUSH) && ASM->cmd.number_of_argument == 0)
        {
            if (!fscanf(ASM->asm_file, "%s", ASM->cmd.name_of_register)) { assert(0); }
        }
        if (!strcmp(ASM->cmd.instruction, POP))
        {
            fscanf(ASM->asm_file, "%s", ASM->cmd.instruction);
            if (check_register_name(ASM->cmd.instruction))
            {
                strcpy(ASM->cmd.name_of_register, ASM->cmd.instruction);
                strcpy(ASM->cmd.instruction, POP);
            }
            else
            {
                ASM->commands.code[ASM->commands.size++] = CMD_POP;
                instruction_not_read = false;
                continue;
            }
        }

        if (eof_indicator != EOF)
        {
            interpreter(ASM);
        }
    }

    fwrite(&ASM->commands.size, sizeof(int), 1, ASM->code_file);
    fwrite(ASM->commands.code, sizeof(int), ASM->commands.size, ASM->code_file);
}

void interpreter(Assembler* ASM)
{
    assert(ASM);

    if (ASM->commands.size == ASM->commands.capacity)
    {
        ASM->commands.code = (int*)realloc(ASM->commands.code, ASM->commands.size + ADD_SIZE_OF_CMD_ARRAY);
        ASM->commands.capacity += ADD_SIZE_OF_CMD_ARRAY;
    }

    ASM->current_labels.cmd_counter++;
    while(true)
    {
        if (!strcmp(ASM->cmd.instruction, HLT )) { ASM->commands.code[ASM->commands.size++] = CMD_HLT ; break; }
        if (!strcmp(ASM->cmd.instruction, PUSH)) { ASM->commands.code[ASM->commands.size++] = CMD_PUSH; break; }
        if (!strcmp(ASM->cmd.instruction, POP )) { ASM->commands.code[ASM->commands.size++] = CMD_POP ; break; }
        if (!strcmp(ASM->cmd.instruction, IN  )) { ASM->commands.code[ASM->commands.size++] = CMD_IN  ; break; }
        if (!strcmp(ASM->cmd.instruction, OUT )) { ASM->commands.code[ASM->commands.size++] = CMD_OUT ; break; }
        if (!strcmp(ASM->cmd.instruction, DUMP)) { ASM->commands.code[ASM->commands.size++] = CMD_DUMP; break; }
        if (!strcmp(ASM->cmd.instruction, ADD )) { ASM->commands.code[ASM->commands.size++] = CMD_ADD ; break; }
        if (!strcmp(ASM->cmd.instruction, SUB )) { ASM->commands.code[ASM->commands.size++] = CMD_SUB ; break; }
        if (!strcmp(ASM->cmd.instruction, MUL )) { ASM->commands.code[ASM->commands.size++] = CMD_MUL ; break; }
        if (!strcmp(ASM->cmd.instruction, DIV )) { ASM->commands.code[ASM->commands.size++] = CMD_DIV ; break; }
        if (!strcmp(ASM->cmd.instruction, SQRT)) { ASM->commands.code[ASM->commands.size++] = CMD_SQRT; break; }
        if (!strcmp(ASM->cmd.instruction, SIN )) { ASM->commands.code[ASM->commands.size++] = CMD_SIN ; break; }
        if (!strcmp(ASM->cmd.instruction, COS )) { ASM->commands.code[ASM->commands.size++] = CMD_COS ; break; }
        if (!strcmp(ASM->cmd.instruction, JMP )) { ASM->commands.code[ASM->commands.size++] = CMD_JMP ; break; }
        if (!strcmp(ASM->cmd.instruction, JA  )) { ASM->commands.code[ASM->commands.size++] = CMD_JA  ; break; }
        if (!strcmp(ASM->cmd.instruction, JAE )) { ASM->commands.code[ASM->commands.size++] = CMD_JAE ; break; }
        if (!strcmp(ASM->cmd.instruction, JB  )) { ASM->commands.code[ASM->commands.size++] = CMD_JB  ; break; }
        if (!strcmp(ASM->cmd.instruction, JBE )) { ASM->commands.code[ASM->commands.size++] = CMD_JBE ; break; }
        if (!strcmp(ASM->cmd.instruction, JE  )) { ASM->commands.code[ASM->commands.size++] = CMD_JE  ; break; }
        if (!strcmp(ASM->cmd.instruction, JNE )) { ASM->commands.code[ASM->commands.size++] = CMD_JNE ; break; }
        if ( strchr(ASM->cmd.instruction, ':' )) { ASM->current_labels.cmd_counter--                  ; break; }

        display_syntax_error(ASM);
        assert(0);
    }

    if (!strcmp(ASM->cmd.instruction, PUSH))
    {
        if (ASM->cmd.number_of_argument == 0 && !check_register_name(ASM->cmd.name_of_register))
        {
            display_syntax_error(ASM);
            assert(0);
        }
        if (ASM->cmd.number_of_argument == 0 && check_register_name(ASM->cmd.name_of_register))
        {
            ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_PUSH, USING_REGISTER);
            ASM->commands.code[ASM->commands.size++] = check_register_name(ASM->cmd.name_of_register);
        }
        else
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
        }
    }
    if (!strcmp(ASM->cmd.instruction, POP))
    {
        if (ASM->cmd.number_of_argument == 0 && check_register_name(ASM->cmd.name_of_register))
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_POP, USING_REGISTER);
            ASM->commands.code[ASM->commands.size++] = check_register_name(ASM->cmd.name_of_register);
        }
        else
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size++] = CMD_POP;
        }
    }
    if (!strcmp(ASM->cmd.instruction, IN))
    {
        if (ASM->cmd.number_of_argument != 1)
        {
            display_syntax_error(ASM);
            assert(0);
        }
        ASM->current_labels.cmd_counter++;
        ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
    }
    if (!strcmp(ASM->cmd.instruction, JMP)
        || !strcmp(ASM->cmd.instruction, JA) || !strcmp(ASM->cmd.instruction, JAE)
        || !strcmp(ASM->cmd.instruction, JB) || !strcmp(ASM->cmd.instruction, JBE)
        || !strcmp(ASM->cmd.instruction, JE) || !strcmp(ASM->cmd.instruction, JNE))
    {
        char name_of_potential_label[MAX_LENGTH_OF_LABELS] = "";
        if (ASM->cmd.number_of_argument == 1)
        {
            ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
        }
        else if (fscanf(ASM->asm_file, "%s", name_of_potential_label) == EOF)
        {
            assert(0);
        }
        else if (strchr(name_of_potential_label, ':'))
        {
            int index_of_label = label_search(ASM, name_of_potential_label);
            if (index_of_label != -1)
            {
                ASM->commands.code[ASM->commands.size++] = ASM->current_labels.labels[index_of_label].position;
            }
            else
            {
                display_syntax_error(ASM);
                assert(0);
            }
        }
        else
        {
            assert(0);
        }
        ASM->current_labels.cmd_counter++;
    }
    if (strchr(ASM->cmd.instruction, ':'))
    {
        strcpy(ASM->current_labels.labels[ASM->current_labels.number_of_labels].name, ASM->cmd.instruction);
        ASM->current_labels.labels[ASM->current_labels.number_of_labels].position = ASM->current_labels.cmd_counter;
        ASM->current_labels.number_of_labels++;
    }
}
// FIXME нет ассертов и констант модификаторов
void display_syntax_error(Assembler* ASM)
{
    assert(ASM);

    ASM->sntxerr = true;
    fprintf(stderr,         "Syntax error: \"%s\"\n", ASM->cmd.instruction);
    fprintf(ASM->code_file, "Syntax error: \"%s\"\n", ASM->cmd.instruction);
}

void CommandStreamCtor(Assembler* ASM)
{
    assert(ASM);

    ASM->commands.code = (int*)calloc(ADD_SIZE_OF_CMD_ARRAY, sizeof(int));
    ASM->commands.capacity = ADD_SIZE_OF_CMD_ARRAY;
    ASM->commands.size = 0;

    assert(ASM->commands.code);
}

void asmDtor(Assembler* ASM)
{
    assert(ASM);
    assert(ASM->commands.code);

    LabelsDtor(ASM);
    FREE(ASM->commands.code);

    FCLOSE(ASM->asm_file);
    FCLOSE(ASM->code_file);
}

enum REGISTERS check_register_name(const char* const name)
{
    assert(name);

    if      (!strcmp(name, AX)) { return REG_AX; }
    else if (!strcmp(name, BX)) { return REG_BX; }
    else if (!strcmp(name, CX)) { return REG_CX; }
    else if (!strcmp(name, DX)) { return REG_DX; }
    else if (!strcmp(name, EX)) { return REG_EX; }
    else if (!strcmp(name, FX)) { return REG_FX; }
    else if (!strcmp(name, GX)) { return REG_GX; }
    else if (!strcmp(name, HX)) { return REG_HX; }
    else                        { return NO_REG; }
}

int setbit(const int value, const int position)
{
    assert(position >= 0);
    return value | (1 << position);
}
