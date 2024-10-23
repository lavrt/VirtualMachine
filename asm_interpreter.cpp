#include "asm_interpreter.h"

#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "instructions.h"
#include "asm_labels.h"

static const char* NAME_OF_ASM_CODE_FILE = "asm_code_in.txt"; // FIXME добавб define DEBUG_PRINT
static const char* NAME_OF_MACHINE_CODE_FILE = "machine_code_in.txt";
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
    ASM->sntxerr = false;
    int eof_indicator = 0;
    while(!ASM->sntxerr && eof_indicator != EOF && ASM->cmd.number_of_argument != EOF)
    {
        eof_indicator = fscanf(ASM->asm_file, "%s", ASM->cmd.instruction);
        ASM->cmd.number_of_argument = fscanf(ASM->asm_file, "%d", &ASM->cmd.value);
        if (eof_indicator != EOF) { interpreter(ASM); }
    }
    fwrite(&ASM->commands.size, sizeof(int), 1, ASM->code_file);
    fwrite(ASM->commands.code, sizeof(int), ASM->commands.size, ASM->code_file);
}

void interpreter(Assembler* ASM)
{
    if (ASM->commands.size == ASM->commands.capacity - 10) { memory_expansion(ASM); }
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

    if (!strcmp(ASM->cmd.instruction, "push"))
    {
        if (ASM->cmd.number_of_argument != 1)
        {
            display_syntax_error(ASM);
            assert(0);
        }
        ASM->current_labels.cmd_counter++;
        ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
    }
    if (!strcmp(ASM->cmd.instruction, "in"))
    {
        if (ASM->cmd.number_of_argument != 1)
        {
            display_syntax_error(ASM);
            assert(0);
        }
        ASM->current_labels.cmd_counter++;
        ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
    }
    if (!strcmp(ASM->cmd.instruction, "jmp")
        || !strcmp(ASM->cmd.instruction, "ja") || !strcmp(ASM->cmd.instruction, "jae")
        || !strcmp(ASM->cmd.instruction, "jb") || !strcmp(ASM->cmd.instruction, "jbe")
        || !strcmp(ASM->cmd.instruction, "je") || !strcmp(ASM->cmd.instruction, "jne"))
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

void display_syntax_error(Assembler* ASM)
{
    ASM->sntxerr = true;
    fprintf(stderr,              "Syntax error: \"%s\"\n", ASM->cmd.instruction);
    fprintf(ASM->code_file, "Syntax error: \"%s\"\n", ASM->cmd.instruction);
}

void CommandStreamCtor(Assembler* ASM)
{
    ASM->commands.code = (int*)calloc(50, sizeof(int));
    assert(ASM->commands.code);
    ASM->commands.capacity = 50;
    ASM->commands.size = 0;
}

void memory_expansion(Assembler* ASM)
{
    ASM->commands.code = (int*)realloc(ASM->commands.code, ASM->commands.size + ADD_SIZE_OF_CMD_ARRAY);
    assert(ASM->commands.code);
    ASM->commands.capacity += ADD_SIZE_OF_CMD_ARRAY;
}

void asmDtor(Assembler* ASM)
{
    LabelsDtor(ASM);
    FREE(ASM->commands.code);

    FCLOSE(ASM->asm_file);
    FCLOSE(ASM->code_file);
}
