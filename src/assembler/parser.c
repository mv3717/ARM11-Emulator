#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "typedefs.h"
#include "parser.h"

#define DEBUG 0
#define HEX 16
#define DEC 10

static Operation get_opcode(const char *opcode);

static void parse_special(Token *token, const char *arguments);

static void parse_branch(Token *token, const char *label);

static Address parse_address(const char *addressstr, Token *token);

static Operand2 parse_operand2(const char *operand2str);

static void parse_data_proc(Token *token, const char *arguments);

static void parse_transfer(Token *token, const char * transstr);

static ShiftType get_shiftType(const char* shift);

static void parse_mult(Token *token, const char *multstr);

static int parse_expression(const char *exprstr);

static Shift parse_shift(const char *shiftstr);

/*
 * Entry point to the parser. Takes a pointer to a Token and extracts information contained in
 * instr into it by delegating to auxiliary functions below
 */
void parse_instr(Token *token, char *instr, unsigned int address) {

    assert(token != NULL);
    assert(instr != NULL);

    //arguments following the opcode
    char *args = split(&instr, ' ', 1);

    Operation operation = get_opcode(instr);

    token->opcode = operation;
    token->address = address;
    token->numericConstantFlag = 0;
    token->condition = AL;

    args = skip_whitespace(args);

    if (operation == LSL) {
        parse_special(token, args);

    } else if (operation <= CMP) {
        parse_data_proc(token, args);

    } else if (operation <= MLA) {
      parse_mult(token, args);

    } else if (operation <= STR) {
        parse_transfer(token, args);

    } else if (operation <= B) {
        parse_branch(token, args);

    } else {
        printf("Error in parse_instr.\n");
        exit(EXIT_FAILURE);
    }

}


//parses special instruction.
void parse_special(Token *token, const char *arguments) {
    //only needed for parsing lsl Rn, <#expression>

    char *firstPart = malloc(strlen((arguments)) + 1);
    strcpy(firstPart, arguments);
    char *operand2;

    operand2 = split(&firstPart, ',', 1);
    token->DataProcessing.rd = (unsigned int) atoi(firstPart + 1);
    token->DataProcessing.op2 = parse_operand2(operand2);
    free(firstPart);

}

//parses a data processing instruction.
void parse_data_proc(Token *token, const char *arguments) {

    char *firstPart = calloc(strlen((arguments)) + 1, sizeof(char));
    strcpy(firstPart, arguments);
    char *operand2;

    switch (token->opcode) {
        case MOV:
            //split rd and second argument
            operand2 = split(&firstPart, ',', 1);
            token->DataProcessing.rd = (unsigned int) atoi(firstPart + 1);
            break;
        case TST:
        case TEQ:
        case CMP:
        case LSL:
            operand2 = split(&firstPart, ',', 1);
            token->DataProcessing.rn = (unsigned int) atoi(firstPart + 1);
        break;
        case ANDEQ: token->condition = EQ; //additionally set condition
        default:
            operand2 = split(&firstPart, ',', 2);
            token->DataProcessing.rd = (unsigned int) atoi(firstPart + 1);
            token->DataProcessing.rn = (unsigned int) atoi(firstPart + 4);
    }
    token->DataProcessing.op2 = parse_operand2(skip_whitespace(operand2));
    free(firstPart);

}

//parses operand 2.
Operand2 parse_operand2(const char *operand2str) {
    Operand2 operand2;
    //check if operand2 is #expression or shifted register
    if (operand2str[0] == '#') {
        operand2.immediate = 1;
        operand2.expression = parse_expression(operand2str+1);
    } else { //shifted register
        operand2.immediate = 0;
        operand2.shiftedRegister.rm = (unsigned int) atoi(operand2str+1);

        char* shft = split((char **) &operand2str, ',', 1);

        if (!strcmp(shft,"")) { //no shift
          operand2.shiftedRegister.shift.shiftName = NO_SHIFT;
        } else {
            operand2.shiftedRegister.shift = parse_shift(skip_whitespace(shft));
        }
    }
    return operand2;
}

