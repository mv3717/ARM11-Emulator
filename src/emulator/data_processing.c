#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utils.h"
#include "executor.h"
#include "tests.h"
#include "typedefs.h"
#include "data_processing.h"
#define DEBUG 0

//execute data processing instruction.
void execute_data_proc(Instr instruction, Registers* regs) {

  int op = opcode(instruction);
  int o2 = operand2(instruction);
  int rnn = rn(instruction);
  int rdd = rd(instruction);
  int immediate = is_immediate(instruction);
  int set_cpsr = set_flags(instruction);
  int operand2;
  Register *cpsr = &regs->cpsr;
  Register *rd = &regs->general[rdd];
  Register rn = regs->general[rnn];

  if (immediate) {
    //extract immediate value from o2
    int imm = o2 & 0xff;
    //extract value of rotation, only even steps possible
    uint32_t rotation = 2 * ((o2 & 0xf00) >> 8);
    operand2 = rotate_right(imm, rotation);
  } else {
    //operand2 is a register
    operand2 = operand2_as_shifted_register(o2,regs,set_cpsr);
  }

  switch (op) {
    case 0: and(rn,operand2,rd,set_cpsr,cpsr); break;
    case 1: eor(rn,operand2,rd,set_cpsr,cpsr); break;
    case 2: sub(rn,operand2,rd,set_cpsr,cpsr); break;
    case 3: rsb(rn,operand2,rd,set_cpsr,cpsr); break;
    case 4: add(rn,operand2,rd,set_cpsr,cpsr); break;
    case 8: tst(rn,operand2,set_cpsr,cpsr); break;
    case 9: teq(rn,operand2,set_cpsr,cpsr); break;
    case 10: cmp(rn,operand2,set_cpsr,cpsr); break;
    case 12: orr(rn,operand2,rd,set_cpsr,cpsr); break;
    case 13: mov(operand2,rd,set_cpsr,cpsr); break;
    default: printf("Invalid opcode\n"); exit(EXIT_FAILURE);
  }
}

//handles the case where operand 2 is a shifted register.
int operand2_as_shifted_register(int o2, Registers *regs, int set_cpsr) {
  int rm = o2 & 0xf;
  int bit4 = o2 & 0x10;
  unsigned int shift_type = (o2 & 0x60)>>5;
  unsigned int shift_amount;

  if (bit4) {
    //shift amount detemined by bottom byte of register rs specified in
    //instruction bits 11-8
    int rs = (o2 & 0xf00) >> 8;
    shift_amount = regs->general[rs] & 0xff;
  } else {
    //shift amount is specified in instruction bits 11-7
    shift_amount = (o2 & 0xf80) >> 7;
  }
  return shift(regs->general[rm],shift_type,shift_amount,&regs->cpsr,set_cpsr);
}

void and(Register rn, int operand2, Register *rd, int set_cpsr, Register *cpsr) {
  int result = rn & operand2;
  if (set_cpsr)
    set_n_z(cpsr,result);
  *rd = result;
}

void eor(Register rn, int operand2, Register *rd, int set_cpsr, Register *cpsr) {
  int result = rn ^ operand2;
  if (set_cpsr)
    set_n_z(cpsr,result);
  *rd = result;
}

void sub(Register rn, int operand2, Register *rd, int set_cpsr, Register *cpsr) {
  int result = ((int) rn) - operand2;
  if (set_cpsr) {
    set_n_z(cpsr,result);
    set_c(cpsr,((int) rn) >= operand2);
  }

  *rd = result;
}

void rsb(Register rn, int operand2, Register *rd, int set_cpsr, Register *cpsr) {
  int result = operand2 - ((int) rn);
  if (set_cpsr)
    set_n_z(cpsr,result);
  *rd = result;
}

void add(Register rn, int operand2, Register *rd, int set_cpsr, Register *cpsr) {
  //check for unsigned overflow
  uint32_t operand2_unsigned = (uint32_t) operand2;
  set_c(cpsr,(rn + operand2_unsigned) < rn);

  int result = ((int) rn) + operand2;
  if (set_cpsr)
    set_n_z(cpsr,result);
  *rd = result;
}

void tst(Register rn, int operand2, int set_cpsr, Register *cpsr) {
  int result = rn & operand2;
  if (set_cpsr)
    set_n_z(cpsr,result);
}

void teq(Register rn, int operand2, int set_cpsr, Register *cpsr) {
  int result = rn ^ operand2;
  if (set_cpsr)
    set_n_z(cpsr,result);
}

void cmp(Register rn, int operand2, int set_cpsr, Register *cpsr) {
  int result = rn - operand2;
  if (set_cpsr) {
    set_n_z(cpsr,result);
    set_c(cpsr,((int) rn) >= operand2);
  }
}

void orr(Register rn, int operand2, Register *rd, int set_cpsr, Register *cpsr) {
  int result = rn | operand2;
  if (set_cpsr)
    set_n_z(cpsr,result);
  *rd = result;
}

void mov(int operand2, Register* rd, int set_cpsr, Register *cpsr) {
  if (set_cpsr)
    set_n_z(cpsr,operand2);
  *rd = operand2;
}

//executes a shift operation.
int shift(uint32_t n, int shift_type, unsigned int shift_amount,
          Register *cpsr, int set_cpsr) {

  // initialise carry out and result
  int32_t cout = 0;
  int result = 0;
  switch(shift_type) {
    case 0: //left shift
      if (shift_amount)
      //need to check for negative/too big shifts to avoid undefined behaviour
        cout = (1<<(32-shift_amount)) & n;
      result = n << shift_amount; break;
    case 1: //logical right shift
      if (shift_amount)
        cout = (1<<(shift_amount-1)) & n;
      result = n >> shift_amount; break;
    case 2: //arithmetic right shift
      if (shift_amount)
        cout = (1<<(shift_amount-1)) & n;
      result = (((int) n) >> shift_amount); break;
    case 3: //rotate right
      if (shift_amount)
        cout = (1<<(shift_amount-1)) & n;
      result = rotate_right(n, shift_amount); break;
    default: printf("Unexpected shift type\n"); exit(EXIT_FAILURE);
  }
  if (DEBUG) printf("Setting c via shifter:");
  if (set_cpsr) set_c(cpsr,(int) cout);
  return result;
}
