#ifndef ASSEMBLER_PARSER_H
#define ASSEMBLER_PARSER_H

#include "typedefs.h"
#include <stdio.h>

void parse_instr(Token *token, char *instruction, unsigned int address);

char* skip_whitespace(const char *string);

char *split(char **stringptr, char separator, unsigned int numOccurrence);

char **split_until(char **stringptr, char *separator, unsigned int numOccurrences);

#endif //ASSEMBLER_PARSER_H

