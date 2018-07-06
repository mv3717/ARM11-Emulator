#ifndef ARM11_44_TYPEDEFS_H
#define ARM11_44_TYPEDEFS_H
#include <stdint.h>

typedef enum {
    ANDEQ,
    ADD,
    SUB,
    RSB,
    AND,
    EOR,
    ORR,
    MOV,
    TST,
    TEQ,
    LSL,
    CMP,
    MUL,
    MLA,
    LDR,
    STR,
    BEQ,
    BNE,
    BGE,
    BLT,
    BGT,
    BLE,
    B
} Operation;



/*
 * Enum to represent the different shift types. The assigned values represent their binary
 * encodings in instructions.
 */
typedef enum {
    LSL_SHIFT = 0x0,
    LSR_SHIFT = 0x1,
    ASR_SHIFT = 0x2,
    ROR_SHIFT = 0x3,
    NO_SHIFT
} ShiftType;



/*
 * Represents the conditions under which instructions should be executed. The assigned values are
 * the encodings in instructions.
 */
typedef enum {
    EQ = 0x0,
    NE = 0x1,
    GE = 0xa,
    LT = 0xb,
    GT = 0xc,
    LE = 0xd,
    AL = 0xe
} Condition;




/*
 * From here on the typedefs represent the information contained in an instruction.
 */


/*
 * Encodes a shift.
 */
typedef struct {
    int format; //set to 0 if struct contains <shiftname> <register>, 1 for <shiftname> <#expression>
    ShiftType shiftName;
    union {
        unsigned int reg;
        int expression;
    };

} Shift;


/*
 * Encodes an operand2
 */
typedef struct {
    int immediate; //set to 1 if struct contains expression, 0 for shifted register
    union {
        int expression;
        struct {
            int rm;
            Shift shift;
        } shiftedRegister;
    };
} Operand2;



/*
 * Encodes the different ways an address can be represented
 */
typedef struct {
    int format; //set to 0 for <expression>, 1 for <register>
    union {
        int expression;
        struct {
            int format; //set to 0 for [Rn,<#expression>], 1 for [Rn,+/-Rm, <shift>]
            int prepost; //set to 0 for pre-indexing, 1 for post-indexing
            int rn;
            union {
                int expression;
                struct {
                    int pm; //set to 0 for +Rm, 1 for -Rm
                    unsigned int rm;
                    Shift shift;
                } Shift;
            };
        } Register;
    };
} Address;



/*
 * Token encodes all information contained in an instruction as a union of structs. The contents
 * of the union are determined based on the opcode
 */

typedef struct {
    unsigned int address;
    Operation opcode; //determine contents of union based on this
    unsigned char numArgs;
    int numericConstantFlag;
    Condition condition;
    union {
        struct {
            unsigned int rd;
            unsigned int rn;
            Operand2 op2;
        } DataProcessing;

        struct {
            unsigned int rd;
            unsigned int rm;
            unsigned int rs;
            unsigned int rn;
        } Multiply;

        struct {
            unsigned int rd;
            Address address;
        } Transfer;

        struct {
            char *expression;
        } Branch;
    };
} Token;


typedef uint32_t Instr;

#endif //ARM11_44_TYPEDEFS_H
