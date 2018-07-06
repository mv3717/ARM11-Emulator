#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "typedefs.h"
#include "assemble.h"
#include "parser.h"
#include "utils.h"


#define DEBUG 0

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("USAGE..\n");
    exit(EXIT_FAILURE);
  }

  FILE *inputFile;
  inputFile = fopen(argv[1], "r");
  if (inputFile == NULL) {
    printf("Error opening %s\n",argv[1]);
    exit(EXIT_FAILURE);
  }

  char* sourcecode = read_textfile(inputFile);
  fclose(inputFile);

  unsigned int numberOfInstructions = 0;
  SymbolTable symbolTable = generate_symboltable(sourcecode,&numberOfInstructions);

  //allocate twice the number of instructions for worst-case scenario of all LDR instructions
  Instr *instructions = calloc((size_t) (numberOfInstructions * 2), sizeof(Instr));

  if (DEBUG) {
    print_symbol_table(&symbolTable);
  }

  int instrIndex = 0; //index for placing instruction in array
  int dataIndex = 0; //offset for data stored at the end of program
  for (int i = 0; i < symbolTable.currentSize; i++) {
    SymbolTableEntry *current = symbolTable.table+i;
    if (!current->isLabel) { //skip labels in symbolTable
      Token token;
      parse_instr(&token,current->symbol,current->address);

      //pass next address for data to be possibly stored by LDR after the program instructions
      unsigned int dataAddress = (numberOfInstructions+dataIndex)*4;
      Instr instr = create_instr_bits(token, &symbolTable, dataAddress);

      if (DEBUG) print_hex(instr);

      if (token.numericConstantFlag) { //place data at end of program
        instructions[numberOfInstructions+dataIndex]= (Instr) token.Transfer.address.expression;
        dataIndex++;
      }

      instructions[instrIndex] = instr;
      instrIndex++;
    }
  }

  free_symboltable(&symbolTable);
  free(symbolTable.table);
  free(sourcecode);

  char *outputFileName = argv[2];
  FILE *binaryFile = fopen(outputFileName,"wb");

  if (binaryFile == NULL) {
    printf("Error creating output file %s\n",outputFileName);
    exit(EXIT_FAILURE);
  }


  fwrite(instructions, sizeof(Instr), (size_t) (numberOfInstructions + dataIndex), binaryFile);

  free(instructions);


  exit(EXIT_SUCCESS);

}

