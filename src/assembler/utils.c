#include <stdlib.h>
#include "utils.h"

/*
 * Allocates a char array of correct size and reads in the provided text file 'file'. Returns the
 * newly allocated char array containing the contents of the file.
 */
char *read_textfile(FILE *file) {
  fseek(file, 0, SEEK_END);
  int lengthOfFile = (int) ftell(file) + 1;
  rewind(file);
  char *text = calloc((lengthOfFile+1),sizeof(char));
  fread(text, 1, (size_t) lengthOfFile, file);
  return text;
}

/*
 * Prints an Instr instr (uint32_t) as it is laid out in memory of a little-endian system
 */
void print_hex(Instr instr) {
  for (int i = 0; i < 4; i++) {
    printf("%02x ", ((instr << (24 - 8 * i) >> (8 * i)) >> (24 - 8 * i)));
  }
  printf("\n");
}
