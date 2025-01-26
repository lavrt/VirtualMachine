#include "asm_interpreter.h"

#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "instructions.h"
#include "asm_labels.h"
#include "debug.h"

// static --------------------------------------------------------------------------------------------------------------

static const char* NAME_OF_ASM_CODE_FILE = "asm_code_in.txt";
static const char* NAME_OF_MACHINE_CODE_FILE = "../Processor/machine_code_in.txt";

static void interpreter(Assembler* const ASM);
static void displaySyntaxError(Assembler* const ASM);
static void сommandStreamCtor(Assembler* const ASM);
static enum REGISTERS checkRegisterName(const char* const name);
static enum INSTRUCTIONS checkCommandName(const char* const name);
static int setbit(const int value, const int position);

static void firstPass(Assembler* const ASM);
static void secondPass(Assembler* const ASM);

static void processPushOrPopArgument(Assembler* const ASM);
static void processJumpsArgument(Assembler* const ASM);

static void ensureCapacityOfCodeArray(Assembler* const ASM);

static void encodePush(Assembler* const ASM);
static void encodePop(Assembler* const ASM);
static void encodeIn(Assembler* const ASM);
static void encodeJumps(Assembler* const ASM);
static void encodeCall(Assembler* const ASM);
static void encodeRet(Assembler* const ASM);

// global --------------------------------------------------------------------------------------------------------------

void asmCtor(Assembler* const ASM)
{
    assert(ASM);

    ASM->sntxerr = false;

    сommandStreamCtor(ASM);
    LabelsCtor(ASM);
    STACKCTOR(&ASM->callStack);
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

void twoPassCompilation(Assembler* const ASM)
{
    assert(ASM);

    firstPass(ASM);
    secondPass(ASM);

    ASM->code_file = fopen(NAME_OF_MACHINE_CODE_FILE, "wb");
    assert(ASM->code_file);

    fwrite(&ASM->commands.size, sizeof(int), 1, ASM->code_file);
    fwrite(ASM->commands.code, sizeof(int), (size_t)ASM->commands.size, ASM->code_file);

    FCLOSE(ASM->code_file);
}

// static --------------------------------------------------------------------------------------------------------------

static void firstPass(Assembler* const ASM)
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

        if (!strtok(string, " "))
        {
            continue;
        }

        switch (checkCommandName(string))
        {
            case NO_CMD  :    break;
            case CMD_PUSH:
            case CMD_POP :
            case CMD_JMP :
            case CMD_JAE :
            case CMD_JA  :
            case CMD_JBE :
            case CMD_JB  :
            case CMD_JNE :
            case CMD_JE  :
            case CMD_CALL:
            case CMD_RET :    ASM->current_labels.cmd_counter++;

            default:          ASM->current_labels.cmd_counter++;
        }

        if (strchr(string, ':'))
        {
            *strchr(string, ':') = '\0';
            strcpy(ASM->current_labels.labels[ASM->current_labels.number_of_labels].name, string);
            ASM->current_labels.labels[ASM->current_labels.number_of_labels].position = ASM->current_labels.cmd_counter;
            ASM->current_labels.number_of_labels++;
        }
    }

    FCLOSE(ASM->asm_file);

    ASM->current_labels.cmd_counter = 0;
}

