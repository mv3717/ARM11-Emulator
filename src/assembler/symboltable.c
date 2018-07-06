#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "symboltable.h"
#include "typedefs.h"

// Goes through the source code and build a map of addresses to corresponding symbols
SymbolTable generate_symboltable(char *sourcecode, unsigned int *noInstructions) {
  assert(sourcecode!= NULL);


  //allocate SymbolTable and entries
  SymbolTable symbolTable;
  symbolTable.table = calloc(strlen(sourcecode)/4, sizeof(SymbolTableEntry)); //allocate enough

  unsigned int numInstructions = 0;
  unsigned int address = 0;
  unsigned int index = 0;

  char *instr;
  while ((instr = strsep(&sourcecode, "\n")) != NULL) {

    //move over whitespace
    while (isspace(*instr)) {
      instr++;
    }

    //if empty string continue to next iteration
    if (!strcmp(instr, "")) continue;

    SymbolTableEntry entry;
    entry.address = address;
    entry.symbol = instr;

    char *instrcpy = malloc((strlen(instr)+1) * sizeof(char));
    strcpy(instrcpy,instr);


    if (strchr(instr, ':') != NULL) { //label
      entry.isLabel = 1;
      instr[strlen(instr) - 1] = '\000'; //remove : character
      instrcpy[strlen(instrcpy) - 1] = '\000';
    } else { //instruction
      entry.isLabel = 0;
      numInstructions++;
      address += sizeof(Instr);
    }

    entry.symbol = instrcpy;
    symbolTable.table[index] = entry;
    index++;

  }

  //reallocate to match number of entries
  symbolTable.table = realloc(symbolTable.table,index*sizeof(SymbolTableEntry));

  symbolTable.currentSize = index;
  *noInstructions = numInstructions;

  return symbolTable;
}



// Destroys a symbol table
void free_symboltable(SymbolTable *symbolTable) {
  assert(symbolTable != NULL);
  int size = symbolTable->currentSize;
  for (int i = 0; i < size; i++) {
    free(symbolTable->table[i].symbol);
  }
}


// Given a symbol, it returns the corresponding address
unsigned int lookup_symboltable(SymbolTable* symbolTable, const char* symbol) {
  assert(symbolTable != NULL);
  int size = symbolTable->currentSize;
  for (int i = 0; i < size; i++) {
    if (!strcmp(symbolTable->table[i].symbol, symbol)) {
      return symbolTable->table[i].address;
    }
  }
  printf("ERROR: Label %s not found\n", symbol);
  return 0;
}



// For testing: prints the symbol table
void print_symbol_table(SymbolTable* symbolTable) {
  assert(symbolTable != NULL);
  int size = symbolTable->currentSize;
  printf("SYMBOL TABLE:\n");
  for (int i = 0; i < size; ++i) {
    printf("%s -> %d\n", symbolTable->table[i].symbol, symbolTable->table[i].address);
  }
  printf("\n");
}