//parses an expression.
int parse_expression(const char *exprstr) {
    if (!strncmp(exprstr,"-",1)) { //negative
       if (!strncmp(exprstr+1,"0x",2)) {
           return (int) -strtol(exprstr+3,NULL,HEX);
       } else {
           return (int) -strtol(exprstr+1,NULL,DEC);
       }
    }
    //positive
    if (!strncmp(exprstr,"0x",2)) { //hex
        return (int) strtol(exprstr+2,NULL,HEX);
    } else {
        return (int) strtol(exprstr,NULL,DEC);
    }
}

//parses a shift operation.
Shift parse_shift(const char *shiftstr) {

    Shift shift;
    shift.shiftName = get_shiftType(shiftstr);

    char *firstPart = calloc(strlen(shiftstr) + 1, sizeof(char));
    strcpy(firstPart, shiftstr);
    char *secondPart = split(&firstPart, ' ', 1);

    if (secondPart[0] == 'r') { //shift a register
        shift.format = 0;
        shift.reg = (unsigned int) strtol(secondPart+1,NULL,DEC);
    } else { // shift #expression
        shift.format = 1;
        shift.expression = parse_expression(secondPart+1);
    }

    free(firstPart);
    return shift;
}

//identifies the type of shift.
ShiftType get_shiftType(const char *shift) {
    if (!strncmp(shift, "asr", 3)) {
        return ASR_SHIFT;
    }
    if (!strncmp(shift, "lsl", 3)) {
        return LSL_SHIFT;
    }
    if (!strncmp(shift, "lsr", 3)) {
        return LSR_SHIFT;
    }

    printf("UNEXPECTED SHIFT TYPE PASSED TO get_ShiftType\n");
    exit(EXIT_FAILURE);
}

//parses a multiply instruction.
void parse_mult(Token *token, const char *multstr) {
    char *argscpy = calloc(strlen(multstr) + 1, sizeof(char));
    strcpy(argscpy, multstr);

    char **tokens = split_until(&argscpy, ",", 3);

    token->Multiply.rd = (unsigned int) atoi(tokens[0] + 1);
    token->Multiply.rm = (unsigned int) atoi(tokens[1] + 1);
    token->Multiply.rs = (unsigned int) atoi(tokens[2] + 1);

    if (token->opcode == MLA) {
        token->Multiply.rn = (unsigned int) atoi(tokens[3] + 1);
    } else {
        token->Multiply.rn = 0;
    }

    free(argscpy);
}

//parses a transfer instruction.
void parse_transfer(Token *token, const char *transstr) {
    token->Transfer.rd = (unsigned int) atoi(transstr+1);

    char* argscpy = calloc(strlen(transstr) + 1, sizeof(char));
    strcpy(argscpy,transstr);

    char *addr = split(&argscpy,',',1);

    token->Transfer.address = parse_address(skip_whitespace(addr), token);

    free(argscpy);
}

//parses an address.
Address parse_address(const char *addressstr, Token *token) {
    Address address;
    if (addressstr[0] == '=') { //expression
        address.format = 0;
        token->Transfer.address.expression = parse_expression(addressstr+1);
        address.expression = parse_expression(addressstr+1);

        if(token->Transfer.address.expression >= 256) {
            token->numericConstantFlag = 1;
        }

        return address;
    }

    //shifted register
    address.format = 1;
    address.Register.rn = atoi(addressstr+2);

    if (addressstr[strlen(addressstr) - 1] == ']') { //entire argument in brackets -> preindexed
        address.Register.prepost = 0;
    } else {
        address.Register.prepost = 1; //postindexed
    }

    char *argscpy = calloc(strlen(addressstr)+1, sizeof(char));
    strcpy(argscpy,addressstr+1); //don't copy initial '[' char

    char *rmexpr= split(&argscpy,',',1); //extract value of rm/expression
    rmexpr = skip_whitespace(rmexpr); //remove whitespace

    if (!strcmp(rmexpr,"")) { //no offset
        address.Register.expression = 0;
        address.Register.format = 0;

    } else {

        if (rmexpr[0] == '#') { //offset by expression
            address.Register.format = 0;
            address.Register.expression = parse_expression(skip_whitespace(rmexpr) + 1);
        } else { //shifted register
            address.Register.format = 1;

            //check fo optional sign of register, take 0 (positive) when omitted
            if (argscpy[0] == '+' || argscpy[0] == '-') {
                address.Register.Shift.pm = (addressstr[0] == '+' ? 0 : 1);
                argscpy++;
            }
            address.Register.Shift.pm = 0;

            address.Register.Shift.rm = (unsigned int) atoi(skip_whitespace(rmexpr) + 1);

            char *shft = split(&rmexpr,',',1); //split at next comma for shift

            if (!(strcmp(shft,""))) { //no shift
                Shift noshift;
                noshift.shiftName = NO_SHIFT;
                address.Register.Shift.shift = noshift;
            } else {
                address.Register.Shift.shift = parse_shift(skip_whitespace(shft));
            }
        }
    }

    free(argscpy);
    return address;
}


