#ifndef PROCESSOR_H
#define PROCESSOR_H

enum INSTRUCTIONS
{
    Hlt,
    Push,
    Pop,
    In,
    Out,
    Dump,
    Add,
    Sub,
    Mul,
    Div,
    Sqrt,
    Sin,
    Cos,
};

void Run(void);

#endif // PROCESSOR_H
