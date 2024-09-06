
#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <string.h>
#include <assert.h>

#include "utils.hpp"
#include "disas_pdp11.hpp"
#include "cycles.hpp"
#include "disas.hpp"

#include "instructions.hpp"
#include "disas.hpp"





// R = 3bit register
// DD = destination: 3bit mode, 3bit register
// SS = source: 3bit mode, 3bit register
// ofs = 8bit signed integer: -128..+127
// NN = number of parameters
// trapno = 8 bit trap/emt number = 0..255
// AC = 3bit: floating point accumulator 0..7
// FSRC = 6bit like SS, but register is floating point accumulator instead of CPU general registers
// FDST = 6bit like DD, but register is floating point accumulator instead of CPU general registers


InstructionDefinition instr_defs[] = {
    {   0177777, 0000000, "halt", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, // 000000
    {   0177777, 0000001, "wait", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, // 000001
    {   0177777, 0000002, "rti", IS_BASIC, DATACYCLES_READ, DATACYCLES_READ,	// 2x pop
        &Disassembler_Base::instr_rti
    }, // 000002
    {   0177777, 0000003, "bpt", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, // 000003
    {   0177777, 0000004, "iot", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, // 000004
    {   0177777, 0000005, "reset", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, // 000005
    {   0177777, 0000006, "rtt", IS_40, DATACYCLES_READ, DATACYCLES_READ,	// 2x pop
        &Disassembler_Base::instr_rti
    }, // 000006
    {   0177777, 0000007, "mfpt", IS_J11, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000010, "emhalt", IS_UNKNOWN, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177700, 0000100, "jmp", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single // wie single, load only effective address
        // &Disassembler_Base::instr_jmp // wie single, load only effective address
    }, // 0001DD
    {   0177770, 0000200, "rts", IS_BASIC, DATACYCLES_READ, DATACYCLES_NONE,	// pop one
        &Disassembler_Base::instr_rts
    }, // 00020R
    {   0177770, 0000230, "spl", IS_PSW, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_number_3bit
    }, // 00023N setpriority level
    {   0177777, 0000240, "nop", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000241, "clc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000242, "clv", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000243, "clvc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000244, "clz", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000245, "clzc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000246, "clzv", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000247, "clzvc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000250, "cln", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000251, "clnc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000252, "clnv", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000253, "clnvc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000254, "clnz", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000255, "clnzc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000256, "clnzv", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000257, "ccc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000261, "sec", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000262, "sev", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000263, "sevc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000264, "sez", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000265, "sezc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000266, "sezv", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000267, "sezvc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000270, "sen", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177777, 0000271, "senc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000272, "senv", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000273, "senvc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000274, "senz", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000275, "senzc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000276, "senzv", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    },
    {   0177777, 0000277, "scc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_zero
    }, //
    {   0177700, 0000300, "swab", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0003DD
    {   0177400, 0000400, "br", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 000400 + ofs
    {   0177400, 0001000, "bne", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 001000 + ofs
    {   0177400, 0001400, "beq", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 001400 + ofs
    {   0177400, 0002000, "bge", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 002000 + ofs
    {   0177400, 0002400, "blt", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 002400 + ofs
    {   0177400, 0003000, "bgt", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 003000 + ofs
    {   0177400, 0003400, "ble", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 003400 + ofs
    {   0177000, 0004000, "jsr", IS_BASIC, DATACYCLES_READ, DATACYCLES_WRITE,	// load addr, push pc
        &Disassembler_Base::instr_jsr // wie &Disassembler_Base::instr_operands__register_plus_destination, load only effective address
    }, // 004RDD
    {   0177700, 0005000, "clr", IS_BASIC, DATACYCLES_WRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0050DD
    {   0177700, 0005100, "com", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0051DD
    {   0177700, 0005200, "inc", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0052DD
    {   0177700, 0005300, "dec", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0053DD
    {   0177700, 0005400, "neg", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0054DD
    {   0177700, 0005500, "adc", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0055DD
    {   0177700, 0005600, "sbc", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0056DD
    {   0177700, 0005700, "tst", IS_BASIC, DATACYCLES_READ, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0057DD
    {   0177700, 0006000, "ror", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0060DD
    {   0177700, 0006100, "rol", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0061DD
    {   0177700, 0006200, "asr", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0062DD
    {   0177700, 0006300, "asl", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0063DD
    {   0177700, 0006400, "mark", IS_40, DATACYCLES_READ, DATACYCLES_NONE, // pop
        &Disassembler_Base::instr_operands_number_6bit
    }, // 0064NN
    {   0177700, 0006700, "sxt", IS_40, DATACYCLES_READ, DATACYCLES_WRITE,
        &Disassembler_Base::instr_operands_single
    }, // 0067DD
    {   0177777, 0007100, "csm", IS_J11,   DATACYCLES_READ, DATACYCLES_WRITE, ///?? call to super visor mode
        &Disassembler_Base::instr_operands_single
},
    {   0177700, 0007200, "tstset", IS_J11, DATACYCLES_READ, DATACYCLES_WRITE,  // ???
        &Disassembler_Base::instr_operands_single
    },
    {   0177700, 0007600, "wrtlck", IS_J11, DATACYCLES_READ, DATACYCLES_WRITE, //???
        &Disassembler_Base::instr_operands_single
    },
    {   0170000, 0010000, "mov", IS_BASIC, DATACYCLES_READ, DATACYCLES_WRITE,
        &Disassembler_Base::instr_operands_double
    }, // 01SSDD
    {   0170000, 0020000, "cmp", IS_BASIC, DATACYCLES_READ, DATACYCLES_READ,
        &Disassembler_Base::instr_operands_double
    }, // 02SSDD
    {   0170000, 0030000, "bit", IS_BASIC, DATACYCLES_READ, DATACYCLES_READ,
        &Disassembler_Base::instr_operands_double
    }, // 03SSDD
    {   0170000, 0040000, "bic", IS_BASIC, DATACYCLES_READ, DATACYCLES_READWRITE,
        &Disassembler_Base::instr_operands_double
    }, // 04SSDD
    {   0170000, 0050000, "bis", IS_BASIC, DATACYCLES_READ, DATACYCLES_READWRITE,
        &Disassembler_Base::instr_operands_double
    }, // 05SSDD
    {   0170000, 0060000, "add", IS_BASIC, DATACYCLES_READ, DATACYCLES_READWRITE,
        &Disassembler_Base::instr_operands_double
    }, // 06SSDD
    {   0177000, 0070000, "mul", IS_EIS, DATACYCLES_READ, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_register_plus_src
    }, // 070RSS
    {   0177000, 0071000, "div", IS_EIS, DATACYCLES_READ, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_register_plus_src
    }, // 071RSS
    {   0177000, 0072000, "ash", IS_EIS, DATACYCLES_READ, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_register_plus_src
    }, // 072RSS
    {   0177000, 0073000, "ashc", IS_EIS, DATACYCLES_READ, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_register_plus_src
    }, // 073RSS
    {   0177000, 0074000, "xor", IS_40, DATACYCLES_READ, DATACYCLES_WRITE,
        &Disassembler_Base::instr_operands_register_plus_dest
    }, // 074RSS
#if 0
    { 0177770, 0075000, "fadd",   &Disassembler_Base::instr_operands_single_register},
    { 0177770, 0075010, "fsub",   &Disassembler_Base::instr_operands_single_register},
    { 0177770, 0075020, "fmul",   &Disassembler_Base::instr_operands_single_register},
    { 0177770, 0075030, "fdiv",   &Disassembler_Base::instr_operands_single_register},
#endif
    {   0177000, 0077000, "sob", IS_40, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_sob
    }, // 077R00 + ofs
    {   0177400, 0100000, "bpl", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 100000 + ofs
    {   0177400, 0100400, "bmi", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 100400 + ofs
    {   0177400, 0101000, "bhi", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 101000 + ofs
    {   0177400, 0101400, "blos", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 101400 + ofs
    {   0177400, 0102000, "bvc", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 102000 + ofs
    {   0177400, 0102400, "bvs", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 102400 + ofs
    {   0177400, 0103000, "bhis", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 103000 + ofs
    {   0177400, 0103400, "blo", IS_BASIC, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_branch
    }, // 103400 + ofs
    {   0177400, 0104000, "emt", IS_BASIC, DATACYCLES_WRITE, DATACYCLES_WRITE, // push pc, psw
        &Disassembler_Base::instr_trap
    }, // 104000 + trapno.
    {   0177400, 0104400, "trap", IS_BASIC, DATACYCLES_WRITE, DATACYCLES_WRITE, // push pc, psw
        &Disassembler_Base::instr_trap
    }, // 104400 + trapno
    {   0177700, 0105000, "clrb", IS_BASIC, DATACYCLES_WRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1050DD
    {   0177700, 0105100, "comb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1051DD
    {   0177700, 0105200, "incb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1052DD
    {   0177700, 0105300, "decb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1053DD
    {   0177700, 0105400, "negb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1054DD
    {   0177700, 0105500, "adcb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1055DD
    {   0177700, 0105600, "sbcb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1056DD
    {   0177700, 0105700, "tstb", IS_BASIC, DATACYCLES_READ, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1057DD
    {   0177700, 0106000, "rorb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1060DD
    {   0177700, 0106100, "rolb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1061DD
    {   0177700, 0106200, "asrb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1062DD
    {   0177700, 0106300, "aslb", IS_BASIC, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1063DD
    {   0177700, 0106400, "mtps", IS_PSW, DATACYCLES_WRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1064SS
    {   0177700, 0006500, "mfpi", IS_MMU, DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0065SS
    { 0177700, 0106500, "mfpd", IS_MMU,   DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1065SS
    { 0177700, 0006600, "mtpi", IS_MMU,   DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 0066SS
    {   0177700, 0106600, "mtpd", IS_MMU,   DATACYCLES_READWRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1066SS
    {   0177700, 0106700, "mfps", IS_PSW, DATACYCLES_WRITE, DATACYCLES_NONE,
        &Disassembler_Base::instr_operands_single
    }, // 1067DD
    {   0170000, 0110000, "movb", IS_BASIC, DATACYCLES_READ, DATACYCLES_WRITE,
        &Disassembler_Base::instr_operands_double
    }, // 11SSDD
    {   0170000, 0120000, "cmpb", IS_BASIC, DATACYCLES_READ, DATACYCLES_READ,
        &Disassembler_Base::instr_operands_double
    }, // 12SSDD
    {   0170000, 0130000, "bitb", IS_BASIC, DATACYCLES_READ, DATACYCLES_READ,
        &Disassembler_Base::instr_operands_double
    }, // 13SSDD
    {   0170000, 0140000, "bicb", IS_BASIC, DATACYCLES_READ, DATACYCLES_READWRITE,
        &Disassembler_Base::instr_operands_double
    }, // 14SSDD
    {   0170000, 0150000, "bisb", IS_BASIC, DATACYCLES_READ, DATACYCLES_READWRITE,
        &Disassembler_Base::instr_operands_double
    }, // 15SSDD
    {   0170000, 0160000, "sub", IS_BASIC, DATACYCLES_READ, DATACYCLES_READWRITE,
        &Disassembler_Base::instr_operands_double
    }, // 16SSDD
    // single precision floating point
    {   0177777, 0170000, "cfcc", IS_FPP, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_fp__operands_zero
    }, // 170000
    {   0177777, 0170001, "setf", IS_FPP,  DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_fp__operands_zero
    }, // 170001
    {   0177777, 0170002, "seti", IS_FPP, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_fp__operands_zero
    }, // 170002
    {   0177777, 0170011, "setd", IS_FPP, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_fp__operands_zero
    }, // 170011
    {   0177777, 0170012, "setl",  IS_FPP, DATACYCLES_NONE, DATACYCLES_NONE,
        &Disassembler_Base::instr_fp__operands_zero
    }, // 170012
    {   0177700, 0170100, "ldfps", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_source
    }, // 170100 + SS
    {   0177700, 0170200, "stfps", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_destination
    }, // 170200 + DD
    {   0177700, 0170300, "stst", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_destination
    }, // 170300 + DD
    {   0177700, 0170400, "clrf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_fdst
    }, // 170400 + FDST
    {   0177700, 0170500, "tstf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_fdst
    }, // 170500 + FDST
    {   0177700, 0170600, "absf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_fdst
    }, // 170600 + FDST
    {   0177700, 0170700, "negf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_fdst
    }, // 170700 + FDST
    {   0177400, 0171000, "mulf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fsrc
    }, // 171000 + AC FSRC
    {   0177400, 0171400, "modf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fsrc
    }, // 171400 + AC FSRC
    {   0177400, 0172000, "addf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fsrc
    }, // 172000 + AC FSRC
    {   0177400, 0172400, "ldf",  IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fsrc
    }, // 172400 + AC FSRC
    {   0177400, 0173000, "subf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fsrc
    }, // 173000 + AC FSRC
    {   0177400, 0173400, "cmpf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fsrc
    }, // 173400 + AC FSRC
    {   0177400, 0174000, "stf",  IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fdst
    }, // 174000 + AC FDST
    {   0177400, 0174400, "divf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fsrc
    }, // 174400 + AC FSRC
    {   0177400, 0175000, "stexp", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_destination
    }, // 175000 + AC DD
    {   0177400, 0175400, "stcfi", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_destination
    }, // 175400 + AC DD
    {   0177400, 0176000, "stcfd", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fdst
    }, // 176000 + AC FDST
    {   0177400, 0176400, "ldexp", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_source
    }, // 176400 + AC SS
    {   0177400, 0177000, "ldcif", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_source
    }, // 177000 + AC SS
    {   0177400, 0177400, "ldcdf", IS_FPP, DATACYCLES_TBD, DATACYCLES_TBD,
        &Disassembler_Base::instr_fp__operands_ac_plus_fsrc
    } // 177400 + AC FSRC
#ifdef NEED_ALT_DEFS
    ,
    { 0177400, 0103000, "bcc",    &Disassembler_Base::instr_branch},
    { 0177400, 0103400, "bcs",    &Disassembler_Base::instr_branch},
    // double precision floating point
    { 0177700, 0170400, "clrd",   &Disassembler_Base::instr_fp__operands_fdst}, // 170400 + FDST
    { 0177700, 0170500, "tstd",   &Disassembler_Base::instr_fp__operands_fdst},
    { 0177700, 0170600, "absd",   &Disassembler_Base::instr_fp__operands_fdst}, // 170600 + FDST
    { 0177700, 0170700, "negd",   &Disassembler_Base::instr_fp__operands_fdst}, // 170700 + FDST
    { 0177400, 0171000, "muld",   &Disassembler_Base::instr_fp__operands_ac_plus_fsrc}, // 171000 + AC FSRC
    { 0177400, 0171400, "modd",   &Disassembler_Base::instr_fp__operands_ac_plus_fsrc},
    { 0177400, 0172000, "addd",   &Disassembler_Base::instr_fp__operands_ac_plus_fsrc}, // 172000 + AC FSRC
    { 0177400, 0172400, "ldd",    &Disassembler_Base::instr_fp__operands_ac_plus_fsrc}, // 172400 + AC FSRC
    { 0177400, 0173000, "subd",   &Disassembler_Base::instr_fp__operands_ac_plus_fsrc}, // 173000 + AC FSRC
    { 0177400, 0173400, "cmpd",   &Disassembler_Base::instr_fp__operands_ac_plus_fsrc}, // 173400 + AC FSRC
    { 0177400, 0174000, "std",    &Disassembler_Base::instr_fp__operands_ac_plus_fdst}, // 174000 + AC FDST
    { 0177400, 0174400, "divd",   &Disassembler_Base::instr_fp__operands_ac_plus_fsrc}, // 174400 + AC FSRC
    { 0177400, 0175400, "stcfl",  &Disassembler_Base::instr_fp__operands_ac_plus_destination}, // 175400 + AC DD
    { 0177400, 0175400, "stcdi",  &Disassembler_Base::instr_fp__operands_ac_plus_destination}, // 175400 + AC DD
    { 0177400, 0175400, "stcdl",  &Disassembler_Base::instr_fp__operands_ac_plus_destination}, // 175400 + AC DD
    { 0177400, 0176000, "stcdf",  &Disassembler_Base::instr_fp__operands_ac_plus_fdst}, // 176000 + AC FDST
    { 0177400, 0177000, "ldclf",  &Disassembler_Base::instr_fp__operands_ac_plus_source},
    { 0177400, 0177000, "ldcid",  &Disassembler_Base::instr_fp__operands_ac_plus_source},
    { 0177400, 0177000, "ldcld",  &Disassembler_Base::instr_fp__operands_ac_plus_source}, // 177000 + AC SS
    {   0177400, 0177400, "ldcfd",  &Disassembler_Base::instr_fp__operands_ac_plus_fsrc)  // 177400 + AC FSRC
#endif
    };

    // count the elements
        int instr_def_n = (sizeof(instr_defs) / sizeof(instr_defs[0]));



        /* id_cmp()
         * compare two instructions by field 'value'
         */
        static int id_cmp(const void *p1, const void *p2)
        {
            return((0xffff & (int)((InstructionDefinition *)p1)->value) - (0xffff & (int)((InstructionDefinition *)p2)->value));
        }


        // sort the original instruction table by field 'value'
            void instruction_table_init(void)
            {
                qsort(instr_defs, instr_def_n, sizeof(InstructionDefinition), id_cmp);
            }


            /*
             * for a given 16bit word, find the instruction in the (sorted) table.
             * result: index in instr_defs[]
             * or DEF_UNUSED
             */
             InstructionDefinition *decodeInstruction(unsigned instruction)
             {
                 int l; // low bound
                 int m; // pivot in the middle
                 int h; // high bound
                 // binary search for instruction "inst" in instr_defs[]
                 // compared are "nmask" msb bits
                 l = 0; // l in range
                 h = instr_def_n; // h behind range
                 while (h - l > 1) {
                     m = (h + l) / 2;
                     if (instruction >= instr_defs[m].value)
                         l = m;
          else
           h = m;
   }
   if ((instruction & instr_defs[l].mask) != instr_defs[l].value)
       return NULL;
   else
       return &(instr_defs[l]);
}



