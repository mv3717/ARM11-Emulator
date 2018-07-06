#ifndef ASSEMBLER_ASSEMBLE_H
#define ASSEMBLER_ASSEMBLE_H


#include <stdio.h>
#include "typedefs.h"
#include "symboltable.h"

Instr create_instr_bits(Token token, SymbolTable *symbolTable, unsigned int dataAddress);

#endif //ASSEMBLER_UTILS_H
