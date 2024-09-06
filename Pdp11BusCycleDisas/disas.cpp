
/*
    Diassemble UNIBUs cycles
    Major classes:
    - instruction fetches
    - isntruction operands
    - traps, interrupts
    - DMA

    MMU: there's no relation chip between "effective" addresses hold
    in register and offsets,
    and physical busadresses.
    !!! So never use 'pdp11bus_cycle.bus_addr'
    !!! to generate disassembled operands or comment text!

    Traps:
    the basic disassembly unit is
    <opcode fetch> <operand fetches> <data read> <data write>,
    max length is "mov @+111,@#2222": 1x opcode, 4x operands, 2x data - 7 cycles
    At any cycle a "bustimeout" trap can occur: <fetch vector 6 psw> <push <push> <fetch vector 4 pc>
    so max instructionsize is 10 (6+4)

    Central cycle access operators:
    cur_cycle()
    next_cycle_with_trap()
        next_cycle_with_trap() throws CYCLECOUNT when not enough cycles for trap vector candidate



*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef _MSC_VER
#include <windows.h>	// Debugging
#endif
#include "utils.hpp"

#include "symbols.hpp"
#include "cycles.hpp"

#include "instructions.hpp"

#include "disas.hpp"

// there are lots of "if (err = test()) return err ;"
// Todo: throw exceptions instead
// #pragma warning(disable: 4706)

/********************************************************
 * parameters
 */

 // code, for which CPU cycles are disassembled
int param_cpu = 0;

// Normally, argument cycles are checked for correct CONTROl signals (DATI, DATO, e..)
// suppress this?
int	param_ignore_unibus_control;


// Global flag to communicate a "STOP" to the disassembler
bool disas_abort = 0; // stop output to check

/********************************************************
 *	signal an error while disassemble
 *	error text is saved in extern global "LastErrorText"
 */

void Disassembler_Base::do_error(unsigned errorcode, const char *errortxt) {
    throw DisassemblerMismatchException(this, errorcode, errortxt);
}

/********************************************************
 * interpret a 16bit data word as address
 */
pdp11_address_val_t Disassembler_Base::data2addr(unsigned dataval) {
    pdp11_address_val_t result;
    result.val = dataval;
    result.iopage = (dataval >= 0xe000) ? 1 : 0;
    return result;
}

/********************************************************
 * check if address has a symbol for a MACRO11 code or data position.
 * addr must be of class ADDRTYPE_SYMBOL
 * success, return the symbol text.
 * If symbol found, format text for code label or nuemric literal
 */
const char *Disassembler_Base::symbol_label(pdp11_address_val_t addr) {
    static char buff[MAX_SYM_LEN];
    pdp11_symbol_t  *sym;
    sym = pdp11SymbolTable.get(addr);
    if (!sym || (sym->addrtype != ADDRTYPE_SYMBOL))
        return "";
    strcpy_s(buff, sizeof(buff), sym->text);
    strcat_s(buff, sizeof(buff), ":");
    return buff;
}

/* return octal digits or sy<mbol
 * both: return "symbol = octal"
 */
char *disas_symbol_addr(pdp11_address_val_t addr, bool both) {
    static char buff[MAX_INFO_STRING_LEN];
    pdp11_symbol_t  *sym;
    //dbg_break(addr == 0777564);
    dbg_break(addr.val == 0177170);
    sym = pdp11SymbolTable.get(addr);
    if (!sym || (sym->addrtype != ADDRTYPE_SYMBOL && sym->addrtype != ADDRTYPE_DEVICE_REG))
        sprintf_s(buff, sizeof(buff), "%o", addr.val);
    else if (!both)
        sprintf_s(buff, sizeof(buff), "%s", sym->text);
    else
        sprintf_s(buff, sizeof(buff), "%s=%o", sym->text, addr.val);
    return buff;
}


/********************************************************
 * check wether snippet contains at least "count" more cycles,
 * including "cur_idx"
 * if the cycle list is too short, current instruction is terminated with
 * DISAS_ERROR_CYCLECOUNT.
 */
void Disassembler_Base::do_cyclecount_error(int count, const char *opcode) {
    char	buffer[256];
    sprintf_s(buffer, sizeof(buffer), "'%s': %d bus cycle expected", opcode, count);
    int cycles_available = cycles->size() - cycles->cur_idx;
    assert(cycles_available >= 0);
    if (cycles_available < count)
        throw DisassemblerMismatchException(this, DISAS_ERROR_CYCLECOUNT, buffer);
}

/********************************************************
 * check wether current cycle is a correct DATI or DATO
 * else DISAS_ERROR_MISMATCH_CYCLETYPE
 * cycles added to complete last opcode are not tested
 */
void Disassembler_Base::do_cycletype_error(unsigned bus_cycle, const char* errortext) {
    if (cycles_cur()->bus_cycle == BUSCYCLE_FILLUP)
        return;
    if (cycles_cur()->bus_cycle != bus_cycle)
        do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, errortext);
}


/* advance to next cycle, and do check for timeout trap
 of previous acccess.
 A timeout trap is something like
 DATI from 6, fetch T/O PSW
 DATO n,
 DATO n+2
 Returns
 */

