
#ifndef _DISAS_H_
#define _DISAS_H_

#include <string.h>
#include "utils.hpp"
#include "disas_pdp11.hpp"
#include "instructions.hpp"
#include "cycles.hpp"


/* supported CPU's */
// numbers must be continuous up from 0
#define DISAS_CPU_NONE	0   // none selected
#define DISAS_CPU_34	1	// PDP-11/34: M8265,M8266
#define DISAS_CPU_34FPP  2	// PDP-11/34 with floating point
#define DISAS_CPU_05	3	// PDP-11/05
#define DISAS_CPU_KDF11 4	// all QBUS CPUs with FONZ chipset, 11/23
#define DISAS_CPU_KD11FHLNPQ 5	// all LSI11/03 variants with WD chipset
#define DISAS_CPU_40    6	// PDP11/40

//#define DISAS_CPU_J11	5	// all QBus cpus with J11 chip


// several types of logic violation in disassembly try
#define	DISAS_ERROR_OK	0
#define	DISAS_ERROR_CYCLECOUNT 1	// not enough bus cycle for opcode sampled
#define	DISAS_ERROR_MISMATCH_CYCLETYPE 2	// wrong bus cycle: example: DATI expected, DATO found
#define	DISAS_ERROR_INSTRUCTION_INVALID	3 // not a PDP-11 isntruction
#define	DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED	4 // cycle analysis not yet programmed

#ifndef C_INTERFACE_ONLY

#include <exception>
#include <vector>




// Standard Disassembler, developed for 11/34
class Disassembler_Base {
    //    friend class DisassemblerMismatchException;
    friend class Disassembler_KD11B;
    friend class Disassembler_KD11E;
	friend class Disassembler_KDF11;
public:
    virtual ~Disassembler_Base() {}

	 unsigned instruction_sets; // bit mask of IS_*

    // a vector has 4 cycles, access order differs
    enum TrapCycle {pushOldPC, pushOldPSW, fetchNewPC, fetchNewPSW};
    std::vector<enum TrapCycle> trapCycle; // indexed with cycle #
    // 11/34: trapCycle[0] = fetchNewPSW, pushOldPSW, pushOldPC, fetchNewPC


	// A CPU has internal registers, acccess to these appear not on UNIBUS/QBUS
	// example "mov #340,@#177776" ; set PSW
	// -> no DATO to 177776 on bus visible
	bool cpuInternalAddr[0x10000] ; // 64k flags, one for each address

    virtual InstructionDefinition *instr_opcode_fetch(void) ;


    /********************************************************
     *	instr_operands__<class>()
     *	generates instruction info for different instuction classes
     * Must be of type
     *	typedef unsigned	(* fn_disas_instruction_class_t)(
     *	InstructionDefinition * instruction_def,
     *		struct _unibus_cycle_t *unibus_cycles,
     *	unsigned	unibus_cycles_count,
        unsigned *cur_cycle_index) ;
     *
     *	On entry:
     *		'cur_cycle_index' points to opcode fetch cycle
     *		'unibus_cycles[cur_cycle_index]' filled with fetched opcode
     *	On exit:
     *		'unibus_cycles[cur_cycle_index]' filled with instruction def: "mov @#ADDR1,r2"
     *		'unibus_cycles[cur_cycle_index+1 ... +n filled
     *					"; fetch address from ADDR1 = 12345, result = 1002"
     *					"; fetch data from 1002r2, result = 4711.\n ; r2 now 4711"
     *
     */

    virtual void instr_operands_zero(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_operands_single(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_operands_single_register(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_operands_number_3bit(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_branch(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_jsr(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_rts(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_rti(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_operands_register_plus_src(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_operands_register_plus_dest(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_operands_number_6bit(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_operands_double(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_sob(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_trap(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_fp__operands_zero(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_fp__operands_source(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_fp__operands_destination(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_fp__operands_fdst(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_fp__operands_ac_plus_fsrc(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_fp__operands_ac_plus_fdst(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_fp__operands_ac_plus_source(InstructionDefinition * instruction_def, char *operands_text);

    virtual void instr_fp__operands_ac_plus_destination(InstructionDefinition * instruction_def, char *operands_text);

    //#define MAX_INSTRUCTION_DEFINITIONS 1000
    // static "struct" list, because of meber function pointers
    //InstructionDefinition instructionDefinitions[MAX_INSTRUCTION_DEFINITIONS];
    //unsigned instructionDefinitionsSize; // actual size of list

private:

    // access to cycle list
    int cycles_cur_idx(void) { return cycles->cur_idx; }

    pdp11bus_cycle_t *cycles_cur(void) {
        return cycles->cur();
    }

    bool cycles_cur_valid(void) {
        return cycles->is_idx_valid(cycles->cur_idx );
    }

    // look forward, but do not move current cycle
    bool cycles_next_valid(void) {
        return cycles->is_idx_valid(cycles->cur_idx + 1);
    }
    pdp11bus_cycle_t *cycles_next(void) {
        return &cycles->pdp11bus_cycles[cycles->cur_idx + 1];
    }

    // advance cur_idx
    void cycles_step_forward(void) {
        cycles->cur_idx++;
    }

    // advance to next cycle, and do check for timeout trap
    // of previous acccess
    void cycles_step_forward_with_timeout(void);


    const char *symbol_label(pdp11_address_val_t addr);
    void instr_operand_cycles(
        char *operands_text,
        unsigned *pc,
        unsigned mode, unsigned reg,
        unsigned data_cycles // DATACYCLES_*
    );



    void do_error(unsigned code, const char *errortxt);
    void do_cyclecount_error(int count, const char *opcode);

    void do_cycletype_error(unsigned buscycle, const char* errortext);

    pdp11_address_val_t data2addr(unsigned dataval);

    bool datadirection_matches_buscontrol(unsigned datadir, unsigned buscontrol);

    pdp11_address_val_t last_trap_vector_address;

public:
    // on 11/34, sometimes an extra DATIP before DATO is generated.
    bool param_tolerate_datip_before_dato;
    // on Angelos '05, sometimes an extra DATI before DATO is generated.
    bool param_tolerate_dati_before_dato;

    // counts disassembly errors for one instruction
    unsigned	last_error_code; // one of DISAS_*MISMATCH* etc.
    int 	last_instruction_error_count;
    char	last_error_text[1024];

    // tracks enter jsr/trap/Intr calls
    // must be initialized by caller of do_trap() and do_isntruction()
    int current_call_level;

    Disassembler_Base();

    void initialize(CycleList *disas_cycles);



    CycleList	*cycles; // cycle list the disassembler works on

    void do_trap(void);
    void do_instruction(void);

};


class DisassemblerMismatchException : public std::exception {
public:
    unsigned errorcode;
    DisassemblerMismatchException(Disassembler_Base *disassembler, unsigned errorcode, const char *errortxt) {
        this->errorcode = errorcode;
        disassembler->last_instruction_error_count++;
        disassembler->last_error_code = errorcode;
        strcpy_s(disassembler->last_error_text, sizeof(disassembler->last_error_text), errortxt);
    }
};

class DisassemblerTrapException : public std::exception {};

extern int param_cpu;
extern int param_ignore_unibus_control;

extern bool disas_abort;

#endif // C_INTERFACE_ONLY


#endif
