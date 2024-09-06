#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

#ifndef C_INTERFACE_ONLY
/*
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    double operand
   |  opcode   |   source spec   |     dest spec   |    010000:067777
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    110000:167777

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    register + operand
   |        opcode      | src reg|     dest spec   |    004000:004777
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    070000:077777

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    single operand
   |           opcode            |     dest spec   |    000100:000177
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    000300:000377
                                                        005000:007777
                                                        105000:107777

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    single register
   |                opcode                |dest reg|    000200:000207
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    000230:000237

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    no operand
   |                     opcode                    |    000000:000007
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    branch
   |       opcode          |  branch displacement  |    000400:003477
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    100000:103477

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    EMT/TRAP
   |       opcode          |       trap code       |    104000:104777
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    cond code operator
   |                opcode             | immediate |    000240:000277
   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

   An operand specifier consists of an addressing mode and a register.
   The addressing modes are:

   0    register direct         R               op = R
   1    register deferred       (R)             op = M[R]
   2    autoincrement           (R)+            op = M[R]; R = R + length
   3    autoincrement deferred  @(R)+           op = M[M[R]]; R = R + 2
   4    autodecrement           -(R)            R = R - length; op = M[R]
   5    autodecrement deferred  @-(R)           R = R - 2; op = M[M[R]]
   6    displacement            d(R)            op = M[R + disp]
   7    displacement deferred   @d(R)           op = M[M[R + disp]]


... and more ...

An operand specifier consists of an addressing mode and a register.
The addressing modes are:

0	 register direct		 R				 op = R
1	 register deferred		 (R)			 op = M[R]
2	 autoincrement			 (R)+			 op = M[R]; R = R + length
3	 autoincrement deferred  @(R)+			 op = M[M[R]]; R = R + 2
4	 autodecrement			 -(R)			 R = R - length; op = M[R]
5	 autodecrement deferred  @-(R)			 R = R - 2; op = M[M[R]]
6	 displacement			 d(R)			 op = M[R + disp]
7	 displacement deferred	 @d(R)			 op = M[M[R + disp]]

There are eight general registers, R0-R7.  R6 is the stack pointer,
R7 the PC.	The combination of addressing modes with R7 yields:

27	 immediate				 #n 			 op = M[PC]; PC = PC + 2
37	 absolute				 @#n			 op = M[M[PC]]; PC = PC + 2
67	 relative				 d(PC)			 op = M[PC + disp]
77	 relative deferred		 @d(PC) 		 op = M[M[PC + disp]]

// types of instruction operands

   R = 3bit register
   DD = destination: 3bit mode, 3bit register
   SS = source: 3bit mode, 3bit register
   ofs = 8bit signed integer: -128..+127
   N = 3bit number
   NN = 6bit number
   trapno = 8 bit trap/emt number = 0..255
   AC = 3bit: floating point accumulator 0..7
   FSRC = 6bit like SS, but regsiter is floating point accumulator instead of CPU general registers
   FDST = 6bit like DD, but regsiter is floating point accumulator instead of CPU general registers

*/
typedef enum {
    OPERANDS_ZERO = 0, // no operands
    OPERANDS_SINGLE_DST, //  opocde + destination DD
    OPERANDS_SINGLE_SRC, //  opocde + destination SS
    OPERANDS_SINGLE_REGISTER,
    OPERANDS_NUMBER_3BIT,
    OPERANDS_NUMBER_6BIT,
    OPERANDS_BRANCH_DISPLACEMENT,
    OPERANDS_REGISTER_PLUS_DESTINATION,
    OPERANDS_REGISTER_PLUS_SOURCE,
    OPERANDS_DOUBLE,
    OPERANDS_REGISTER_PLUS_BRANCH_DISPLACEMENT,
    OPERANDS_TRAPCODE,
    // fp11-a floating point formats
    OPERANDS_FP_ZERO, // 11/34 F5
    OPERANDS_FP_SOURCE, // 11/34 F4
    OPERANDS_FP_DESTINATION, // 11/34 F4
    OPERANDS_FP_FDST, // 11/34 F2
    OPERANDS_FP_AC_PLUS_FDST,   // 11/34 F1
    OPERANDS_FP_AC_PLUS_FSRC,  // 11/34 F1
    OPERANDS_FP_AC_PLUS_SOURCE, // 11/34 F3
    OPERANDS_FP_AC_PLUS_DESTINATION // 11/34 F3
} instruction_operands_t;

// max number of bus cycles poduced by ececution of an opcode
#define MAX_BUSCYCLES_PER_INSTRUCTION 5
#define ADDRESS_UNKNOWN   0xffffffff

// data cycles for 1st and 2nd operand
#define DATACYCLES_NONE			0	// generate only effective address (jmp, jsr)
#define DATACYCLES_READ			1
#define DATACYCLES_WRITE		2
#define DATACYCLES_READWRITE	3	// read,modify,write instructions
#define DATACYCLES_TBD			0xff // to be defined in further versions


/* Different instruction sets 
 * Assignment of isntructiosn to these constants 
 * needs REALLY to be checked again!
 */
#define IS_BASIC    0x01   // 11/20, '05
#define IS_40      0x02    // addedd with 11/40
#define IS_EIS      0x04    // '34
#define IS_MMU     0x08    // other memory spaces: MTPD/I,  MFPD/I '34
#define IS_PSW     0x10    // Access to procesor status word: SPL,MTPS,MFPS
#define IS_FIS    0x20    // Floating Instruction Set 11/35,40,03
#define IS_FPP    0x40    // Floating Point Processor, 11/34, '45, FP11A
#define IS_CIS    0x80    // Commercial Instruction Set
#define IS_J11    0x100    // first detected in EK-KDJ1A-UG-002
#define IS_UNKNOWN   0x1000    // why listed?

class Disassembler_Base;
struct InstructionDefinitionStruct;

// Pointer to a instruction disassembling function
typedef void (Disassembler_Base::*InstructionDisassemblingFunction)(InstructionDefinitionStruct * instructionDefinition,
    char *operands_text);

// description of an instruction
typedef struct InstructionDefinitionStruct {
    unsigned mask;	//  opcode mask
    unsigned value; // opcode value
    const char *name; // assembler mnemonic
    unsigned instruction_set; // bit mask coded IS_BASIC/EIS/FP11A
    unsigned	operand1_data_direction; // DATADIR_*: expected bus cycle to fetch/write operands
    unsigned	operand2_data_direction;

    InstructionDisassemblingFunction disassemblingFunction;
} InstructionDefinition;


extern InstructionDefinition instr_defs[];
extern int instr_def_n;


void instruction_table_init(void);
InstructionDefinition * decodeInstruction(unsigned instruction);

#endif // C_INTERFACE_ONLY
#endif
