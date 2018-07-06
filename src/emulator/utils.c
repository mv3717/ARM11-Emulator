#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "typedefs.h"
#include "utils.h"

#define IMMEDIATE_MASK 1<<25
#define SET_FLAGS_MASK 1<<20
#define OPCODE_MASK 0x1e00000
#define OPCODE_BIT_POS  21
#define RN_MASK 0xf0000
#define RS_MASK_MULTIPLY 0xf00
#define RD_MASK_MULTIPLY 0xf0000
#define RN_MASK_MULTIPLY 0xf000
#define RM_MASK_MULTIPLY 15
#define RN_BIT_POS_MULTIPLY 12
#define RD_BIT_POS_MULTILLY 16
#define RS_BIT_POS_MULTIPLY 8
#define RN_BIT_POS 16
#define RD_MASK 0xf000
#define RD_BIT_POS 12
#define OPERAND2_MASK 0xfff
#define COND_BIT_OFFSET 28
#define ACCUMULATE 1<<21
#define SET_COND_CODES 1<<20
#define PRE_INDEXING_MASK 1 << 24
#define UP_MASK 1 << 23
#define LOAD_MASK 1 << 20
#define DEBUG 0

//reads a binary file
long readBinary(FILE* file, void *destination) {
  fseek(file,0,SEEK_END);
  //+1 because zero instruction at end of file is considered end of file but
  //needs to be included for emulator
  long filelen = ftell(file) + 1;
  rewind(file);
  fread(destination,1,(size_t) filelen,file);
  return filelen;
}

//returns bit that indicates whether operand 2 is an immediate value.
uint32_t is_immediate(Instr instruction) {
  return (IMMEDIATE_MASK & instruction);
}

//returns flag S.
uint32_t set_flags(Instr instruction) {
  return (SET_FLAGS_MASK & instruction);
}

//returns the opcode.
int opcode(Instr instruction) {
  return (OPCODE_MASK & instruction) >> OPCODE_BIT_POS;
}

//returns Rn.
int rn(Instr instruction) {
  return (RN_MASK & instruction) >> RN_BIT_POS;
}

//returns Rd
int rd(Instr instruction) {
  return (RD_MASK & instruction) >> RD_BIT_POS;
}

//returns whether value is accumulated.
uint32_t accumulate(Instr instruction) {
  return (ACCUMULATE & instruction);
}

//returns Rn when it is a multiply instruction.
int rnMultiply(Instr instruction) {
  return (RN_MASK_MULTIPLY & instruction) >> RN_BIT_POS_MULTIPLY;
}

//returns Rd when it is a multiply instruction.
int rdMultiply(Instr instruction) {
  return (RD_MASK_MULTIPLY & instruction) >> RD_BIT_POS_MULTILLY;
}

//returns Rs when it is a multiply instruction.
int rsMultiply(Instr instruction) {
  return (RS_MASK_MULTIPLY & instruction) >> RS_BIT_POS_MULTIPLY;
}

//returns Rm when it is a multiply instruction.
int rmMultiply(Instr instruction) {
  return (RM_MASK_MULTIPLY & instruction);
}

//returns condition codes.
uint32_t setConditionCodes(Instr instruction) {
  return (SET_COND_CODES & instruction);
}

//returns operand 2.
int operand2(Instr instruction) {
  return (OPERAND2_MASK & instruction);
}

//returns bit indicating whether there is pre-indexing.
uint32_t is_pre_indexing(Instr instruction) {
  return (PRE_INDEXING_MASK & instruction);
}

//returns U bit when its a Single Data Transfer instruction.
uint32_t is_up(Instr instruction) {
  return (UP_MASK & instruction);
}

//returns L bit when its a Single Data Transfer instruction.
uint32_t is_load(Instr instruction) {
  return (LOAD_MASK & instruction);
}

int get_cond(Instr instruction) {
  //since condition codes make up first four bits of instruction, unsigned shifting is enough
  return  instruction >> COND_BIT_OFFSET;
}