Disassembler_Base::Disassembler_Base()
{
    // clear lists
    memset(cpuInternalAddr, 0, sizeof(cpuInternalAddr));


    // 11/34 will activate
    param_tolerate_datip_before_dato = false;
    // Angelos '05 will activate
    param_tolerate_dati_before_dato = false;
}

void  Disassembler_Base::cycles_step_forward_with_timeout(void) {
    cycles->cur_idx++;

    // fast pre-check for timeout instruction, PC/PSW fetch order depends on CPU
    do_cyclecount_error(1, "bus timeout vector fetch "); // 1st cycle tested here
    if (cycles_cur()->bus_address.val != 4 && cycles_cur()->bus_address.val != 6)
        return;

    // is next a trap?
    int save_idx = cycles_cur_idx();
    int save_errorcount = last_instruction_error_count;
    try {
        do_trap();
        // that will trhow execpetions:
        // MISMATCH, if no proper trap seqeuence
        // CYCLECOUNT, if run into end of snippet
    }
    catch (DisassemblerMismatchException& ex) {
        if (ex.errorcode == DISAS_ERROR_CYCLECOUNT) {
            // check for trap could not be completed:
            // instruction is incomplete
            throw ex;
        }
        else {
            // rollback parse position, and continue with disassemblingFunction
            // analysis
            cycles->cur_idx = save_idx;
            last_instruction_error_count = save_errorcount;
            return; // no trap
        }
    }
    // at current cycle indeed a trap sequence is found.
    // only the timeout trap 4 may occur INSIDE instructions,
    // all other traps are only accepted BETWEEN instructions.
    // So only accept if "timout" trap here.
    if (last_trap_vector_address.val != 4) {
        // rollback parse position, and continue with disassemblingFunction
        cycles->cur_idx = save_idx; // no bus timeout trap
        last_instruction_error_count = save_errorcount;
    }
    // abort current instruction fetches
    throw DisassemblerTrapException();
}


// check, wether a bus control (DATI,DATAO,..) matches the operand type (src, dest)
// can be switched off
bool Disassembler_Base::datadirection_matches_buscontrol(unsigned datadir, unsigned buscontrol) {
    return 	param_ignore_unibus_control
        || (datadir == DATACYCLES_READ && (buscontrol == BUSCYCLE_DATI || buscontrol == BUSCYCLE_DATIP))
        || (datadir == DATACYCLES_WRITE && (buscontrol == BUSCYCLE_DATO || buscontrol == BUSCYCLE_DATOB))
        ;
}



/*************************************
 * fetch opcode and decode
 * instructionDefinition: result
 * */
InstructionDefinition * Disassembler_Base::instr_opcode_fetch() {
    InstructionDefinition *instructionDefinition;

    // handle different classes of opcode
    assert(cycles->is_cur_valid());

    // 1. fetch current instruction from unibus data stream
    // and disassemble into cycle list
// dbg_break(cycles_cur()->bus_address == 0406250);
    instructionDefinition = decodeInstruction(cycles_cur()->bus_data);
    if (instructionDefinition == NULL)
        do_error(DISAS_ERROR_INSTRUCTION_INVALID, "invalid instruction");

    // 2. now generated disassembled instruction_cycles[0]
    do_cycletype_error(BUSCYCLE_DATI, "Opcode fetch: DATI cycle expected");

    // instruction_cycles[0] = opcode fetch cycle: refined by instruction analyzing function
    cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPCODE;
    strcpy_s(cycles_cur()->disas_sym_label, MAX_SYM_LEN, symbol_label(cycles_cur()->bus_address));
    strcpy_s(cycles_cur()->disas_opcode, MAX_OPCODE_STRING_LEN, instructionDefinition->name);

    return instructionDefinition;
}




/*************************************
 *		disas_vector_fetch()
 *		check, whether cycles->cur is fetch of an trap or interrupt vector
 *
 *		A trap sequence consists of
 *		DATI fetch <cur_addr+2>PSW
 *		DATO push old psw onto stack <stack>
 *		DATO push old pc onto stack	<stack-2>
 *		DATI fetch <cur_addr> new pc
 *		That sequence is required, different CPUs may reorder this pattern.
 *
 *		if yes: result = 0, comment cycle, cur_idx on last cycle of sequence
 *		if no: result = error, cur_idx changed/invalid
 *	*/
