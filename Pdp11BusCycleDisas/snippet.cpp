/*
Code snippet, which is extracted from cyyclelist by removing
DMA candidates or errors.
Can be disassembled.
Technically a list of cycles.



*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utils.hpp"
#include "cycles.hpp"
#include "dma.hpp"
#include "disas.hpp"
#include "snippet.hpp"


// Allocate snippet 
Snippet::Snippet(CycleList *all_cycles, const char *dbg_name, int cycle_count) {
    strcpy_s(this->dbg_name, sizeof(this->dbg_name), dbg_name);
    this->all_cycles = all_cycles;
    // allocate space for all, even if DMA cycles are ignored
    this->capacity = cycle_count;
    this->pdp11bus_cycles = (pdp11bus_cycle_t *)malloc(cycle_count * sizeof(pdp11bus_cycle_t));
    // crossreference
    this->all_cycles_idx = (int *)malloc(cycle_count * sizeof(int));
}

Snippet::~Snippet()
{
    free(pdp11bus_cycles);
    free(all_cycles_idx);
}

// copy subset of buscycles into snippet
// Central to generates code candidates
// 'with_dma' false: exclude DMA candidates
void Snippet::fetch(int cycle_start_idx, bool with_dma) {

    int snippet_write_idx = 0;
    int all_cycles_read_idx = cycle_start_idx;
    pdp11bus_cycle_t *all_cycles_cycle = NULL;
    pdp11bus_cycles_count = 0;

    // fill snippet with potential code cycles, depending on algorithm:
    // skip or include all dma candidates
    while (snippet_write_idx < SNIPPET_MIN_CODE_PREFETCH_SIZE
        && all_cycles_read_idx < all_cycles->size()) {
        assert(snippet_write_idx < capacity);
        all_cycles_cycle = all_cycles->get(all_cycles_read_idx);
        if (all_cycles_cycle->disas_dma_candidate_id == 0
            || with_dma
            || dma_candidate_is_code(all_cycles_cycle->disas_dma_candidate_id)) {
            pdp11bus_cycles[snippet_write_idx] = *all_cycles_cycle;
            all_cycles_idx[snippet_write_idx] = all_cycles_read_idx;
            snippet_write_idx++;
            pdp11bus_cycles_count = snippet_write_idx;
        }
        all_cycles_read_idx++;
    }
    /* when trying to disassemble DMA candidate, try the WHOLE DMA block.
     Else false success on first words may lead to a false code execution point
     via dma_candidate_is_code().
     Following DMA words would then rated as "code" because of neighborhood to
     false "successful" opcodes.
     */
    if (pdp11bus_cycles_count == 0)
        return;

    // snippet_write_idx : last fetched
    // all_cycles_read_idx, all_cycles_cycle: next to check
    snippet_write_idx--;
    unsigned this_dma_candidate_id = pdp11bus_cycles[snippet_write_idx].disas_dma_candidate_id;
    if (with_dma && snippet_write_idx >= 0
        // snippet ends with DMA?
        && this_dma_candidate_id != 0) {
        bool ready;
        do {
            // fetch cycles until end of list or end of DMA
            ready = (all_cycles_read_idx >= all_cycles->size());
            if (ready)
                continue;
            all_cycles_cycle = all_cycles->get(all_cycles_read_idx);
//            ready = (all_cycles_cycle->disas_dma_candidate_id != this_dma_candidate_id); 
            ready = (all_cycles_cycle->disas_dma_candidate_id == 0);
            if (ready) // end of this dma block
                continue;
            // fetch to next
            snippet_write_idx++;
            assert(snippet_write_idx < capacity);
            pdp11bus_cycles_count = snippet_write_idx + 1;
            // copy cur
            pdp11bus_cycles[snippet_write_idx] = *all_cycles_cycle;
            all_cycles_idx[snippet_write_idx] = all_cycles_read_idx;
            // check next
            all_cycles_read_idx++;
        } while (!ready);
    }
}





