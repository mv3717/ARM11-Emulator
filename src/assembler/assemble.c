#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "assemble.h"
#include "typedefs.h"
#include "symboltable.h"


#define BRANCH_SPECIFIC 24
#define RN_POS 16
#define RD_POS 12
#define OFFSET_POS 0
#define RM_POS_MUL 0
#define RS_POS_MUL 8
#define RN_POS_MUL 12
#define RD_POS_MUL 16
#define ACC_POS 21
#define IMM_POS 25
#define PRE_POS 24
#define UP_POS 23
#define LDR_POS 20
#define SET_COND_POS 20
#define COND_POS 28
#define OP_2 0
#define SHIFT_T_POS 5
#define SHIFT_REG_POS 8
#define SHIFT_CONST_POS 7


static void create_data_proc(Token* token, Instr *instr);

static void create_mult(Token *token, Instr *instr);

static void create_trans(Token *token, Instr *instr, unsigned int dataAddress);

static void create_address(Address address, Instr *instr);

static void create_branch(Token *token, Instr *instr, SymbolTable* symbolTable);

static void create_special(Token *token, Instr *instr);

// Helper function that assigns bits at the right place in an instruction
static void assign_bits(Instr *instruction, uint32_t input, int from) {
  *instruction |= (input << from);
}

//creates the sequence of bits which corresponds to a given instruction.
Instr create_instr_bits(Token token, SymbolTable *symbolTable, unsigned int dataAddress) {
  Instr output = 0;
  assign_bits(&output, token.condition, COND_POS); //cond

  if (token.opcode == LSL) {        //special case
    create_special(&token, &output);

  } else if (token.opcode <= CMP) { //data processing
    create_data_proc(&token, &output);

  } else if (token.opcode <= MLA) { //mult
    create_mult(&token, &output);

  } else if (token.opcode <= STR) { //transfer
    create_trans(&token, &output, dataAddress);

  } else if (token.opcode <= B) {   //branch
    create_branch(&token, &output, symbolTable);
  } else {
    printf("Unexpected opcode in Token passed to create_instr_bits\n");
    exit(EXIT_FAILURE);
  }

  return output;
}

// Helper function that rotates right a uint32 a given amount of times.
static uint32_t rotate_right(uint32_t n, unsigned int shift_amount) {
  if ((shift_amount &= sizeof(n) * 8 - 1) == 0)
    return n;
  return (n << shift_amount) | (n >> (sizeof(n) * 8 - shift_amount));
}

//creates the sequence of bits which corresponds to a data processing instruction.
void create_data_proc(Token *token, Instr *instr) {
  assert(token != NULL);
  assign_bits(instr, (uint32_t) token->DataProcessing.op2.immediate, IMM_POS);

  uint32_t opcode;
  switch (token->opcode) {
    case ANDEQ:
    case AND:
      opcode = 0x0;
      break;
    case EOR:
      opcode = 0x1;
      break;
    case SUB:
      opcode = 0x2;
      break;
    case RSB:
      opcode = 0x3;
      break;
    case ADD:
      opcode = 0x4;
      break;
    case TST:
      opcode = 0x8;
      break;
    case TEQ:
      opcode = 0x9;
      break;
    case CMP:
      opcode = 0xa;
      break;
    case ORR:
      opcode = 0xc;
      break;
    case MOV:
      opcode = 0xd;
      break;
    default:
      printf("Unexpected opcode %x passed to create_data_proc.\n", token->opcode);
      exit(EXIT_FAILURE);
  }

  assign_bits(instr, opcode, 21);

  if (token->opcode == TST || token->opcode == TEQ || token->opcode == CMP) {
    assign_bits(instr, 1, SET_COND_POS);
  } else {
    assign_bits(instr, (int) token->DataProcessing.rd, RD_POS);
  }

  if (!(token->opcode == MOV)) {
    assign_bits(instr, token->DataProcessing.rn, RN_POS);
  }

  if (token->DataProcessing.op2.immediate) {

    int expression = token->DataProcessing.op2.expression;

    if (expression > 0xff) {

      int shiftAmount = 0;
      uint32_t value = (uint32_t) token->DataProcessing.op2.shiftedRegister.rm;
      while (value > 0xff) {
        shiftAmount += 1;
        value = rotate_right(value, 2);
      }
      assign_bits(instr, value, OP_2);
      assign_bits(instr, (uint32_t) shiftAmount, 8);

    } else {
      assign_bits(instr, (uint32_t) expression, OP_2);
    }

  } else {

    assign_bits(instr, (uint32_t) token->DataProcessing.op2.shiftedRegister.rm, OP_2);

    if (!token->DataProcessing.op2.shiftedRegister.shift.format) { //reg

      ShiftType shiftType = token->DataProcessing.op2.shiftedRegister.shift.shiftName;
      assign_bits(instr, (uint32_t) shiftType, SHIFT_T_POS);

      if (!(token->DataProcessing.op2.shiftedRegister.shift.shiftName == NO_SHIFT)) {
        assign_bits(instr, 1, 4);
        assign_bits(instr, token->DataProcessing.op2.shiftedRegister.shift.reg, 8);
      }
    }
  }
}