void Disassembler_Base::do_trap()
{
    int cycle_start_idx = cycles->cur_idx;
    pdp11bus_cycle_t *cycle_start = cycles_cur();
    pdp11bus_cycle_t *cycle_fetch_psw = NULL;
    pdp11bus_cycle_t *cycle_fetch_pc = NULL;
    pdp11bus_cycle_t *cycle_push_psw = NULL;
    pdp11bus_cycle_t *cycle_push_pc = NULL;

    pdp11_symbol_t *text;
    // cur_idx on first cycle
    // on exit: cur-idx on last cycle

    /* Do not expect fix 4 cycles,
    Instead abort after mismatch of each expected DATI/DATO
    (Else very last cycles in last snippet never get disassembled)
    */
    // do_cyclecount_error(4, "trap push & vector fetch ");

    // 4 cycles in differing order
    for (int i = 0; i < 4; i++) {
        if (i > 0)
            cycles->cur_idx++;

        if (!cycles_cur_valid()) {
            cycles->cur_idx = cycle_start_idx; // roll back to start of potential vector
            do_error(DISAS_ERROR_CYCLECOUNT, "trap sequence ended early");
        }

        cycles_cur()->disas_class = DISAS_CYCLE_CLASS_VECTOR;
        strcpy_s(cycles_cur()->disas_opcode, MAX_OPCODE_STRING_LEN, "");
        strcpy_s(cycles_cur()->disas_operands, MAX_OPERAND_STRING_LEN, "");
        strcpy_s(cycles_cur()->disas_address_info, MAX_INFO_STRING_LEN, "");
        cycles_cur()->disas_call_level = current_call_level;

        switch (trapCycle[i]) {
        case pushOldPSW:
            cycle_push_psw = cycles_cur();
            do_cycletype_error(BUSCYCLE_DATO, "trap sequence push PSW expected");
            strcpy_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "irq/trap push PSW");
            strcpy_s(cycles_cur()->disas_address_info, MAX_INFO_STRING_LEN, "stack pointer");
            break;
        case pushOldPC:
            cycle_push_pc = cycles_cur();
            do_cycletype_error(BUSCYCLE_DATO, "trap sequence push PC expected");
            strcpy_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "irq/trap push PC");
            break;
        case fetchNewPC:
            cycle_fetch_pc = cycles_cur();
            text = pdp11SymbolTable.get(cycles_cur()->bus_address);
            // must be known vector!
            do_cycletype_error(BUSCYCLE_DATI, "trap sequence PC fetch expected");
            if (!text || text->addrtype != ADDRTYPE_VECTOR_PC)
                do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "trap sequence PC fetch expected");
            // unknown vector: must match "fetch vector" pattern:
            // vector = PC, vector+2 = PSW
            do_cycletype_error(BUSCYCLE_DATI, "trap sequence PC fetch address error");
            assert(text);
            sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "fetch PC. %.80s",
                text->info); // trunc info, so result < 128
            break;
        case fetchNewPSW:
            cycle_fetch_psw = cycles_cur();
            text = pdp11SymbolTable.get(cycles_cur()->bus_address);
            // must be known vector!
            if (!text || text->addrtype != ADDRTYPE_VECTOR_PSW)
                do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "trap sequence PSW fetch expected");

            //strcpy_s(cycle_fetch_psw->disas_opcode, MAX_OPCODE_STRING_LEN, "");
            assert(text);
            sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN,
                "fetch PSW. %.80s", text->info); // trunc info, so result < 128
            break;
        }
    }
    last_trap_vector_address = cycle_fetch_pc->bus_address;

    /*** are fetch & push addresses corelated? ***/
    // push of PSW and PC to adjacent stack locations
    if (cycle_push_pc->bus_address.val != cycle_push_psw->bus_address.val - 2)
        do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "trap push order PSW,PC violated");

    // fetch of PSW and PC from adjacent vector locations
    if (cycle_fetch_psw->bus_address.val != cycle_fetch_pc->bus_address.val + 2)
        do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "trap fetch order PC,PSW violated");

    // first cycles gets visual mark
    text = pdp11SymbolTable.get(last_trap_vector_address);
    assert(text);
    strcpy_s(cycle_start->disas_opcode, MAX_OPCODE_STRING_LEN, "; trap");
    sprintf_s(cycle_start->disas_operands, MAX_OPCODE_STRING_LEN, "\"%s\"", text->text);
}




/*
    disassembly bus cycles for operand
    opcode at unibus_cycles[cur_cycle_index]

mode:
   0    register direct         R               op = R
   1    register deferred       (R)             op = M[R]
   2    autoincrement           (R)+            op = M[R]; R = R + length
   3    autoincrement deferred  @(R)+           op = M[M[R]]; R = R + 2
   4    autodecrement           -(R)            R = R - length; op = M[R]
   5    autodecrement deferred  @-(R)           R = R - 2; op = M[M[R]]
   6    displacement            d(R)            op = M[R + disp]
   7    displacement deferred   @d(R)           op = M[M[R + disp]]

   Result: in 'unibus_cycles' the 'disas_*' field updated.
    size of cycle list "result_size" extended
    "processed_cycle_count" updated.
    text representation of operand in 'operand_text'
    return: one of DISAS_MATCH_*
    flag "data_cycle": normally true: data is fetched/stored from effective address
        for JMP, JSR: no data access is perofrmed. only effective address is generated

            Processes in 2 Phases
  Phase 1: generate effective address (EA)
        use register without cycle, load address, or use displacemeent
   Phase 2: access data using effective address
        data access controlled by "data_cycles
        none (jmp, jsr need only EA)
        read, write,
        read&write  (com, neg, inc, ...

    Special case for absolute addressed CÜU itnernal regsites (mode 3)
    Exmaple : "mov #340,@#177776"		; set PSW

    Input:
        mode , reg: addressing mode, and register number
        data_cycles: read, write, or read_write?
    Results:
        operand_text: disasembly, like "@1(r0)"
        pc: bytes, the pc was incremented by displacement fetch

   */