/*
 * parses a branch instructions.
 */
void parse_branch(Token *token, const char *label) {
    assert(token != NULL);
    assert(label != NULL);

    switch (token->opcode) {
      case B:   token->condition = AL; break;
      case BEQ: token->condition = EQ; break;
      case BNE: token->condition = NE; break;
      case BGE: token->condition = GE; break;
      case BLT: token->condition = LT; break;
      case BGT: token->condition = GT; break;
      case BLE: token->condition = LE; break;
        default: printf("Unexpected opcode 0x%x passed to parse_branch",token->opcode);
        exit(EXIT_FAILURE);
    }

    token->Branch.expression = (char *) label;
}

/*
 * Given an opcode as a string, returns the corresponding enum
 */
Operation get_opcode(const char *opcode) {

    if (!strcmp(opcode, "add")) {
        return ADD;
    }

    if (!strcmp(opcode, "sub")) {
        return SUB;
    }

    if (!strcmp(opcode, "rsb")) {
        return RSB;
    }

    if (!strcmp(opcode, "and")) {
        return AND;
    }

    if (!strcmp(opcode, "eor")) {
        return EOR;
    }

    if (!strcmp(opcode, "orr")) {
        return ORR;
    }

    if (!strcmp(opcode, "mov")) {
        return MOV;
    }

    if (!strcmp(opcode, "tst")) {
        return TST;
    }

    if (!strcmp(opcode, "teq")) {
        return TEQ;
    }

    if (!strcmp(opcode, "cmp")) {
        return CMP;
    }

    if (!strcmp(opcode, "mul")) {
        return MUL;
    }

    if (!strcmp(opcode, "mla")) {
        return MLA;
    }

    if (!strcmp(opcode, "ldr")) {
        return LDR;
    }

    if (!strcmp(opcode, "str")) {
        return STR;
    }

    if (!strcmp(opcode, "b")) {
        return B;
    }

    if (!strcmp(opcode, "beq")) {
        return BEQ;
    }

    if (!strcmp(opcode, "bne")) {
        return BNE;
    }

    if (!strcmp(opcode, "bge")) {
        return BGE;
    }

    if (!strcmp(opcode, "blt")) {
        return BLT;
    }

    if (!strcmp(opcode, "bge")) {
        return BGE;
    }

    if (!strcmp(opcode, "bgt")) {
        return BGT;
    }

    if (!strcmp(opcode, "ble")) {
        return BLE;
    }

    if (!strcmp(opcode, "lsl")) {
        return LSL;
    }

    if (!strcmp(opcode, "andeq")) {
        return ANDEQ;
    }

    printf("Unexpected Mnemonic. Exiting.\n");
    exit(EXIT_FAILURE);
}


/*
 * Splits a string at the numOccurrence number of occurrence in a string by replacing it with a
 * '\0' character. Returns an empty string if no split was performed.
 */
char *split(char **stringptr, char separator, unsigned int numOccurrence) {

    int occ = 0;
    int i = 0;
    size_t len = strlen(*stringptr);
    while (i < len) {
        if ((*stringptr)[i] == separator) {
            occ++;
            if (occ == numOccurrence) break;
        }
        i++;
    }

    (*stringptr)[i] = '\000';
    return (*stringptr)+i+1;
}




/*
 * Splits the string pointed to by stringptr at the first numOccurrences number of occurrences of
 * characters contained in separator by replacing them with a '\0' character. Allocates an array
 *  to store the pointers to the tokens thus generated and returns it to the caller.
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
 * Given a pointer to a string, returns a pointer to the first non-whitespace character therein.
 */
char* skip_whitespace(const char *string) {
    char *copy = (char*) string;
    while (isspace(*copy)) {
        copy++;
    }
    return copy;
}
