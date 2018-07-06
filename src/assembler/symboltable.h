#ifndef ASSEMBLER_SYMBOLTABLE_H
#define ASSEMBLER_SYMBOLTABLE_H

typedef struct {
    char *symbol;
    unsigned int address;
    char isLabel;
} SymbolTableEntry;


typedef struct {
    SymbolTableEntry* table;
    int currentSize;
} SymbolTable;


SymbolTable generate_symboltable(char *sourcecode, unsigned int *noInstructions);

void free_symboltable(SymbolTable *symbolTable);

unsigned int lookup_symboltable(SymbolTable* symbolTable, const char* symbol);

void print_symbol_table(SymbolTable* symbolTable);

#endif //ASSEMBLER_SYMBOLTABLE_H
