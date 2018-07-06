#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utils.h"
#include "executor.h"
#include "tests.h"
#include "typedefs.h"
#include "data_processing.h"

#define BRANCH_OFFSET_MASK 0xffffff
#define DEBUG 0
#define PC_NMBR 15
#define MEMORY_CAPACITY 1<<15
#define PIN09_ADDRESS 0x20200000
#define PIN1019_ADDRESS 0x20200004
#define PIN2029_ADDRESS 0x20200008
#define PIN_OFF 0x20200028
#define PIN_ON 0x2020001C

void execute(Instr instr, Registers* regs, Instr* fetched, Byte *memory, int *halt) {
//executes Instruction instr. Registers regs is the struct (defined in typedefs.h) modelling the
// registers (general purpose, cpsr, pc). Instr* fetched is the next instruction to be executed, provided by the
// pipeline loop. It can be set to 0 by execute_branch in case of a jump, clearing the pipeline.

//terminate when all-zero instruction received
  if (!instr) {
    *halt = 1;
    return;;
  }

  if (DEBUG) {
    test_all(instr);
    if (instruction_is_valid(instr,regs)) {
      printf("Instruction valid\n");
    } else {
      printf("Instruction invalid\n");
    }
    print_registers(regs);
    printf("\n");
  }

  //only execute instruction if cond field is satisfied by cpsr register
  if (instruction_is_valid(instr,regs)) {
    int offset;

    InstrType type = get_instr_type(instr);
    switch (type) {
      case DATA_PROC:
        execute_data_proc(instr,regs);
            break;
      case MULT:
        execute_mult(instr,regs);
            break;
      case TRANSFER:
        execute_transfer(instr,regs,memory);
            break;
      case BRANCH:
        //sign extend branch offset mask by shifting it left so the leftmost bit lines up
        //with msb of 32bit int, then asr back to preserve top bit
        offset = (BRANCH_OFFSET_MASK & instr)<<8;
            offset>>=8;
            execute_branch(offset,&regs->pc,fetched);
            break;
      case NOOP:
        return;
      default: printf("UNEXPECTED INSTRUCTION TYPE\n"); exit(EXIT_FAILURE);
    }
  }
}

//rotates rights a uint32 a give number of times
uint32_t rotate_right(uint32_t n, unsigned int rotation_amount) {
  const unsigned int mask = (8*sizeof(n) - 1);
  rotation_amount &= mask;
  return (n>>rotation_amount) | (n<<((-rotation_amount)&mask));
}

//executes multiply instruction.
void execute_mult(Instr instr, Registers* regs) {
  uint32_t operand1 = regs->general[rmMultiply(instr)];
  uint32_t operand2 = regs->general[rsMultiply(instr)];
  uint64_t multiplicationResult = operand1 * operand2;

  // multiply and accumulate
  if (accumulate(instr)) {
    multiplicationResult += regs->general[rnMultiply(instr)];
  }
  //truncate result to 32bits
  uint32_t multiplicationResult32 = multiplicationResult;
  regs->general[rdMultiply(instr)] = multiplicationResult32;

  //set cpsr flags
  set_n_z(&regs->cpsr,multiplicationResult32);
}

static int is_accessing_pin(uint32_t address) {
// Helper function for transfer that checks if the accessed address correspond
// to one of the gpios addresses

    if (address == PIN09_ADDRESS) {
        return 0;
    }
    if (address == PIN1019_ADDRESS) {
        return 10;
    }
    if (address == PIN2029_ADDRESS) {
        return 20;
    }
    return -1;
}