// generate code for cycles in snippet.
// sets "disas_end_cycle_idx for end of last good instruction
// returns quality indicator:
// ratio of total disassembled cycles / error count
// global subroutine "call_level" is traced
// calller must initilaze "initial_call_level"
void Snippet::disas(Disassembler_Base *disassembler) {
    bool err;

    dbg_msg("snippet_disas(%s)\n", dbg_name);
    disas_errorcount = 0; // error sum of all complete instructions

    disas_abort = 0;
    disassembler->current_call_level = disas_current_call_level = disas_initial_call_level;

    // linkt to cycles and clear them
    disassembler->initialize(this);

    // start disassembly at beginn of snippet
    disas_end_cycle_idx = -1;
    cur_idx = 0; // start at 1st unibus cycle
    while (!disas_abort && is_cur_valid() && disassembler->last_error_code != DISAS_ERROR_CYCLECOUNT) {
        dbg_msg("snippet_disas(%s) id=%d, all_start_idx = %d, snippet_cur_idx = %d\n",
            dbg_name, id, all_cycles_idx[0], cur_idx);

        /**** fetch of a trap vector ? ***/
        int tmp_start_idx = cur_idx;

        // todo: remove last_error_code, last_error_text, work with expection 
        err = true;
//dbg_break(id == 0 && cur_idx == 2);
        try {
            disassembler->current_call_level = disas_current_call_level;
            disassembler->do_trap();
            err = false;
        }
        catch (DisassemblerMismatchException& ex) {
            UNREFERENCED_PARAMETER(ex);
        }
        // if not enough cycles for vector test: ERR_CYCLECOUNT

        if (disassembler->last_error_code != DISAS_ERROR_CYCLECOUNT && err) {
            /**** execution of an instruction ? ***/
            cur_idx = tmp_start_idx; // no vector, reparse for instruction
            try {
                //dbg_break(all_cycles_idx[0] == 7826);
//dbg_break(id == 23 && cur_idx == 0);
                disassembler->current_call_level = disas_current_call_level; // unchanged from do_trap()
                disassembler->do_instruction();
                err = false;
            }
            catch (DisassemblerMismatchException& ex) {
                UNREFERENCED_PARAMETER(ex);
            }
        }
        // cur_idx now on last cycle of trap vector/instruction execution
        assert(!err || disassembler->last_instruction_error_count > 0);

        // !err: disassembled, cycles->cur now at error position or last operand

        if (disassembler->last_error_code == DISAS_ERROR_CYCLECOUNT) {
            // end of cycle stream hit
            // 'cur' is at last valid cycle, possibly inside operands
            // premature end of a cycle is not an error !
            // (normal situtation as snippets are not synced with instructions)
            // disas_end_cycle_idx is not updated and points to incomplete instruction,
            // which is reparsed at beginning of next snippet
        }
        else if (err) {
            // instruction parser stopped at error cycle
            disas_errorcount += disassembler->last_instruction_error_count;
            cur()->disas_class = DISAS_CYCLE_CLASS_ERROR;
            sprintf_s(cur()->disas_comment, MAX_INFO_STRING_LEN, "Error: %.80s", disassembler->last_error_text); // trunc text
            disas_end_cycle_idx = cur_idx; // erroneous, but complete
            assert(is_cur_valid());
        }
        else {
            // good cpu instruction: 
            disas_errorcount += disassembler->last_instruction_error_count;
            // update pointer to last completely processed instruction
            int disas_start_idx = disas_end_cycle_idx + 1;

            // if this is violated, one of the instruction disassembler parsed beyond end of snippet
            disas_end_cycle_idx = cur_idx;
            disas_current_call_level = disassembler->current_call_level; // from trap() or isntruction()

            dbg_msg("snippet_disas(%s) complete instr idx = %d - %d\n", dbg_name, disas_start_idx, disas_end_cycle_idx);
            // cycle->dma_candidate_id remains set
            assert(is_cur_valid());
        }
        cur_idx++;
    }
    if (disas_end_cycle_idx < 0)
        dbg_msg("snippet_disas(%s) disas_end_cycle_idx = %d\n", dbg_name, disas_end_cycle_idx);
    // debugging: truncate list
    if (disas_abort && is_cur_valid()) {
        cycle_set_endmark(cur_idx);
    }

}

// add error points for disassmebled code, which is probably NO code
// (multiple HALTs, == 0,0,0,....
void Snippet::disas_add_code_penalty() {
    // add one error for each 2-HALT-chain
    // 
    int idx = 1;
    while (idx < disas_end_cycle_idx) {
        if (
            get(idx - 1)->disas_class == DISAS_CYCLE_CLASS_OPCODE
            && get(idx - 1)->bus_data == 0
            && get(idx)->disas_class == DISAS_CYCLE_CLASS_OPCODE
            && get(idx)->bus_data == 0
            ) {
            disas_errorcount++;
            idx += 2;
        }
        else
            idx++;
    }
}

// Add error points for skipped cycles, which are probably 
// code isnted of DMA: address in range of program counter
// void Snippet::disas_add_DMA_penalty() {
// }


// return errors per snippet length (disas_end_cycle_idx)
double Snippet::disas_error_ratio() {
    if (disas_errorcount == 0)
        return 0; // all successful
    else if (disas_end_cycle_idx == -1) // no error free disassemblies
        return 1; // very high
    else
        return (double)disas_errorcount / (disas_end_cycle_idx + 1);
}


// Write disas results back into main cycle list "TheCycles"
// 
// Skipped DMA cycles are insert in disas_fixup() later.
void Snippet::merge_back() {
    for (int i = 0; i < size(); i++) {
        // transform snipped index into list opf all cycles
        int all_cycles_cycle_idx = all_cycles_idx[i];
        pdp11bus_cycle_t *all_cycle = all_cycles->get(all_cycles_cycle_idx);
        *all_cycle = *get(i);
        //dbg
#if _DEBUG        
        sprintf_s(all_cycle->disas_debug_info, MAX_INFO_STRING_LEN, "snippet%s.idx = %d.%d", dbg_name, id, i);
#endif
    }
}


