#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "haiku.h"
#include "grammar.h"
#include "parser.h"

#define DEBUG 0
#define ISTYPE(wordType) !strncmp(words[i],wordType,strlen(wordType))
#define MAKEWORD(wordType) {\
        strsep(&words[i]," ");\
        numSyllables = atoi(words[i]);\
        numPossibilities = dict->wordType.sizes[numSyllables-1]; \
        result[i] = dict->wordType.wordArrays[numSyllables-1][(int) (rand()%numPossibilities)];\
        if (DEBUG) printf("Chose word %s\n",result[i]);}

char *make_haiku(Haiku *haiku, Dictionary *dict) {
  srand48(time(0));
  srand(time(NULL));
  if (DEBUG) printf("Num haikus: %d\n",haiku->numHaikus);
  //unsigned int chosenHaiku = (unsigned int) (haiku->numHaikus * drand48());
  unsigned int chosenHaiku = (unsigned int) (rand() % haiku->numHaikus);
  if (DEBUG) printf("Haiku type: %d\n",chosenHaiku);

  char *haikuptr = haiku->haikus[chosenHaiku];

  //allocate for resulting array of words
  char **result = malloc(strlen(haikuptr) * sizeof(char*));
  if (!result) {
    printf("failed to allocate result in make_haiku\n");
    exit(EXIT_FAILURE);
  }

  //tokenize the haiku
  char **words = malloc(strlen(haikuptr) * sizeof(char*));
  if (!words) {
    printf("failed to allocate words in make_haiku\n");
    exit(EXIT_FAILURE);
  }

  //replace placeholders in haiku
  unsigned int numWords = 0;
  tokenize(words,&haikuptr,",\n",&numWords);
  for (int i = 0; i < numWords; i++) {

    //extract wordType
    unsigned int numSyllables;
    unsigned int numPossibilities;

    if (ISTYPE("noun")) {
      MAKEWORD(nouns);
    } else if (ISTYPE("adj")) {
      MAKEWORD(adjs);
    } else if (ISTYPE("adv")) {
      MAKEWORD(advs);
    } else if (ISTYPE("verbi")) {
      MAKEWORD(verbis);
    } else if (ISTYPE("verbt")) {
      MAKEWORD(verbts);
    } else if (ISTYPE("deter")) {
      MAKEWORD(deters);
    } else if (ISTYPE("prepo")) {
      MAKEWORD(prepos);
    } else if (ISTYPE("$NL")){
      result[i] = ",\n";
    } else {
      result[i] = words[i];
    }
  }

  char *finishedHaiku = concat_separator(result,numWords," ");
  free(result);
  free(words);
  return finishedHaiku;

}

char* concat_separator(char **words, unsigned int numWords, const char *sep) {
  assert(words != NULL);
  assert(sep != NULL);

  char *concatenated = calloc(1,100);
  for (int i = 0; i < numWords; i++) {
    strcat(concatenated,words[i]);
    strcat(concatenated,sep);
  }
  concatenated = realloc(concatenated,strlen(concatenated)+1);
  return concatenated;
}
