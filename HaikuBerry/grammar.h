#ifndef RASPBERRYHIAKU_GRAMMAR_H
#define RASPBERRYHIAKU_GRAMMAR_H

typedef struct {
    unsigned int *sizes; //sizes of each array of words
    char ***wordArrays; //array of arrays of words with specific number of syllables
                        //e.g. wordsArrays[0] points to the array of words with 1 syllable
} Words;

typedef struct {
    Words nouns;
    Words adjs;
    Words advs;
    Words verbts;
    Words verbis;
    Words prepos;
    Words deters;
} Dictionary;

typedef struct {
    char *source;
    char **haikus;
    unsigned int numHaikus;
} Haiku;


void init_words(Words *words, unsigned int numWords, unsigned int maxSyllables);

void init_dict(Dictionary *dict);

void free_words(Words *words);

void free_dict(Dictionary *dict);

void add_to_words(Words *words, char *word, unsigned int numSyllables);

#define MAX_NUM_WORDS 100
#define MAX_NUM_SYLLABLES 5

#endif //RASPBERRYHIAKU_GRAMMAR_H
