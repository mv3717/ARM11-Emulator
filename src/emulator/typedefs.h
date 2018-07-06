#include <stdint.h>

#ifndef EMULATOR_ENUMS_H
#define EMULATOR_ENUMS_H

enum instr_type{
    DATA_PROC,
    MULT,
    TRANSFER,
    BRANCH,
    NOOP=0xffffffff
};

typedef enum instr_type InstrType;

enum cond{
    eq=0b0000,
    ne=0b0001,
    ge=0b1010,
    lt=0b1011,
    gt=0b1100,
    le=0b1101,
    al=0b1110};

typedef enum cond Cond;

typedef uint32_t Instr;

typedef uint32_t Register;

struct registers {
  Register general[13];
  Register pc;
  Register cpsr;
};

typedef struct registers Registers;

typedef unsigned char Byte;

#endif //EMULATOR_ENUMS_H