//returns the type of isntruction being processed.
InstrType get_instr_type(Instr instruction) {

  //NOOP = 0xffffffff
  if (instruction == NOOP) {
    return NOOP;
  }

  //if bit 27 is set -> branch
  if ((1<<27) & instruction) {
    return BRANCH;
  }

  //if bit 26 is set -> single data transfer
  if ((1<<26) & instruction) {
    return TRANSFER;
  }

  //by this point we need to distinguish between multiply and data processing

  //if immediate bit is set -> definitely data processing
  if (IMMEDIATE_MASK & instruction) {
    return DATA_PROC;
  }

  //if bit 7 is 0 -> definitely data processing
  if (!((1<<7) & instruction)) {
    return DATA_PROC;
  }

  //now if bit 4 is zero -> definitely data processing
  if (!((1<<4) & instruction)) {
    return DATA_PROC;
  }

  return MULT;
}

//checks the validity of an instruction.
int instruction_is_valid(Instr instr, Registers* regs) {
  uint32_t Z_register = 1<<30;
  uint32_t cpsr = regs->cpsr;
  uint32_t Z_is_set = Z_register & cpsr;
  uint32_t Z_is_clear = !Z_is_set;

  int isValid = 0;

  uint32_t N_is_equal_V = (cpsr >> 31) == ((cpsr << 3) >> 31);
  uint32_t N_is_not_equal_V = !N_is_equal_V;

  Cond cond = get_cond(instr);
  switch(cond) {
    case eq: isValid = Z_is_set; break;
    case ne: isValid = Z_is_clear; break;
    case ge: isValid = N_is_equal_V; break;
    case lt: isValid = N_is_not_equal_V; break;
    case gt: isValid = Z_is_clear && N_is_equal_V; break;
    case le: isValid = Z_is_set || N_is_not_equal_V; break;
    case al: isValid = 1; break;
    default: isValid = 0;
  }
  return isValid;
}

//prints the state of the memory.
void display_memory(Byte* memory, int memory_capacity) {
  printf("Non-zero memory:\n");
  for (int i = 0; i < memory_capacity; i+=4) {
    if (*((uint32_t*) (memory+i))) {
      printf("0x%08x: 0x",i);
      for (int j = 0; j < 4; j++) {
        printf("%02x", memory[i+j]);
      }
      printf("\n");
    }
  }
}

//prints the value of each register.
void print_registers(Registers *regs){
  printf("Registers:\n");
  int i;
  for(i = 0; i < 13; i++) {
    if (i<10){
    printf("$%d  : %10d (0x%08x)\n", i, regs->general[i], regs->general[i]);
  } else {
    printf("$%d : %10d (0x%08x)\n", i, regs->general[i], regs->general[i]);
  }
}
    printf("PC  : %10d (0x%08x)\n", regs->pc, regs->pc);
    printf("CPSR: %10d (0x%08x)\n", regs->cpsr, regs->cpsr);
}

//Sets the Z amd N bits of Cond.
void set_n_z(Register *cpsr, int result) {
  set_z(cpsr, !result); //set z flag if result is 0
  set_n(cpsr, result & 0x80000000); //set n flag to top bit of result
}

//Sets the V bit of Cond.
void set_v(Register *cpsr, int value) {
  if (DEBUG) printf("setting v: cpsr before: %08x\n", *cpsr);
  if (value) {
    *cpsr |= (1<<28);
  } else {
    *cpsr &= 0xefffffff;
  }
  if (DEBUG) printf("cpsr after: %08x\n", *cpsr);
}

//Sets the C bit of Cond.
void set_c(Register *cpsr, int value) {
  if (DEBUG) printf("setting c: cpsr before: %08x\n", *cpsr);
  if (value) {
    *cpsr |= (1<<29);
  } else {
    *cpsr &= 0xdfffffff;
  }
  if (DEBUG) printf("cpsr after: %08x\n", *cpsr);
}

//Sets the Z bit of Cond.
void set_z(Register *cpsr, int value) {
  if (DEBUG) printf("setting z: cpsr before: %08x\n", *cpsr);
  if (value) {
    *cpsr |= (1<<30);
  } else {
    *cpsr &= 0xbfffffff;
  }
  if (DEBUG) printf("cpsr after: %08x\n", *cpsr);
}

//Sets the N bit of Cond.
void set_n(Register *cpsr, int value) {
  if (DEBUG) printf("setting n: cpsr before: %08x\n", *cpsr);
  if (value) {
    *cpsr |= (1<<31);
  } else {
    *cpsr &= 0x70000000;
  };
  if (DEBUG) printf("cpsr after: %08x\n", *cpsr);
}
