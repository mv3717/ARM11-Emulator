#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

#include "typedefs.h"
#include <stdio.h>

char* read_textfile(FILE *file);

void print_hex(Instr instr);

#endif //ASSEMBLER_UTILS_H
