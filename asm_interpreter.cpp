#include "asm_interpreter.h"

#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "asm_labels.h"
#include "instructions.h"

// FIXME разбить на папки

void asmRun(CurrentCommand* cmd, LabelSystem* current_labels, CommandStream* commands)
{
    cmd->sntxerr = false;
    int eof_indicator = 0;
    while(!cmd->sntxerr && eof_indicator != EOF && cmd->number_of_argument != EOF)
    {
        eof_indicator = fscanf(cmd->asm_file, "%s", cmd->instruction);
        cmd->number_of_argument = fscanf(cmd->asm_file, "%d", &cmd->value);
        if (eof_indicator != EOF) { interpreter(cmd, current_labels, commands); }
    }
    fwrite(&commands->size, sizeof(int), 1, cmd->code_file);
    fwrite(commands->code, sizeof(int), commands->size, cmd->code_file);
}

void interpreter(CurrentCommand* cmd, LabelSystem* current_labels, CommandStream* commands)
{
    if (commands->size == commands->capacity - 10) { memory_expansion(commands); }
    current_labels->cmd_counter++;
    while(true)
    {
        if (!strcmp(cmd->instruction, HLT )) { commands->code[commands->size++] = CMD_HLT ; break; }
        if (!strcmp(cmd->instruction, PUSH)) { commands->code[commands->size++] = CMD_PUSH; break; }
        if (!strcmp(cmd->instruction, POP )) { commands->code[commands->size++] = CMD_POP ; break; }
        if (!strcmp(cmd->instruction, IN  )) { commands->code[commands->size++] = CMD_IN  ; break; }
        if (!strcmp(cmd->instruction, OUT )) { commands->code[commands->size++] = CMD_OUT ; break; }
        if (!strcmp(cmd->instruction, DUMP)) { commands->code[commands->size++] = CMD_DUMP; break; }
        if (!strcmp(cmd->instruction, ADD )) { commands->code[commands->size++] = CMD_ADD ; break; }
        if (!strcmp(cmd->instruction, SUB )) { commands->code[commands->size++] = CMD_SUB ; break; }
        if (!strcmp(cmd->instruction, MUL )) { commands->code[commands->size++] = CMD_MUL ; break; }
        if (!strcmp(cmd->instruction, DIV )) { commands->code[commands->size++] = CMD_DIV ; break; }
        if (!strcmp(cmd->instruction, SQRT)) { commands->code[commands->size++] = CMD_SQRT; break; }
        if (!strcmp(cmd->instruction, SIN )) { commands->code[commands->size++] = CMD_SIN ; break; }
        if (!strcmp(cmd->instruction, COS )) { commands->code[commands->size++] = CMD_COS ; break; }
        if (!strcmp(cmd->instruction, JMP )) { commands->code[commands->size++] = CMD_JMP ; break; }
        if (!strcmp(cmd->instruction, JA  )) { commands->code[commands->size++] = CMD_JA  ; break; }
        if (!strcmp(cmd->instruction, JAE )) { commands->code[commands->size++] = CMD_JAE ; break; }
        if (!strcmp(cmd->instruction, JB  )) { commands->code[commands->size++] = CMD_JB  ; break; }
        if (!strcmp(cmd->instruction, JBE )) { commands->code[commands->size++] = CMD_JBE ; break; }
        if (!strcmp(cmd->instruction, JE  )) { commands->code[commands->size++] = CMD_JE  ; break; }
        if (!strcmp(cmd->instruction, JNE )) { commands->code[commands->size++] = CMD_JNE ; break; }
        if ( strchr(cmd->instruction, ':' )) { current_labels->cmd_counter--              ; break; }

        display_syntax_error(cmd);
        assert(0);
    }

    if (!strcmp(cmd->instruction, "push"))
    {
        if (cmd->number_of_argument != 1)
        {
            display_syntax_error(cmd);
            assert(0);
        }
        current_labels->cmd_counter++;
        commands->code[commands->size++] = cmd->value;
    }
    if (!strcmp(cmd->instruction, "in"))
    {
        if (cmd->number_of_argument != 1)
        {
            display_syntax_error(cmd);
            assert(0);
        }
        current_labels->cmd_counter++;
        commands->code[commands->size++] = cmd->value;
    }
    if (!strcmp(cmd->instruction, "jmp")
        || !strcmp(cmd->instruction, "ja") || !strcmp(cmd->instruction, "jae")
        || !strcmp(cmd->instruction, "jb") || !strcmp(cmd->instruction, "jbe")
        || !strcmp(cmd->instruction, "je") || !strcmp(cmd->instruction, "jne"))
    {
        char name_of_potential_label[MAX_LENGTH_OF_LABELS] = "";
        if (cmd->number_of_argument == 1)
        {
            commands->code[commands->size++] = cmd->value;
        }
        else if (fscanf(cmd->asm_file, "%s", name_of_potential_label) == EOF)
        {
            assert(0);
        }
        else if (strchr(name_of_potential_label, ':'))
        {
            int index_of_label = label_search(current_labels, name_of_potential_label);
            if (index_of_label != -1)
            {
                commands->code[commands->size++] = current_labels->labels[index_of_label].position;
            }
            else
            {
                display_syntax_error(cmd);
                assert(0);
            }
        }
        else
        {
            assert(0);
        }
        current_labels->cmd_counter++;
    }
    if (strchr(cmd->instruction, ':'))
    {
        strcpy(current_labels->labels[current_labels->number_of_labels].name, cmd->instruction);
        current_labels->labels[current_labels->number_of_labels].position = current_labels->cmd_counter;
        current_labels->number_of_labels++;
    }
}

void display_syntax_error(CurrentCommand* cmd)
{
    cmd->sntxerr = true;
    fprintf(stderr,         "Syntax error: \"%s\"\n", cmd->instruction);
    fprintf(cmd->code_file, "Syntax error: \"%s\"\n", cmd->instruction);
}

void CommandStreamCtor(CommandStream* commands)
{
    commands->code = (int*)calloc(50, sizeof(int));
    assert(commands->code);
    commands->capacity = 50;
}

void memory_expansion(CommandStream* commands)
{
    commands->code = (int*)realloc(commands->code, commands->size + ADD_SIZE_OF_CMD_ARRAY);
    assert(commands->code);
    commands->capacity += ADD_SIZE_OF_CMD_ARRAY;
}

void CommandStreamDtor(CommandStream* commands)
{
    FREE(commands->code);
}
