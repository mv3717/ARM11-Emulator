#ifndef RASPBERRYHIAKU_PARSER_H
#define RASPBERRYHIAKU_PARSER_H

#include "grammar.h"


void parse_dict(FILE *src, Dictionary *dict);

void parse_haikus(FILE *haikus, Haiku *haiku);

char **split_until(char **stringptr, char *separator, unsigned int numOccurrences);

char *skip_whitespace(const char *string);

void tokenize(char **buffer, char **stringptr, char *separator, unsigned int *numSplits);

#endif //RASPBERRYHIAKU_PARSER_H
