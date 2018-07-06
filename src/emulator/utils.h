#ifndef EMULATOR_UTILS_H
#define EMULATOR_UTILS_H

#include "typedefs.h"

long readBinary(FILE* file, void *destination);

uint32_t set_flags(Instr instruction);

uint32_t is_immediate(Instr instruction);

int opcode(Instr instruction);

int rd(Instr instruction);

int rn(Instr instruction);

int operand2(Instr instruction);

uint32_t is_pre_indexing(Instr instruction);

uint32_t is_up(Instr instruction);

uint32_t is_load(Instr instruction);

int get_cond(Instr instruction);

InstrType get_instr_type(Instr instruction);

int instruction_is_valid(Instr instr, Registers* regs);

uint32_t accumulate(Instr instruction);

void set_n_z(Register *cpsr, int result);

void set_v(Register *cpsr, int value);

void set_c(Register *cpsr, int value);

void set_z(Register *cpsr, int value);

void set_n(Register *cpsr, int value);

int rnMultiply(Instr instruction);

int rdMultiply(Instr instruction);

int rsMultiply(Instr instruction);

int rmMultiply(Instr instruction);

uint32_t setConditionCodes(Instr instruction);

void print_registers(Registers *regs);

void display_memory(Byte* memory, int memory_capacity);

#endif //EMULATOR_UTILS_H
