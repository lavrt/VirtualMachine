#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

enum INSTRUCTIONS
{
    CMD_HLT  = 0,
    CMD_PUSH = 1,
    CMD_POP  = 2,
    CMD_IN   = 3,
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

const char* const HLT  = "hlt" ;
const char* const PUSH = "push";
const char* const POP  = "pop" ;
const char* const IN   = "in"  ;
const char* const OUT  = "out" ;
const char* const DUMP = "dump";
const char* const ADD  = "add" ;
const char* const SUB  = "sub" ;
const char* const MUL  = "mul" ;
const char* const DIV  = "div" ;
const char* const SQRT = "sqrt";
const char* const SIN  = "sin" ;
const char* const COS  = "cos" ;
const char* const JMP  = "jmp" ;
const char* const JA   = "ja"  ;
const char* const JAE  = "jae" ;
const char* const JB   = "jb"  ;
const char* const JBE  = "jbe" ;
const char* const JE   = "je"  ;
const char* const JNE  = "jne" ;

#endif // INSTRUCTIONS_H
