#include "asm_interpreter.h"

#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "instructions.h"

void asmRun(CurrentCommand* cmd, LabelSystem* current_labels)
{
    cmd->sntxerr = false;
    int eof_indicator = 0;
    while(!cmd->sntxerr && eof_indicator != EOF && cmd->number_of_argument != EOF)
    {
        eof_indicator = fscanf(cmd->asm_file, "%s", cmd->instruction);
        cmd->number_of_argument = fscanf(cmd->asm_file, "%d", &cmd->value);
        if (eof_indicator != EOF) { interpreter(cmd, current_labels); }
    }
}

void interpreter(CurrentCommand* cmd, LabelSystem* current_labels)
{
    current_labels->cmd_counter++;
    while (true)
    {
        if (!strcmp(cmd->instruction, "hlt" )) { fprintf(cmd->code_file, "%d ", CMD_HLT ); break; }
        if (!strcmp(cmd->instruction, "push")) { fprintf(cmd->code_file, "%d ", CMD_PUSH); break; }
        if (!strcmp(cmd->instruction, "pop" )) { fprintf(cmd->code_file, "%d ", CMD_POP ); break; }
        if (!strcmp(cmd->instruction, "in"  )) { fprintf(cmd->code_file, "%d ", CMD_IN  ); break; }
        if (!strcmp(cmd->instruction, "out" )) { fprintf(cmd->code_file, "%d ", CMD_OUT ); break; }
        if (!strcmp(cmd->instruction, "dump")) { fprintf(cmd->code_file, "%d ", CMD_DUMP); break; }
        if (!strcmp(cmd->instruction, "add" )) { fprintf(cmd->code_file, "%d ", CMD_ADD ); break; }
        if (!strcmp(cmd->instruction, "sub" )) { fprintf(cmd->code_file, "%d ", CMD_SUB ); break; }
        if (!strcmp(cmd->instruction, "mul" )) { fprintf(cmd->code_file, "%d ", CMD_MUL ); break; }
        if (!strcmp(cmd->instruction, "div" )) { fprintf(cmd->code_file, "%d ", CMD_DIV ); break; }
        if (!strcmp(cmd->instruction, "sqrt")) { fprintf(cmd->code_file, "%d ", CMD_SQRT); break; }
        if (!strcmp(cmd->instruction, "sin" )) { fprintf(cmd->code_file, "%d ", CMD_SIN ); break; }
        if (!strcmp(cmd->instruction, "cos" )) { fprintf(cmd->code_file, "%d ", CMD_COS ); break; }
        if (!strcmp(cmd->instruction, "jmp" )) { fprintf(cmd->code_file, "%d ", CMD_JMP ); break; }
        if (!strcmp(cmd->instruction, "ja"  )) { fprintf(cmd->code_file, "%d ", CMD_JA  ); break; }
        if (!strcmp(cmd->instruction, "jae" )) { fprintf(cmd->code_file, "%d ", CMD_JAE ); break; }
        if (!strcmp(cmd->instruction, "jb"  )) { fprintf(cmd->code_file, "%d ", CMD_JB  ); break; }
        if (!strcmp(cmd->instruction, "jbe" )) { fprintf(cmd->code_file, "%d ", CMD_JBE ); break; }
        if (!strcmp(cmd->instruction, "je"  )) { fprintf(cmd->code_file, "%d ", CMD_JE  ); break; }
        if (!strcmp(cmd->instruction, "jne" )) { fprintf(cmd->code_file, "%d ", CMD_JNE ); break; }
        if ( strchr(cmd->instruction,  ':'  )) { current_labels->cmd_counter--           ; break; }

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
        fprintf(cmd->code_file, "%d ", cmd->value);
    }
    if (!strcmp(cmd->instruction, "in"))
    {
        if (cmd->number_of_argument != 1)
        {
            display_syntax_error(cmd);
            assert(0);
        }
        current_labels->cmd_counter++;
        fprintf(cmd->code_file, "%d ", cmd->value);
    }
    if (!strcmp(cmd->instruction, "jmp")
        || !strcmp(cmd->instruction, "ja") || !strcmp(cmd->instruction, "jae")
        || !strcmp(cmd->instruction, "jb") || !strcmp(cmd->instruction, "jbe")
        || !strcmp(cmd->instruction, "je") || !strcmp(cmd->instruction, "jne"))
    {
        char name_of_potential_label[MAX_LENGTH_OF_LABELS] = "";
        if (cmd->number_of_argument == 1)
        {
            fprintf(cmd->code_file, "%d ", cmd->value);
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
                fprintf(cmd->code_file, "%d ", current_labels->labels[index_of_label].position);
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
