#ifndef RASPBERRYHIAKU_HAIKU_H
#define RASPBERRYHIAKU_HAIKU_H

#include "grammar.h"
#include "parser.h"

char *make_haiku(Haiku *haiku, Dictionary *dict);

char* concat_separator(char **words, unsigned int numWords, const char *sep);

#endif //RASPBERRYHIAKU_HAIKU_H
