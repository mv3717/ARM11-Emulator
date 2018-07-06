#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "grammar.h"
#define DEBUG 0


void init_words(Words *words, unsigned int numWords, unsigned int maxSyllables) {
  words->sizes = calloc(sizeof(unsigned int),maxSyllables);
  if (!words->sizes) {
    perror("init_words");
    exit(EXIT_FAILURE);
  }
  words->wordArrays = calloc(sizeof(char **),maxSyllables);
  if (!words->wordArrays) {
    perror("init_words");
    exit(EXIT_FAILURE);
  }

  words->wordArrays[0] = malloc(numWords * maxSyllables * sizeof(char*));
  if (!words->wordArrays[0]) {
    perror("init_words");
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < maxSyllables; i++) {
    words->wordArrays[i] = words->wordArrays[i-1] + numWords;
  }
}

void free_words(Words *words) {
  for (int i = 0; i < MAX_NUM_SYLLABLES; i++) {
    for (int j = 0; j < words->sizes[i]; j++) {
      free(words->wordArrays[i][j]);
    }
  }
  free(words->wordArrays[0]);
  free(words->wordArrays);
  free(words->sizes);
}

void init_dict(Dictionary *dict) {
  init_words(&dict->nouns, MAX_NUM_WORDS, MAX_NUM_SYLLABLES);
  init_words(&dict->adjs, MAX_NUM_WORDS, MAX_NUM_SYLLABLES);
  init_words(&dict->deters, MAX_NUM_WORDS, MAX_NUM_SYLLABLES);
  init_words(&dict->prepos, MAX_NUM_WORDS, MAX_NUM_SYLLABLES);
  init_words(&dict->verbts, MAX_NUM_WORDS, MAX_NUM_SYLLABLES);
  init_words(&dict->verbis, MAX_NUM_WORDS, MAX_NUM_SYLLABLES);
  init_words(&dict->advs, MAX_NUM_WORDS, MAX_NUM_SYLLABLES);
}


void free_dict(Dictionary *dict) {
  free_words(&dict->nouns);
  free_words(&dict->adjs);
  free_words(&dict->deters);
  free_words(&dict->prepos);
  free_words(&dict->verbts);
  free_words(&dict->verbis);
  free_words(&dict->advs);
}

/* Adds a copy of the string pointed to by word to the given words under numSyllables
 * number of syllables */
void add_to_words(Words *words, char *word, unsigned int numSyllables) {
  assert(words != NULL);
  assert(word != NULL);


  unsigned int index = words->sizes[numSyllables-1];
  if (DEBUG) {
    printf("Adding word: %s at index = %d\n", word,index);
  }
  words->wordArrays[numSyllables-1][index] = malloc(strlen(word)+1);
  strcpy(words->wordArrays[numSyllables-1][index],word);

  words->sizes[numSyllables-1]++;
}

