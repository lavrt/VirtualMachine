#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

enum INSTRUCTIONS
{
    CMD_HLT  = 0,
    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_IN   = 3, // FIXME in читает с stdin число и пушит на стек
    CMD_OUT  = 4,
    CMD_DUMP = 5,
    CMD_ADD  = 6,
    CMD_SUB  = 7,
    CMD_MUL  = 8,
    CMD_DIV  = 9,
    CMD_SQRT = 10,
    CMD_SIN  = 11,
    CMD_COS  = 12,
    CMD_JMP  = 13,
    CMD_JA   = 14,
    CMD_JAE  = 15,
    CMD_JB   = 16,
    CMD_JBE  = 17,
    CMD_JE   = 18,
    CMD_JNE  = 19,
};

#endif // INSTRUCTIONS_H
