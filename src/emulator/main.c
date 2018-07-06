#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "executor.h"
#define MEMORY_CAPACITY 1<<15


int main(int argc, char* argv[]) {

  //checking that there is the correct number of arguments.
  if (argc != 2) {
    printf("USAGE: %s filename\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  FILE *file;
  file = fopen(argv[1], "rb");

  //checking the file is valid
  if (file == NULL) {
    printf("INVALID FILE\n");
    exit(EXIT_FAILURE);
  }

  Byte *memory = calloc(MEMORY_CAPACITY, 1);
  readBinary(file,memory);
  Registers* regs = calloc(1,sizeof(Registers));

  fclose(file);

  //prepare pipeline
  Instr fetched = 0x0;
  Instr decoded = NOOP;
  int halt = 0;

  do {
    fetched = *((Instr*) (memory+regs->pc));
    regs->pc+=4;
    execute(decoded,regs,&fetched,memory,&halt);
    decoded = fetched;
  } while (!halt);
  print_registers(regs);
  display_memory(memory,MEMORY_CAPACITY);
  free(regs);
  exit(EXIT_SUCCESS);
}