void execute_transfer(Instr instr, Registers* regs, Byte* memory) {
// Executes a single data transfer instruction which modifies either the registers (load)
// or the memory (store). Special instructions are interpreted as gpios instructions as if
// there were executed on a raspberry pi.

    int32_t offset = operand2(instr);
    uint32_t address = 0;

    // Check if the base register is not the PC
    if (rn(instr) != PC_NMBR) {
        address = regs->general[rn(instr)];
    } else {
        address = regs->pc;
    }

    // Finding value of offset
    if (is_immediate(instr)) {
        // offset is a shifted register
        offset = operand2_as_shifted_register(offset, regs, 0);
    }

    if (is_pre_indexing(instr)) {
        // Pre-indexing

        if (is_up(instr)) {
            // addition
            address += offset;
        }
        else {
            // subtraction
            address -= offset;
        }

        if (is_load(instr)) {
            // word loaded from memory

            if (is_accessing_pin(address) != -1) {
                // GPIO
                printf("One GPIO pin from %d to %d has been accessed\n", is_accessing_pin(address),
                       is_accessing_pin(address) + 9);
                regs->general[rd(instr)] = address;
                return;
            }

            else if (address == PIN_OFF) {
                printf("PIN OFF\n");
                return;
            }
            else if (address == PIN_ON) {
                printf("PIN ON\n");
                return;
            }

            else if (address >= MEMORY_CAPACITY) {
                // Out of bounds error
                printf("Error: Out of bounds memory access at address 0x%08x\n", address);
                return;
            }
            regs->general[rd(instr)] = *((uint32_t*) (memory+address));

        } else {
            // word stored into memory

            if (is_accessing_pin(address) != -1) {
                // GPIO
                printf("One GPIO pin from %d to %d has been accessed\n", is_accessing_pin(address),
                       is_accessing_pin(address) + 9);
                regs->general[rd(instr)] = address;
                return;
            }

            else if (address == PIN_OFF) {
                printf("PIN OFF\n");
                return;
            }
            else if (address == PIN_ON) {
                printf("PIN ON\n");
                return;
            }

            if (address >= MEMORY_CAPACITY) {
                // Out of bounds error
                printf("Error: Out of bounds memory access at address 0x%08x\n", address);
                return;
            }
            *((uint32_t*) (memory+address)) = regs->general[rd(instr)];
        }

    } else {
        // Post-indexing
        assert (rn(instr) != rd(instr));

        if (is_load(instr)) {
            // word loaded from memory

            if (is_accessing_pin(address) != -1) {
                // GPIO
                printf("One GPIO pin from %d to %d has been accessed\n", is_accessing_pin(address),
                       is_accessing_pin(address) + 9);
                regs->general[rd(instr)] = address;
                return;
            }

            else if (address == PIN_OFF) {
                printf("PIN OFF\n");
                return;
            }
            else if (address == PIN_ON) {
                printf("PIN ON\n");
                return;
            }

            if (address >= MEMORY_CAPACITY) {
                // Out of bounds error
                printf("Error: Out of bounds memory access at address 0x%08x\n", address);
                return;
            }
            regs->general[rd(instr)] = *((uint32_t*) (memory+address));
        } else {
            // word stored into memory

            if (is_accessing_pin(address) != -1) {
                // GPIO
                printf("One GPIO pin from %d to %d has been accessed\n", is_accessing_pin(address),
                       is_accessing_pin(address) + 9);
                regs->general[rd(instr)] = address;
                return;
            }

            else if (address == PIN_OFF) {
                printf("PIN OFF\n");
                return;
            }
            else if (address == PIN_ON) {
                printf("PIN ON\n");
                return;
            }

            if (address >= MEMORY_CAPACITY) {
                // Out of bounds error
                printf("Error: Out of bounds memory access at address 0x%08x\n", address);
                return;
            }
            *((uint32_t*) (memory+address)) = regs->general[rd(instr)];
        }

        if (is_up(instr)) {
            // addition
            regs->general[rn(instr)] += offset;
        } else {
            // substraction
            regs->general[rn(instr)] -= offset;
        }
    }
}

  //executes branch instruction.
  void execute_branch(int offset, Register* pc, Instr* fetched) {
    //since pc is multiple of 4, left shift offset by 4
    offset<<=2;
    if (DEBUG) {
      printf("Executing branch: offset %d; pc before: %d; ", offset, *pc);
    }
    *pc += offset;

    if (DEBUG) {
      printf("pc after: %d\n\n", *pc);
    }
    *fetched = NOOP;
  }