void Disassembler_Base::instr_operand_cycles(
    char *operands_text,
    unsigned *pc,
    unsigned mode, unsigned reg,
    unsigned data_cycles // DATACYCLES_*
)
{
    // dbg_msg("instr_operand_cycles(): pc before = %06o\n", *pc);
    const char    *sep;
    // save 
    char    prev_operands_text[MAX_OPERAND_STRING_LEN];
    strcpy_s(prev_operands_text, sizeof(prev_operands_text), operands_text);

    char effective_address_text[MAX_INFO_STRING_LEN];
    pdp11_address_val_t effective_addr; // address of operand data access
    effective_addr.val = 0; // init

    if (strlen(operands_text))
        sep = ",";
    else sep = "";

    switch (mode) {
    case 0: // register direct: no bus cycles
        sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s%s", prev_operands_text, sep, pdp11_regname[reg]);
        data_cycles = DATACYCLES_NONE; // correct param
        break;
    case 1: //  register deferred: 1 data fetch. EA = register
        sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s(%s)", prev_operands_text, sep, pdp11_regname[reg]);
        sprintf_s(effective_address_text, MAX_INFO_STRING_LEN, ", address = %s", pdp11_regname[reg]);
        break;
    case 2:  // auto increment:  1 data fetch  EA = register
        if (reg == 7)
            *pc += 2;
        sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s(%s)+", prev_operands_text, sep, pdp11_regname[reg]);
        // if PC immediate mode: operands_text is overwritten after data fetch

        //!!! JMP(rx) (und andere): ist wie single(dst), aber kein write cycle auf bus!!!!
        // wie mov (rc),pc
        sprintf_s(effective_address_text, MAX_INFO_STRING_LEN, ", address = %s", pdp11_regname[reg]);
        break;
    case 3: { //  autoincrement deferred: 2 fetch cycles, EA fetched from address in register
        cycles_step_forward_with_timeout(); // skip opcode
        do_cyclecount_error(1, "autoincrement deferred");
        // next bus cycle is DATI cycle register constant as address
        do_cycletype_error(BUSCYCLE_DATI, "DATI expected");
        cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
        effective_addr = data2addr(cycles_cur()->bus_data); // 
        if (effective_addr.val == 0177776)
            printf("PSW!\n");
        sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "fetch address of %s from address in %s",
            disas_symbol_addr(effective_addr, 1),
            pdp11_regname[reg]);
        strcpy_s(effective_address_text, MAX_INFO_STRING_LEN, ""); // address already in comment

        if (reg == 7) { // pc absolute address mode
            if (effective_addr.val == 0177776)
                printf("PSW\n!");
            *pc += 2;
            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s@#%s", prev_operands_text, sep, disas_symbol_addr(effective_addr, 0));
        }
        else
            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s@(%s)+", prev_operands_text, sep, pdp11_regname[reg]);
    }
            break;
    case 4: // auto decrement:  1 data fetch, EA = register
        if (reg == 7) // pc absolute address mode
            *pc += 2;
        sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s-(%s)", prev_operands_text, sep, pdp11_regname[reg]);
        sprintf_s(effective_address_text, MAX_INFO_STRING_LEN, ", address = %s", pdp11_regname[reg]);
        break;
    case 5: //  autodecrement deferred: 2 fetch cycles, EA fetched from address in register
        if (reg == 7) // pc absolute address mode
            *pc += 2;
        cycles_step_forward_with_timeout(); // skip opcode
        do_cyclecount_error(1, "autodecrement deferred");
        // next bus cycle is DATI cycle register constant as address
        do_cycletype_error(BUSCYCLE_DATI, "DATI expected");
        cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
        sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "fetch address of %s from address in %s",
            disas_symbol_addr(data2addr(cycles_cur()->bus_data), 1),
            pdp11_regname[reg]);
        sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s@-(%s)", prev_operands_text, sep, pdp11_regname[reg]);
        strcpy_s(effective_address_text, MAX_INFO_STRING_LEN, ""); // address already in comment
        break;
    case 6: { //  displacement d(R) op = M[R + disp], EA = displacement + register
        word displacement;

        // fetch displacement
        *pc += 2;
        cycles_step_forward_with_timeout(); // skip opcode
        do_cyclecount_error(1, "displacement");
        do_cycletype_error(BUSCYCLE_DATI, "fetch displacemement: DATI expected");
        displacement = (word)cycles_cur()->bus_data;
        cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
        sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "fetch displacement from PC");

        if (reg == 7) { // pc based relativ address: relocatable
            pdp11_address_val_t addr = data2addr((displacement + *pc) & 0xffff);
            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s%s", prev_operands_text, sep, disas_symbol_addr(addr, 0)); // skip displ, pc inc'ed
//            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s%o", prev_operands_text, sep, (displacement + *pc) & 0xffff); // skip displ, pc inc'ed
//            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s.+%o", prev_operands_text, sep, displacement + 2 + *pc_offset); // skip displ, pc inc'ed
        }
        else
            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s%o(%s)", prev_operands_text, sep, displacement, pdp11_regname[reg]);
        sprintf_s(effective_address_text, MAX_INFO_STRING_LEN, ", address = %o+%s",
            displacement,
            pdp11_regname[reg]);
        // content of reg can not be deduced from bus_addr: MMU!
        // will be guessed in data access
    }
            break;
    case 7: { //    displacement deferred   @d(R)           op = M[M[R + disp]], EA = fetched from displacement + register
        word displacement;

        // fetch displacement
        *pc += 2;
        cycles_step_forward_with_timeout(); // skip opcode
        do_cyclecount_error(2, "displacement deferred");
        do_cycletype_error(BUSCYCLE_DATI, "fetch displacemement: DATI expected");
        displacement = (word)cycles_cur()->bus_data;
        cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
        sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "fetch displacement from PC");
        cycles_step_forward_with_timeout();

        // fetch address
        do_cycletype_error(BUSCYCLE_DATI, "DATI expected");
        cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
        sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "fetch address from %o+%s", displacement,
            pdp11_regname[reg]);
        // content of reg can not be deduced from bus_addr: MMU!

        if (reg == 7) { // pc based relativ address deferred: relocatable
            pdp11_address_val_t addr = data2addr((displacement + *pc) & 0xffff);
            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s@%s", prev_operands_text, sep, disas_symbol_addr(addr, 0)); // skip displ, pc inc'ed
//            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s@(%o)", prev_operands_text, sep, (displacement + *pc) & 0xffff);
        //            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s@(.+%o)", prev_operands_text, sep, displacement + 2 + *pc_offset);
        }
        else
            sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s@%o(%s)", prev_operands_text, sep, displacement, pdp11_regname[reg]);
        strcpy_s(effective_address_text, MAX_INFO_STRING_LEN, ""); // already in comment

    }
            break;
    }

    /*** Phase 2: generate data access cycles with effective address ***/
    if (cpuInternalAddr[effective_addr.val]) {
        // no cycle visible on BUS. 
        strcat_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, ", next cycle CPU local");
    }
    else {
        if (data_cycles == DATACYCLES_READ || data_cycles == DATACYCLES_READWRITE) {
            cycles_step_forward_with_timeout(); // skip last cycle of instruction fetch
            do_cyclecount_error(1, "data read");
            if (!datadirection_matches_buscontrol(DATACYCLES_READ, cycles_cur()->bus_cycle)) // verify data access direction
                do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "src/dest mode does not match UNIBUS control signals");

            cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
            sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "read data%.80s",
                effective_address_text); // trunc info, so result < 128
            // only case, were 'data' is used for opcode operand expression
            if (reg == 7 && mode == 2) // pc immediate mode, replace "(pc)+" with "#"constant
                sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s%s#%o", prev_operands_text, sep, cycles_cur()->bus_data);
        }

        if (data_cycles == DATACYCLES_WRITE || data_cycles == DATACYCLES_READWRITE) {
            cycles_step_forward_with_timeout(); // skip last cycle of instruction fetch
            do_cyclecount_error(1, "data write");
            // sometimes a DATIP is generated before DATO, even if not necessary.
            // on 11/34: for all CLR, and SUB in some address modes?
            if (((param_tolerate_datip_before_dato && cycles_cur()->bus_cycle == BUSCYCLE_DATIP)
                || (param_tolerate_dati_before_dato && cycles_cur()->bus_cycle == BUSCYCLE_DATI))
                && cycles_next_valid()
                && datadirection_matches_buscontrol(DATACYCLES_WRITE, cycles_next()->bus_cycle))
            {
                // tolerable DATI(P) before DATO
                cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
                sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "extra DATI on dest data%.80s",
                    effective_address_text); // trunc info, so result < 128
                cycles_step_forward_with_timeout();
                do_cyclecount_error(1, "data write");
            }
            if (!datadirection_matches_buscontrol(DATACYCLES_WRITE, cycles_cur()->bus_cycle)) // verify data access direction
                do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "src/dest mode does not match UNIBUS control signals");

            cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
            sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "write data%.80s",
                effective_address_text);  // trunc info, so result < 128
        }
    }
    //dbg_msg("instr_operand_cycles(): pc after = %06o\n", *pc);

}