//creates the sequence of bits which corresponds to a multiply instruction.
void create_mult(Token *token, Instr *instr) {
  assert(token != NULL);
  assign_bits(instr, 9, 4);
  assign_bits(instr, token->Multiply.rd, RD_POS_MUL);
  assign_bits(instr, token->Multiply.rn, RN_POS_MUL);
  assign_bits(instr, token->Multiply.rs, RS_POS_MUL);
  assign_bits(instr, token->Multiply.rm, RM_POS_MUL);
  if (token->opcode == MLA) {
    assign_bits(instr, 1, ACC_POS);
  }
}

//creates the sequence of bits which corresponds to a data transfer instruction.
void create_trans(Token *token, Instr *instr, unsigned int dataAddress) {
  assert(token != NULL);
  assign_bits(instr, 1, 26);
  assign_bits(instr, (uint32_t) token->Transfer.rd, RD_POS);

  if (token->opcode == LDR) {
    assign_bits(instr, 1, LDR_POS);
  }

  if (token->Transfer.address.format == 0) {     // a numeric constant expression
    int expression = token->Transfer.address.expression;
    // use a mov instruction, use of data processing functions
    if (expression < 0xff) {
      Token movToken;                 // Create a token for MOV instruction
      movToken.opcode = MOV;
      movToken.DataProcessing.rd = token->Transfer.rd;
      movToken.DataProcessing.op2.immediate = 1;
      movToken.DataProcessing.op2.expression = token->Transfer.address.expression;
      *instr = 0;                    //need to reset instr to 0 and reassign AL condition bits
      assign_bits(instr, AL, 28);
      create_data_proc(&movToken, instr);

    } else {
      //-8 to compensate for PC being ahead 8 bytes of the instruction that is currently
      //being executed as a result of the ARM pipeline
      int offset = ((int) dataAddress) - ((int) token->address) - 8;

      assign_bits(instr, 15, RN_POS);
      assign_bits(instr, 1, PRE_POS);
      assign_bits(instr, 1, UP_POS);
      assign_bits(instr, (uint32_t) offset, OFFSET_POS);

    }
  } else {
    if (!token->Transfer.address.Register.prepost)
      assign_bits(instr, 1, PRE_POS);              // set the pre-index flag
    create_address(token->Transfer.address, instr);
  }
}

//creates the sequence of bits which corresponds to a branch instruction.
void create_branch(Token *token, Instr *instr, SymbolTable *symbolTable) {
  assert(token != NULL);
  assign_bits(instr, 0xa, BRANCH_SPECIFIC);
  int labelAddress = lookup_symboltable(symbolTable, token->Branch.expression);
  int difference =
      labelAddress - ((int) token->address) - 8; //-8 to compensate for effect of ARM pipeline
  difference >>= 2;
  difference &= 0x00ffffff;
  assign_bits(instr, (uint32_t) difference, 0);
}

//creates the seauence of bits which corresponds to a special instruction.
void create_special(Token *token, Instr *instr) {

  //only needed to treat lsl Rn <#expression> as mov Rn, Rn, <#expression>
  assert(token != NULL);
  uint32_t opcode;
  opcode = 0xd;
  assign_bits(instr, opcode, 21);
  assign_bits(instr, token->DataProcessing.rd, RD_POS);
  //mov Rn, Rn, lsl <#expression>
  assign_bits(instr, token->DataProcessing.rd, 0);
  assign_bits(instr, (uint32_t) token->DataProcessing.op2.expression, 7);
}

//creates the sequence of bits which corresponds to a given address.
void create_address(Address address, Instr *instr) {

  assign_bits(instr, (uint32_t) address.Register.rn, RN_POS); //assign base register

  if (address.Register.format == 0) {     //simple address-index with a direct expression
    if (address.Register.expression >= 0) {
      assign_bits(instr, 1,
                  UP_POS);                                              // assign up bit if positive
      assign_bits(instr, (uint32_t) address.Register.expression, OFFSET_POS);     // assign offset
    } else {
      assign_bits(instr, (uint32_t) (-1 * address.Register.expression),
                  OFFSET_POS);   // assign offset
    }

  } else {//operand 2 is a register

    //setting the basic flags
    assign_bits(instr, address.Register.Shift.rm, OFFSET_POS);
    assign_bits(instr, 1, IMM_POS);  //assign rm in offset
    switch (address.Register.Shift.shift.shiftName) {   // assign the type of rotation if it exists
      case ASR_SHIFT:
        assign_bits(instr, 2, SHIFT_T_POS);
        break;
      case LSL_SHIFT:
        assign_bits(instr, 0, SHIFT_T_POS);
        break;
      case LSR_SHIFT:
        assign_bits(instr, 1, SHIFT_T_POS);
        break;
      case ROR_SHIFT:
        assign_bits(instr, 3, SHIFT_T_POS);
        break;
      default:
        break;
    }

    if (address.Register.Shift.pm == 0) {//positive offset
      assign_bits(instr, 1, UP_POS);
    }
    //setting the offset which depends on on the format of the shift
    if (address.Register.Shift.shift.format == 0 &&
        address.Register.Shift.shift.shiftName !=
        NO_SHIFT) {           // shift specified by another register
      assign_bits(instr, 1, 4);
      assign_bits(instr, address.Register.Shift.shift.reg, SHIFT_REG_POS);
    } else if (address.Register.Shift.shift.shiftName !=
               NO_SHIFT &&
               address.Register.Shift.shift.format == 1) {  //shift specified by a constant
      assign_bits(instr, address.Register.Shift.shift.expression, SHIFT_CONST_POS);
    }
  }
}
