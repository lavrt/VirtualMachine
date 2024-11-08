#include "asm_interpreter.h"

#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "instructions.h"
#include "asm_labels.h"

// вынести в debug.h
#define $ fprintf(stderr, "%s:%d in function: %s\n", __FILE__, __LINE__, __func__);
// FIXME разбить на папки

// static ---------------------------------------------------------------------

static const char* NAME_OF_ASM_CODE_FILE = "asm_code_in.txt"; // FIXME добавб define DEBUG_PRINT
static const char* NAME_OF_MACHINE_CODE_FILE = "machine_code_in.txt";

static void interpreter(Assembler* const ASM);
static void displaySyntaxError(Assembler* const ASM);
static void сommandStreamCtor(Assembler* const ASM);
static enum REGISTERS checkRegisterName(const char* const name);
static enum INSTRUCTIONS checkCommandName(const char* const name);
static int setbit(const int value, const int position);

// global ---------------------------------------------------------------------

void asmCtor(Assembler* const ASM)
{
    assert(ASM);

    ASM->code_file = fopen(NAME_OF_MACHINE_CODE_FILE, "wb"); // FIXME открывать в конце, чтоб открыть, записать, закрыть сразу
    assert(ASM->code_file);

    ASM->asm_file = fopen(NAME_OF_ASM_CODE_FILE, "rb"); // FIXME открыть, считать в буфер, закрыть
    assert(ASM->asm_file);

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

    FCLOSE(ASM->asm_file);
    FCLOSE(ASM->code_file);
}

void asmRun(Assembler* const ASM)
{
    // FIXME переписать:
    // идём по строчкам
    // внутри срочки мы можем понять что она: 1) пустая, 2) метка(по :), 3) команда, иначе синтаксическая ошибка
    // если метка то обрабатываем как метку
    // если пустая то пропускаем
    // если команда, то определяем команду и обрабатываем аргументы(отдельная функция)
    // всё

    assert(ASM);

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
            if (!checkRegisterName(ASM->cmd.name_of_register)
                && !(ASM->cmd.ram_address_indicator = sscanf(ASM->cmd.name_of_register, "[%lu", &ASM->cmd.ram_address)))
            {
                assert(0);
            }
        }
        if (!strcmp(ASM->cmd.instruction, POP))
        {
            fscanf(ASM->asm_file, "%s", ASM->cmd.instruction);
            if (checkRegisterName(ASM->cmd.instruction))
            {
                strcpy(ASM->cmd.name_of_register, ASM->cmd.instruction);
                strcpy(ASM->cmd.instruction, POP);
            }
            else if (checkCommandName(ASM->cmd.instruction))
            {
                ASM->current_labels.cmd_counter++;
                ASM->commands.code[ASM->commands.size++] = CMD_POP;
                instruction_not_read = false;
                continue;
            }
            else if ((ASM->cmd.ram_address_indicator = sscanf(ASM->cmd.instruction, "[%lu", &ASM->cmd.ram_address)))
            {
                strcpy(ASM->cmd.instruction, POP);
            }
        }

        if (eof_indicator != EOF) { interpreter(ASM); }
    }

    fwrite(&ASM->commands.size, sizeof(int), 1, ASM->code_file);
    fwrite(ASM->commands.code, sizeof(int), ASM->commands.size, ASM->code_file);
}

// static ---------------------------------------------------------------------

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
    }
    else
    {
        displaySyntaxError(ASM);
        assert(0);
    }

    if (!strcmp(ASM->cmd.instruction, PUSH))
    {
        if (ASM->cmd.number_of_argument == 0 && !checkRegisterName(ASM->cmd.name_of_register) && !ASM->cmd.ram_address_indicator)
        {
            displaySyntaxError(ASM);
            assert(0);
        }
        else if (ASM->cmd.number_of_argument == 0 && checkRegisterName(ASM->cmd.name_of_register) && !ASM->cmd.ram_address_indicator)
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_PUSH, USING_REGISTER);
            ASM->commands.code[ASM->commands.size++] = checkRegisterName(ASM->cmd.name_of_register);
            memset(ASM->cmd.name_of_register, '\0', 8);
        }
        else if (ASM->cmd.ram_address_indicator != 0 && ASM->cmd.ram_address_indicator != EOF)
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_PUSH, USING_RAM);
            ASM->commands.code[ASM->commands.size++] = ASM->cmd.ram_address;
            ASM->cmd.ram_address_indicator = 0;
        }
        else
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
        }
        ASM->cmd.number_of_argument = 0;
    }
    if (!strcmp(ASM->cmd.instruction, POP))
    {
        if (ASM->cmd.number_of_argument == 0 && checkRegisterName(ASM->cmd.name_of_register))
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_POP, USING_REGISTER);
            ASM->commands.code[ASM->commands.size++] = checkRegisterName(ASM->cmd.name_of_register);
            memset(ASM->cmd.name_of_register, '\0', 8);
        }
        else if (ASM->cmd.ram_address_indicator != 0 && ASM->cmd.ram_address_indicator != EOF)
        {
            ASM->current_labels.cmd_counter++;
            ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_POP, USING_RAM);
            ASM->commands.code[ASM->commands.size++] = ASM->cmd.ram_address;
            ASM->cmd.ram_address_indicator = 0;
        }
    }
    if (!strcmp(ASM->cmd.instruction, IN))
    {
        if (ASM->cmd.number_of_argument != 1)
        {
            displaySyntaxError(ASM);
            assert(0);
        }
        ASM->current_labels.cmd_counter++;
        ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
        ASM->cmd.number_of_argument = 0;
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
            int index_of_label = labelSearch(ASM, name_of_potential_label);
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
        else
        {
            assert(0);
        }
        ASM->current_labels.cmd_counter++;
        ASM->cmd.number_of_argument = 0;
    }
    if (strchr(ASM->cmd.instruction, ':'))
    {
        strcpy(ASM->current_labels.labels[ASM->current_labels.number_of_labels].name, ASM->cmd.instruction);
        ASM->current_labels.labels[ASM->current_labels.number_of_labels].position = ASM->current_labels.cmd_counter;
        ASM->current_labels.number_of_labels++;
    }

    if (ASM->cmd.number_of_argument != 0 && ASM->cmd.number_of_argument != -1)
    {
        fprintf(stderr, "Syntax error: \"%d\"\n", ASM->cmd.value);
        assert(0);
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
    ASM->commands.code = (int*)calloc(ADD_SIZE_OF_CMD_ARRAY, sizeof(int));
    ASM->commands.capacity = ADD_SIZE_OF_CMD_ARRAY;
    ASM->commands.size = 0;

    assert(ASM->cmd.instruction); // FIXME лучше сразу проверять если что то открываешь или выделяешь, и лучше не асертом(но это *)
    assert(ASM->commands.code);
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