void Disassembler_Base::instr_operands_zero(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    no operand
    // |                     opcode                    |    000000:000007
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    UNREFERENCED_PARAMETER(operands_text);
    strcpy_s(
        cycles_cur()->disas_opcode, MAX_OPCODE_STRING_LEN,
        instruction_def->name);
}


void Disassembler_Base::instr_operands_single(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    single operand
    // |           opcode            |     dest spec   |    000100:000177
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    000300:000377

    unsigned dst_mode = (cycles_cur()->bus_data >> 3) & 7;
    unsigned dst_reg = (cycles_cur()->bus_data >> 0) & 7;
    unsigned pc = cycles_cur()->bus_address.val + 2;

    // one operand
    // returns 0..3 disassembled cycles
    instr_operand_cycles(operands_text, &pc, dst_mode, dst_reg, instruction_def->operand1_data_direction);
    // throws DisassemblyException
}


void Disassembler_Base::instr_operands_single_register(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    single register
    // |                opcode                |dest reg|    000200:000207
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    000230:000237
    UNREFERENCED_PARAMETER(instruction_def);
    unsigned reg = cycles_cur()->bus_data & 7;

    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s", pdp11_regname[reg]);
    // uc->disas_class = DISAS_CYCLE_CLASS_OPCODE ;
    // sprintf_s(uc->disas_comment, MAX_INFO_STRING_LEN, "access data");
}


void Disassembler_Base::instr_operands_number_3bit(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    unsigned n = cycles_cur()->bus_data & 7; // mask 3 bits

    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%o", n);
}

