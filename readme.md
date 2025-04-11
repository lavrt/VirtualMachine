# Functional CPU Simulator

A cycle-accurate simulator of a custom CPU architecture with register/ram emulation.

## Key Features

**Features**:
- 22 implemented instructions (arithmetic, control flow, memory ops)
- Register emulation 
- RAM memory model with configurable size
- Custom assembler syntax

## Quick Start

```bash
git clone https://github.com/lavrt/VirtualMachine
cd VirtualMachine
./build.sh
```

## Instruction Set Architecture
| Category	 | Instructions                        |
|------------|-------------------------------------|
| Arithmetic | ADD, SUB, MUL, DIV, SQRT, SIN, COS  |
| Control    | JMP/JA/JAE/JB/JBE/JE/JNE, CALL, RET |
| Memory	 | PUSH, POP                           |
| I/O        | IN, OUT                             |
| System	 | HLT, DUMP                           |