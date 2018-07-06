#include <stdio.h>
#include <string.h>
#include "grammar.h"
#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#define DEBUG 1

/*
 * Reads in file specified by src and parses it into given dictionary
 */
void parse_dict(FILE *src, Dictionary *dict) {
  if (DEBUG) {
    assert(src != NULL);
    assert(dict != NULL);
  }

  char *lineptr = NULL;
  char *originalLineptr = lineptr;
  size_t size = 0;
  unsigned int numWords = 0;

  while (getline(&lineptr, &size, src) >= 0) {
    originalLineptr = lineptr;
    numWords++;

    if (lineptr[strlen(lineptr)-1] == '\n') {
      lineptr[strlen(lineptr)-1] = '\0';
    }

   char **tokens = split_until(&lineptr, ",", 2);

    if (DEBUG) {
      assert(tokens != NULL);
      assert(tokens[0] != NULL);
    }

    //split_until sets the pointer it is passed to NULL once it is done splitting,
    //so we need to restore it to avoid getline realloating the buffer on each iteration
    lineptr = originalLineptr;

    unsigned int numSyllables = (unsigned int) atoi(tokens[1]);
    char *word = tokens[2];

    if (!strcmp(tokens[0], "noun")) {
      add_to_words(&dict->nouns,word,numSyllables);
    } else if (!strcmp(tokens[0], "verbt")) {
      add_to_words(&dict->verbts,word,numSyllables);
    } else if (!strcmp(tokens[0], "verbi")) {
      add_to_words(&dict->verbis,word,numSyllables);
    } else if (!strcmp(tokens[0], "adjective")) {
      add_to_words(&dict->adjs,word,numSyllables);
    } else if (!strcmp(tokens[0], "preposition")) {
      add_to_words(&dict->prepos,word,numSyllables);
    } else if (!strcmp(tokens[0], "determiner")) {
      add_to_words(&dict->deters,word,numSyllables);
    } else if (!strcmp(tokens[0], "adverb")) {
      add_to_words(&dict->advs,word,numSyllables);
    }
    free(tokens);
  }

  free(originalLineptr);
}


/*
 * Parses the given haikus file into the Haiku struct haiku
 */
void parse_haikus(FILE *haikus, Haiku *haiku) {

  //get number of lines in file and allocate accordingly
  fseek(haikus,0,SEEK_END);
  long filelen = ftell(haikus);
  rewind(haikus);


  //read in haikus
  char *haikuDB = calloc(sizeof(char), filelen + 1);
  fread(haikuDB, 1, (size_t) filelen, haikus);

  unsigned int numHaikus = 0;
  char **haikusSeparate = malloc(sizeof(char*) * filelen);

  char *inputcpy = haikuDB;

  //split haikus and count number of them read in
  tokenize(haikusSeparate,&inputcpy,"@",&numHaikus);

  haiku->haikus = haikusSeparate;
  haiku->numHaikus = numHaikus;
  haiku->source = haikuDB;
}


/*
 * Traverses the string pointed to by stringptr and replaces the first numOccurrences
 * occurrences of any character contained in separator by a '\0' character. The addresses of each
 * now separated string are stored in **tokens and returned to the caller
 */
char **split_until(char **stringptr, char *separator, unsigned int numOccurrences) {
  char **tokens = malloc(sizeof(char*) * (numOccurrences+1));
  if (tokens == NULL) {
    printf("Failed to allocate buffer for split_until.\n");
    exit(EXIT_FAILURE);
  }
  unsigned int numTokens = 0;
  while ((tokens[numTokens] = strsep(stringptr,separator)) != NULL && numTokens != numOccurrences) {
    numTokens++;
  }
  if (DEBUG) {
    if (numOccurrences != numTokens) {
      printf("split_until didn't split as many tokens as asked to.\n");
    }
  }
  return tokens;
}

/*
 * Similarly to split_until, occurrences of characters in the string pointed to by stringptr are
 * replaced by the '\0' character. However, tokenize splits at all occurrences of characters in
 * separator, and saves the number of tokens generated this way in *numTokens and the pointers to
 * the respective tokens in the provided buffer array, rather than newly allocating one and
 * returning it.
 */
void tokenize(char **buffer, char **stringptr, char *separator, unsigned int *numTokens) {
  unsigned int cnt = 0;
  while ((buffer[cnt] = skip_whitespace(strsep(stringptr,separator))) != NULL) {
    if (strcmp(buffer[cnt],"") != 0) {
      cnt++;
    }
  }
  if (numTokens != NULL) {
    *numTokens = cnt;
  }
}

/*
 * Given a pointer to a string, returns a pointer to the first non-whitespace character contained
 * therein
 */
char *skip_whitespace(const char *string) {
  if (string == NULL) {
    return (char*) string;
  }

  char *copy = (char *) string;
  while (isspace(*copy)) {
    copy++;
  }
  return copy;
}