void Disassembler_Base::instr_branch(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    branch
    // |       opcode          |  branch displacement  |    000400:003477
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    100000:103477
    UNREFERENCED_PARAMETER(instruction_def);
    int  displacement_fetched = cycles_cur()->bus_data & 0xff; // +/- 128 words
    int  displacement_bytes;
    unsigned new_pc;

    if (displacement_fetched & 0x80) {
        // negative
        displacement_bytes = displacement_fetched | 0xffffff00; // sign extend: 0xff -> 0xffffffff
        // displacement = -displacement ; // strip off sign: 0xffffffff = -1 -> +1
        displacement_bytes = 2 * displacement_bytes + 2;
        new_pc = cycles_cur()->bus_address.val + displacement_bytes;
        if (displacement_bytes != 0)
            sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "branch to PC = %o (if no MMU)", new_pc & 0xffff);
    }
    else {
        displacement_bytes = 2 * displacement_fetched + 2;
        new_pc = cycles_cur()->bus_address.val + displacement_bytes;
        sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "branch to PC = %o (if no MMU)", new_pc & 0xffff);
    }
    // operand = absolute address = PC + displacement
    if (displacement_bytes == 0)
        sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "."); // loop to same addr
    else if (displacement_bytes > 0)
        sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, ". + %o", displacement_bytes);
    else
        sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, ". - %o", (-displacement_bytes & 0x7f));
    // uc->disas_class = DISAS_CYCLE_CLASS_OPCODE ;
}

#if 0
// like "__single(), but build only effective address
unsigned Disassembler_Base::instr_jmp(
    InstructionDefinition * instruction_def,
    cycles_t *cycles,
    char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    single operand
    // |           opcode            |     dest spec   |
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

    unsigned dst_mode = (cycles_cur()->bus_data >> 3) & 7;
    unsigned dst_reg = (cycles_cur()->bus_data >> 0) & 7;

    unsigned err;

    // returns 0..2 disassembled cycles
    err = operand_cycles(operands_text, dst_mode, dst_reg, instruction_def->operand1_data_direction);
    // throws DisassemblyException
    return err; // disassembly mismatch?
}
#endif

/*************************************
 * jsr: like instr_operands_register_plus_operand(),
 * but no data access, only effective address
 * */

void Disassembler_Base::instr_jsr(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    register + operand
    // |        opcode      | src reg|     dest spec   |
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    UNREFERENCED_PARAMETER(instruction_def);

    unsigned reg = (cycles_cur()->bus_data >> 6) & 7;
    unsigned dst_mode = (cycles_cur()->bus_data >> 3) & 7;
    unsigned dst_reg = (cycles_cur()->bus_data >> 0) & 7;
    char	operands_dst_text[MAX_OPERAND_STRING_LEN];
    unsigned pc = cycles_cur()->bus_address.val + 2;

    operands_dst_text[0] = 0;

    // 1. load  dst
    instr_operand_cycles(operands_dst_text, &pc, dst_mode, dst_reg, DATACYCLES_NONE); // load only effective address
    // throws DisassemblyException

//	  cur_idx now on last cycle of fetch: opcode or argument
// operand like "R5,#4711"
    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s,%s", pdp11_regname[reg], operands_dst_text);

    cycles_step_forward_with_timeout();

    do_cyclecount_error(1, "jsr push");

    // 2. push reg to stack
    if (!datadirection_matches_buscontrol(DATACYCLES_WRITE, cycles_cur()->bus_cycle)) // verify data access direction
        do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "jsr push must be DATO cycle, does not match UNIBUS control signals");

    cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
    sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "push %s onto stack", pdp11_regname[reg]);

    // 3. now continue from new pc and new nesting level
    current_call_level++;
}




void Disassembler_Base::instr_rts(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    // |        000020                        |  reg   |
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    UNREFERENCED_PARAMETER(instruction_def);

    // pop 1x from stack. arg = reg
    unsigned reg = cycles_cur()->bus_data & 7;

    cycles_step_forward_with_timeout(); // skip opcode
    do_cyclecount_error(1, "rts");
    // pop reg from stack
    if (!datadirection_matches_buscontrol(DATACYCLES_READ, cycles_cur()->bus_cycle)) // verify data access direction
        do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "rts pop must be DATI cycle, does not match UNIBUS control signals");
    cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
    sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "pop %s from stack", pdp11_regname[reg]);

    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s", pdp11_regname[reg]);

    current_call_level--;
}


void Disassembler_Base::instr_rti(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);

    cycles_step_forward_with_timeout(); // skip opcode

    // no args, pop 2x from stack
    do_cyclecount_error(2, "rti");

    // 1. pop pc from stack
    if (!datadirection_matches_buscontrol(DATACYCLES_READ, cycles_cur()->bus_cycle)) // verify data access direction
        do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "rti pop must be DATI cycle, does not match UNIBUS control signals");
    cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
    sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "pop pc from stack");
    cycles_step_forward_with_timeout();

    // 2. pop psw from stack
    if (!datadirection_matches_buscontrol(DATACYCLES_READ, cycles_cur()->bus_cycle)) // verify data access direction
        do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "rti pop must be DATI cycle, does not match UNIBUS control signals");
    cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
    sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "pop psw from stack");

    current_call_level--;
}