static void secondPass(Assembler* const ASM)
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

        char* token = strtok(string, " ");

        if (string[0] == '\0' || strchr(string, ':'))
        {
            continue;
        }
        else if (checkCommandName(token))
        {
            strcpy(ASM->cmd.instruction, token);

            switch (checkCommandName(ASM->cmd.instruction))
            {
                case CMD_PUSH:
                case CMD_POP :    processPushOrPopArgument(ASM);    break;
                case CMD_CALL:
                case CMD_JMP :
                case CMD_JAE :
                case CMD_JA  :
                case CMD_JBE :
                case CMD_JB  :
                case CMD_JNE :
                case CMD_JE  :    processJumpsArgument(ASM);        break;

                default: break;
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

    StackDtor(&ASM->callStack);
}

static void processPushOrPopArgument(Assembler* const ASM)
{
    assert(ASM);

    char* ptr = strtok(NULL, " ");
    if (!ptr)
    {
        displaySyntaxError(ASM);
        assert(0);
    }

    if (strchr(ptr, '[') && strchr(ptr, ']'))
    {
        ASM->cmd.presence_ram = true;
        ASM->cmd.ram_address = (size_t)atoi(strtok(strchr(ptr, '['), "[]"));
    }
    else if ((ASM->cmd.name_of_register = checkRegisterName(ptr)))
    {
        ASM->cmd.presence_reg = true;
    }
    else if (atoi(ptr))
    {
        ASM->cmd.value = atoi(ptr);
    }
    else
    {
        displaySyntaxError(ASM);
        assert(0);
    }
}

static void processJumpsArgument(Assembler* const ASM)
{
    assert(ASM);

    char* ptr = strtok(NULL, " ");
    if (!ptr)
    {
        displaySyntaxError(ASM);
        assert(0);
    }

    strcpy(ASM->cmd.name_of_label, ptr);
}

static void interpreter(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);
    assert(ASM->cmd.instruction);

    ensureCapacityOfCodeArray(ASM);

    ASM->current_labels.cmd_counter++;
    ASM->commands.code[ASM->commands.size++] = checkCommandName(ASM->cmd.instruction);

    switch (checkCommandName(ASM->cmd.instruction))
    {
        case CMD_PUSH:    encodePush (ASM);    break;
        case CMD_POP :    encodePop  (ASM);    break;
        case CMD_IN  :    encodeIn   (ASM);    break;
        case CMD_CALL:    encodeCall (ASM);
        case CMD_JMP :
        case CMD_JAE :
        case CMD_JA  :
        case CMD_JBE :
        case CMD_JB  :
        case CMD_JNE :
        case CMD_JE  :    encodeJumps(ASM);    break;
        case CMD_RET :    encodeRet  (ASM);    break;

        default: break;
    }
}

static void ensureCapacityOfCodeArray(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);

    if (ASM->commands.capacity - ASM->commands.size <= 10)
    {
        ASM->commands.code = (int*)realloc(ASM->commands.code,
            (long unsigned)(ASM->commands.size + ADD_SIZE_OF_CMD_ARRAY) * sizeof(int));
        assert(ASM->commands.code);
        ASM->commands.capacity += ADD_SIZE_OF_CMD_ARRAY;
    }
}

static void encodePush(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);

    ASM->current_labels.cmd_counter++;

    if (ASM->cmd.presence_reg)
    {
        ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_PUSH, USING_REGISTER);
        ASM->commands.code[ASM->commands.size++] = ASM->cmd.name_of_register;
        ASM->cmd.name_of_register = NO_REG;
    }
    else if (ASM->cmd.presence_ram)
    {
        ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_PUSH, USING_RAM);
        ASM->commands.code[ASM->commands.size++] = (int)ASM->cmd.ram_address;
    }
    else
    {
        ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
    }
}

static void encodePop(Assembler* ASM)
{
    assert(ASM);
    assert(ASM->commands.code);

    ASM->current_labels.cmd_counter++;

    if (ASM->cmd.presence_reg)
    {
        ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_POP, USING_REGISTER);
        ASM->commands.code[ASM->commands.size++] = ASM->cmd.name_of_register;
        ASM->cmd.name_of_register = NO_REG;
    }
    else if (ASM->cmd.presence_ram)
    {
        ASM->commands.code[ASM->commands.size - 1] = setbit(CMD_POP, USING_RAM);
        ASM->commands.code[ASM->commands.size++] = (int)ASM->cmd.ram_address;
    }
    else
    {
        ASM->commands.code[ASM->commands.size++] = ASM->cmd.value;
    }
}

static void encodeIn(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);

    ASM->current_labels.cmd_counter++;

    scanf("%d", &(ASM->commands.code[ASM->commands.size++]));
}

static void encodeJumps(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);
    assert(ASM->current_labels.labels);

    ASM->current_labels.cmd_counter++;

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
}

static void encodeCall(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);

    ASM->commands.code[ASM->commands.size - 1] = CMD_JMP;

    push(&ASM->callStack, ASM->current_labels.cmd_counter + 1);
}

static void encodeRet(Assembler* const ASM)
{
    assert(ASM);
    assert(ASM->commands.code);

    ASM->commands.code[ASM->commands.size - 1] = CMD_JMP;
    ASM->current_labels.cmd_counter++;

    if (!ASM->callStack.size)
    {
        displaySyntaxError(ASM);
        assert(0);
    }

    ASM->commands.code[ASM->commands.size++] = pop(&ASM->callStack);
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
    else if (!strcmp(name, CALL)) { return CMD_CALL; }
    else if (!strcmp(name, RET )) { return CMD_RET ; }
    else                          { return NO_CMD  ; }
}

static int setbit(const int value, const int position)
{
    assert(position >= 0);

    return value | (1 << position);
}
