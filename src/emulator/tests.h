//
// Created by Sebastian Kovats on 24/5/18.
//

#include "typedefs.h"

#ifndef EMULATOR_TESTS_H
#define EMULATOR_TESTS_H

char* print_condition(Cond cond);

void test_condition(Instr instruction);

void test_opcode(Instr instruction);

void test_rd(Instr instruction);

void test_rn(Instr instruction);

void test_operand2(Instr instruction);

void test_immediate(Instr instruction);

void test_set_codes(Instr instruction);

void test_get_instr_type(Instr instruction);

void test_all(Instr instr);

uint32_t instructionIsValid(Instr instruction);

#endif //EMULATOR_TESTS_H
