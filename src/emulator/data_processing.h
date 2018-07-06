#ifndef ARM_11_44_DATA_PROCESSING_H
#define ARM_11_44_DATA_PROCESSING_H

#include "typedefs.h"

void execute_data_proc(Instr instruction, Registers* regs);

int operand2_as_shifted_register(int o2, Registers *regs, int set_cpsr);

void and(Register rn, int operand2, Register *rd, int set_flags, Register *cpsr);

void eor(Register rn, int operand2, Register *rd, int set_flags, Register *cpsr);

void sub(Register rn, int operand2, Register *rd, int set_flags, Register *cpsr);

void rsb(Register rn, int operand2, Register *rd, int set_flags, Register *cpsr);

void add(Register rn, int operand2, Register *rd, int set_flags, Register *cpsr);

void tst(Register rn, int operand2, int set_flags, Register *cpsr);

void teq(Register rn, int operand2, int set_flags, Register *cpsr);

void cmp(Register rn, int operand2, int set_flags, Register *cpsr);

void orr(Register rn, int operand2, Register *rd, int set_flags, Register *cpsr);

void mov(int operand2, Register* rd, int set_flags, Register *cpsr);

int shift(uint32_t n, int shift_type, unsigned int shift_amount,
          Register *cpsr, int set_flags);

#endif
