#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "typedefs.h"
#include "utils.h"
#include "tests.h"
#include <math.h>

void test_all(Instr instr) {
  test_condition(instr);
   test_opcode(instr);
   test_rd(instr);
   test_rn(instr);
   test_operand2(instr);
   test_immediate(instr);
   test_set_codes(instr);
   test_get_instr_type(instr);
}

char* print_condition(Cond cond) {
  char* ret;
  switch (cond) {
    case ne: ret = "ne"; break;
    case eq: ret = "eq"; break;
    case ge: ret = "ge"; break;
    case lt: ret = "lt"; break;
    case gt: ret = "gr"; break;
    case le: ret = "le"; break;
    case al: ret = "al"; break;
    default: ret = "NOOP";
  }
  return ret;
}

void test_condition(Instr instruction) {
  Cond cond = get_cond(instruction);
  printf("Cond: %s\n",print_condition(cond));
}

void test_opcode(Instr instruction) {
  int op = opcode(instruction);
  printf("Opcode: 0x%x\n",op);
}

void test_rd(Instr instruction) {
  int rdd = rd(instruction);
  printf("Rd: %d\n",rdd);
}

void test_rn(Instr instruction) {
  int rnn = rn(instruction);
  printf("Rn: %d\n", rnn);
}

void test_operand2(Instr instruction) {
  int o2 = operand2(instruction);
  printf("Operand2: 0x%x\n", o2);
}

void test_immediate(Instr instruction) {
  if (is_immediate(instruction)) {
    printf("Immediate: 1\n");
  } else {
    printf("Immediate: 0\n");
  }
}

void test_set_codes(Instr instruction) {
  if (set_flags(instruction)) {
    printf("Set codes: 1\n");
  } else {
    printf("Set codes: 0\n");
  }
}

void test_get_instr_type(Instr instruction) {
  InstrType it = get_instr_type(instruction);
  printf("Instruction Type: ");
  switch (it) {
    case DATA_PROC: printf("Data Processing\n"); break;
    case MULT: printf("Multiply\n"); break;
    case TRANSFER: printf("Single Data Transfer\n"); break;
    case BRANCH: printf("Branch\n"); break;
    case NOOP: printf("NOOP\n");
      break;
    default: printf("ERROR\n");
  }
}