void Disassembler_Base::instr_operands_register_plus_src(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    register + operand
    // |        opcode      | dst reg|     src  spec   |
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    UNREFERENCED_PARAMETER(instruction_def);

    unsigned reg = (cycles_cur()->bus_data >> 6) & 7;
    unsigned src_mode = (cycles_cur()->bus_data >> 3) & 7;
    unsigned src_reg = (cycles_cur()->bus_data >> 0) & 7;
    char	operands_src_text[MAX_OPERAND_STRING_LEN];
    unsigned pc = cycles_cur()->bus_address.val + 2;

    operands_src_text[0] = 0;

    do_cyclecount_error(1, "one more bus cycle expected");

    // src operand
    instr_operand_cycles(operands_src_text, &pc, src_mode, src_reg, DATACYCLES_READ);
    // throws DisassemblyException

    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s,%s", operands_src_text, pdp11_regname[reg]);

}


void Disassembler_Base::instr_operands_register_plus_dest(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    register + operand
    // |        opcode      | src reg|     dest spec   |
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    UNREFERENCED_PARAMETER(instruction_def);

    unsigned reg = (cycles_cur()->bus_data >> 6) & 7;
    unsigned dst_mode = (cycles_cur()->bus_data >> 3) & 7;
    unsigned dst_reg = (cycles_cur()->bus_data >> 0) & 7;
    char	operands_dst_text[MAX_OPERAND_STRING_LEN];
    unsigned pc = cycles_cur()->bus_address.val + 2;

    operands_dst_text[0] = 0;

    do_cyclecount_error(1, "one more bus cycle expected");

    // dst operand
    instr_operand_cycles(operands_dst_text, &pc, dst_mode, dst_reg, DATACYCLES_WRITE);
    // throws DisassemblyException

    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s,%s", pdp11_regname[reg], operands_dst_text);
}


void Disassembler_Base::instr_operands_number_6bit(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);

    unsigned n = cycles_cur()->bus_data & 0x3f; // mask 6 bits

    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%o", n);
}


void Disassembler_Base::instr_operands_double(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    double operand
    //  |  opcode   |   source spec   |     dest spec   |    010000:067777
    //  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    110000:167777
    unsigned src_mode = (cycles_cur()->bus_data >> 9) & 7;
    unsigned src_reg = (cycles_cur()->bus_data >> 6) & 7;
    unsigned dst_mode = (cycles_cur()->bus_data >> 3) & 7;
    unsigned dst_reg = (cycles_cur()->bus_data >> 0) & 7;
    //    char	src_operand_text[MAX_OPERAND_STRING_LEN], dst_operand_text[MAX_OPERAND_STRING_LEN];
    unsigned	pc = cycles_cur()->bus_address.val + 2;

    //    src_operand_text[0] = dst_operand_text[0] = 0;

        // one operand
        // returns 0..3 disassembled cycles
    instr_operand_cycles(operands_text, &pc, src_mode, src_reg, instruction_def->operand1_data_direction);
    //    instr_operand_cycles(src_operand_text, &pc_offset, src_mode, src_reg, instruction_def->operand1_data_direction);
        // throws DisassemblyException

            // returns another 0..3 disassembled cycles
    instr_operand_cycles(operands_text, &pc, dst_mode, dst_reg, instruction_def->operand2_data_direction);
    // throws DisassemblyException

// combine src and dst operand cycles, also in case of error
//    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s,%s", src_operand_text, dst_operand_text);
}


void Disassembler_Base::instr_sob(InstructionDefinition * instruction_def, char *operands_text)
{
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+ SOB
    // |      077           |   reg  |      offset     |
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    UNREFERENCED_PARAMETER(instruction_def);

    unsigned offset = cycles_cur()->bus_data & 0x3f; // 6bit offset
    unsigned reg = (cycles_cur()->bus_data >> 6) & 7;

    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s,.-%o", pdp11_regname[reg], 2 * offset - 2);
}


void Disassembler_Base::instr_trap(InstructionDefinition * instruction_def, char *operands_text)
{
    //
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+    EMT:  104000:104377
    // |       opcode          |       trap code       |    TRAP: 104400:104777
    // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    UNREFERENCED_PARAMETER(instruction_def);
    unsigned trapcode;

    // all traps: have 4 cycles, order depends on CPU. For example
    // 1. DATI read new PSW from <vector>+2
    // 2. DATO old PSW to @SP
    // 3. DATO old PC to @SP-2
    // 4. DATI read new PC from <vector>

    // lower 8 bits are trap/emt code
    trapcode = cycles_cur()->bus_data & 0xff; // 8bit
    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%o", trapcode);
    current_call_level++;
}

void Disassembler_Base::instr_fp__operands_zero(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);
    do_error(DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED, "floating point instruction not yet implemented");
}



void Disassembler_Base::instr_fp__operands_source(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);
    do_error(DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED, "floating point instruction not yet implemented");
}


void Disassembler_Base::instr_fp__operands_destination(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);
    do_error(DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED, "floating point instruction not yet implemented");
}


void Disassembler_Base::instr_fp__operands_fdst(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);
    do_error(DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED, "floating point instruction not yet implemented");
}


void Disassembler_Base::instr_fp__operands_ac_plus_fsrc(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);
    do_error(DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED, "floating point instruction not yet implemented");
}


void Disassembler_Base::instr_fp__operands_ac_plus_fdst(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);
    do_error(DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED, "floating point instruction not yet implemented");
}


