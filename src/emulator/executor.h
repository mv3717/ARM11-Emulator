#ifndef ARM11_44_EXECUTOR_H
#define ARM11_44_EXECUTOR_H

void execute(Instr instr, Registers* regs, Instr* fetched, Byte *memory, int *halt);

void execute_mult(Instr instr, Registers* regs);

void execute_branch(int offset, Register* pc, Instr* fetched);

void execute_transfer(Instr instr, Registers* regs, Byte* memory);

uint32_t rotate_right(uint32_t n, unsigned int c);

#endif //ARM11_44_EXECUTOR_H
