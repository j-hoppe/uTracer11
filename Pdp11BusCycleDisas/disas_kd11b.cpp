
/*
    Disassembly function for PDP-11/05 CPU "KD11-B"
*/

#include "utils.hpp" // UNREFERENCED_PARAMETER
#include "disas_kd11b.hpp"



Disassembler_KD11B::Disassembler_KD11B() : Disassembler_Base() {
    param_tolerate_datip_before_dato = true;
    param_tolerate_dati_before_dato = true;

    // access order of cycles for trap processing
    trapCycle = { pushOldPSW, pushOldPC, fetchNewPC, fetchNewPSW };

	
	// mark CPU-internal addresses not visible on QBUS
	// cpuInternalAddr[0177776] = true ; // PSW

}


/*************************************
 * pdp 11/05 has several extra unibus accesses.
 * skip them
 * result: false, if end of cycle list reached
 */
bool Disassembler_KD11B::ignore_pdp1105_extras(const char *info)
{
    cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
    if (info && strlen(info))
        sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "ignored 11/05 access: %s", info);
    else
        sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "ignored 11/05 access");
    cycles->cur_idx++;

    return cycles->is_cur_valid();
}


/* fetch opcode and decode
  instructionDefinition: result
*/
InstructionDefinition * Disassembler_KD11B::instr_opcode_fetch() {
    InstructionDefinition *instructionDefinition;

    // handle different classes of opcode
    assert(cycles->is_cur_valid());

    // 1. fetch current instruction from unibus data stream
    // and disassemble into cycle list
// dbg_break(cycles_cur()->bus_address == 0406250);
    instructionDefinition = decodeInstruction(cycles_cur()->bus_data);
    if (instructionDefinition == NULL)
        do_error(DISAS_ERROR_INSTRUCTION_INVALID, "invalid instruction");

    // instr belongs to one of valid instruction sets ?
    if ((instructionDefinition->instruction_set & instruction_sets) == 0)
        do_error(DISAS_ERROR_INSTRUCTION_INVALID, "invalid instruction");

    // 2. now generated disassembled instruction_cycles[0]
    do_cycletype_error(BUSCYCLE_DATI, "Opcode fetch: DATI cycle expected");

    // PDP-11/05 may fetch again, if "difficult" opcode
    // mark the 1st as "ignored"
    int next_idx = cycles->cur_idx + 1;
    if (cycles->is_idx_valid(next_idx)) {
        pdp11bus_cycle_t *bc = cycles->get(next_idx);
        if (cycles_cur()->bus_address.val == bc->bus_address.val
            && cycles_cur()->bus_cycle == bc->bus_cycle
            && cycles_cur()->bus_data == bc->bus_data) {
            if (!ignore_pdp1105_extras("repeated opcode fetch"))
                do_error(DISAS_ERROR_CYCLECOUNT, "repeated opcode fetch");
            cycles->cur_idx = next_idx;
        }
    }

    // instruction_cycles[0] = opcode fetch cycle: refined by instruction analyzing function
    cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPCODE;
    strcpy_s(cycles_cur()->disas_sym_label, MAX_SYM_LEN, symbol_label(cycles_cur()->bus_address));
    strcpy_s(cycles_cur()->disas_opcode, MAX_OPCODE_STRING_LEN, instructionDefinition->name);

    return instructionDefinition;
}




// jsr: like instr_operands_register_plus_operand(),
// but no data access, only effective address

void Disassembler_KD11B::instr_jsr(InstructionDefinition * instruction_def, char *operands_text)
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
    // throws DisassemblyException
instr_operand_cycles(operands_dst_text, &pc, dst_mode, dst_reg, DATACYCLES_NONE); // load only effective address

//	  cur_idx now on last cycle of fetch: opcode or argument
// operand like "R5,#4711"
    sprintf_s(operands_text, MAX_OPERAND_STRING_LEN, "%s,%s", pdp11_regname[reg], operands_dst_text);

    cycles_step_forward_with_timeout();

    do_cyclecount_error(1, "jsr push");

    // may reach end of cycle list
    if (!datadirection_matches_buscontrol(DATACYCLES_WRITE, cycles_cur()->bus_cycle)) {
        // 11/05 has this always?
        if (!ignore_pdp1105_extras("jsr push"))
            do_error(DISAS_ERROR_CYCLECOUNT, "opcode fetch before jsr push");
    }

    // 2. push reg to stack
    if (!datadirection_matches_buscontrol(DATACYCLES_WRITE, cycles_cur()->bus_cycle)) // verify data access direction
        do_error(DISAS_ERROR_MISMATCH_CYCLETYPE, "jsr push must be DATO cycle, does not match UNIBUS control signals");

    cycles_cur()->disas_class = DISAS_CYCLE_CLASS_OPERAND;
    sprintf_s(cycles_cur()->disas_comment, MAX_INFO_STRING_LEN, "push %s onto stack", pdp11_regname[reg]);

    // 3. now continue from new pc and new nesting level
    current_call_level++;
}