void Disassembler_Base::instr_fp__operands_ac_plus_source(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);
    do_error(DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED, "floating point instruction not yet implemented");
}


void Disassembler_Base::instr_fp__operands_ac_plus_destination(InstructionDefinition * instruction_def, char *operands_text)
{
    UNREFERENCED_PARAMETER(instruction_def);
    UNREFERENCED_PARAMETER(operands_text);
    do_error(DISAS_ERROR_INSTRUCTION_NOT_IMPLEMENTED, "floating point instruction not yet implemented");
}




/*
    disassemble the instruction at unibus_cycles[cur_cycle_index]
    one opcode processing results in many bus cycles
    at least 1: the fetched opcode
    possible maximum 7: example
    add @1(R2),@2(r3),
    #2: fetch source displacement
    #3: fetch dest displacement
    #4: fetch source address ( from value in r2 + source displacement)
    #5: fetch value at source address
    #6: fetch destination address (from value in r3 + dest displacement)
    #7: store result to destination ADDRESS

    Input:
        a list of raw bus cycles
        with cycles->cur = opcode

    Result:
        disas_* field updated
        cur_cycle_index incremented
    belefor a given pdp-11 opcode, calculate the
    expected bus cycles on the unibus
    Output: a list of disassembled cycles

    If cycle stream ends in instructioN: instruction_incomplete = true
    (normal situtation as snippets are not synce with instructions)

*/

void Disassembler_Base::do_instruction()
{
    InstructionDefinition	*instruction_def;
    //unsigned processed_operand_cycles;
    //char operands_text[MAX_OPERAND_STRING_LEN];
    // start position of instruction
    int cycles_seq_start_idx;

    last_instruction_error_count = 0;
    last_error_code = DISAS_ERROR_OK;

    instruction_def = instr_opcode_fetch();
    assert(instruction_def); // else exception
    strcpy_s(cycles_cur()->disas_operands, MAX_OPERAND_STRING_LEN, ""); // filled by instruction disas methods
    strcpy_s(cycles_cur()->disas_address_info, MAX_INFO_STRING_LEN, "");
    // this comment is annoying: //strcpy_s(uc->disas_comment, MAX_INFO_STRING_LEN, "instruction fetch") ;
    strcpy_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "");
    cycles_cur()->disas_call_level = current_call_level; // may be refined later

    // 3. disassemble additional cycles for current instruction
    //assert(instruction_def-> fn_instruction_class);

    // 4. build disassembly for whole instruction (process all cycles)
    // execute the instruction analyzing function.
    // forward bus cycles after instruction to evaluation
    // result in instruction_cycles
    //processed_operand_cycles = 0; // defaults
    //operands_text[0] = 0;
    // moves cycles->cur
    cycles_seq_start_idx = cycles->cur_idx; // opcode

    InstructionDisassemblingFunction funcptr = instruction_def->disassemblingFunction;
    /*** for all instructions: cur_idx still pointing to opcode ***/
    try {
        // operands text to opcode cycle
//        (this->*funcptr)(instruction_def, operands_text);
        char *optxt = cycles_cur()->disas_operands;
        (this->*funcptr)(instruction_def, optxt);

    }
    // instruction may get terminated by timeout trap, thats ok.
    catch (DisassemblerTrapException const&) { //  warning: catching polymorphic type ‘class DisassemblerTrapException’ by value [-Wcatch-value=]
        dbg_msg("timeout trap");
    };

    // may throw DisassemblyException
    // add operand disassembly to static instruction opcode
    // (also in case of error ... give more hints)
//    strcpy_s(cycles->get(cycles_seq_start_idx)->disas_operands, MAX_OPERAND_STRING_LEN, operands_text);

    dbg_instruction_opcode(cycles, cycles_seq_start_idx);

    // add address info for physical address
    // get call_level from opcode
    int call_level = cycles->get(cycles_seq_start_idx)->disas_call_level;
    for (int next_idx = cycles_seq_start_idx + 1;
        // "seqstart+1": not for the opcode fetch
        next_idx <= cycles->cur_idx;
        next_idx++) {
        // loop for all cycles of current instruction (opcode+operands)
        pdp11bus_cycle_t *bc = cycles->get(next_idx);
        pdp11_symbol_t *sym;
        sym = pdp11SymbolTable.get(bc->bus_address);
        if (sym) {
            if (strlen(sym->group))
                // make comment for accessed address. suppress long "info" feld
                sprintf_s(bc->disas_address_info, MAX_INFO_STRING_LEN, "%s %s", sym->group, sym->text);
            else
                strcpy_s(bc->disas_address_info, MAX_INFO_STRING_LEN, sym->text);
        }
        // inherite call level from prev cycle of same opcode
        bc->disas_call_level = call_level;
    }
}



// all disassembly flags & results back to "nothing"
void Disassembler_Base::initialize(CycleList *disas_cycles) {
    this->cycles = disas_cycles;

    for (int i = 0; i < cycles->size(); i++) {
        pdp11bus_cycle_t *bc = cycles->get(i);
        bc->disas_call_level = 0;
        bc->disas_class = DISAS_CYCLE_CLASS_UNKNOWN;
    }
    last_instruction_error_count = 0;
    last_error_code = DISAS_ERROR_OK;
    last_error_text[0] = 0;
}


