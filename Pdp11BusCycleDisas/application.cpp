/*
  Global control of disassembly process.
  Uses instruction stream disassembler, dma logic, code snippets

  Problem: Cycle stream is not pure CPU isntruction processing, but also
  DMA equences and possble traceing errors.

  Base algorithm:
  Segment full cycle list into short "snippets",
  Disassmemble snippets in several speculative ways
  Final disassembly result is best one.

  Detect possible DMA sequences, mark as "dma candidates"
  start at first sample
  Divide cycle stream into short snippets.
  Disassemble snipptes with or without DMA candidades
  Choose best result, copy diassembly info back to original list.

*/

#include <assert.h>

#include "utils.hpp"

#include "instructions.hpp"
#include "parameters.hpp"
#include "cycles.hpp"
#include "symbols.hpp"

#include "snippet.hpp"
#include "dma.hpp"
#include "disas.hpp"
#include "disas_kd11b.hpp"
#include "disas_kd11e.hpp"
#include "disas_kdf11.hpp"
#include "disas_kd11fhlnpq.hpp"

#include "application.hpp"


DisasApplication::DisasApplication() {
    // prepare data structs
}


// disassemble next snippet from cur_idx, the best possible way
// advances the_cycles->cur
// call_level: global subroutine call_level, forwarded to disassmebler
//	and updated from selected snippet alterantive.
// result: false = end of all_cycles reached
bool DisasApplication::disas_next_snippet(CycleList *all_cycles, int snippet_id, int all_cycles_start_idx, int *all_cycles_next_idx, int *call_level) {
    dbg_msg("disas_next_snippet(all_cycles_start_idx=%d)\n", all_cycles_start_idx);
    // fill snippet1,2 with code including and excluding next DMA candidate
    int snippet_start_cycle_idx = all_cycles_start_idx;
    // snippet1: interpret dma candidate as code
    // snippet2: cut out DMA candidate from code
    // normally regular code contains many short DMA candidates, so check that first.
    Snippet *best_snippet = NULL;
    double error_ratio1, error_ratio2;
    snippet1->id = snippet_id; // mark for debug
    //dbg_break(snippet_id == 25);
    //dbg_break(*all_cycles_next_idx == 1096916);
    snippet1->fetch(snippet_start_cycle_idx, /*with_dma*/true);
    snippet1->disas_initial_call_level = *call_level;
    snippet1->disas(disassembler);
    /* dma candidates are excluded if
    - the DMA transfer is from addresses near the last code fetch
    - disassembly as code is error free
     */
     // DMA may dump a block of 00s, which are disassembled as HALTs,
     // Add penalty errorcount for code which is likely DMA (multi-HALT)
    snippet1->disas_add_code_penalty();

    error_ratio1 = snippet1->disas_error_ratio();
    if (error_ratio1 == 0) {
        best_snippet = snippet1;
        //    } else if (dma_candidate && dma_candidate_is_code(dma_candidate->id)) {
        //        best_snippet = snippet1;
    }
    else {
        // better result if DMA candidates excluded from disassembly?
        snippet2->id = snippet_id; // mark for debug
        snippet2->fetch(snippet_start_cycle_idx, /*with_dma*/false);
        if (snippet2->size() == 0) {
            // we're totally in a DMA candidate, entry via snippet1 in loop before
            snippet2->disas_end_cycle_idx = -1; // not disassed
            if (snippet1->disas_errorcount == 0)
                best_snippet = snippet1;
            else
                best_snippet = snippet2; // all DMA
        }
        else {
            snippet2->disas_initial_call_level = *call_level;
            snippet2->disas(disassembler);
            // Add penalty errorcount for DMA which is likely code
            // snippet2->add_dma_penalty();
            error_ratio2 = snippet2->disas_error_ratio();
            // if same result with DMA: choose for "code"
            if ( // code may be get shortened, increasing ratio: no penalty
                snippet1->disas_errorcount == snippet2->disas_errorcount
                // different error count: weight by snippet length
                || error_ratio1 <= error_ratio2)
                best_snippet = snippet1;
            else
                best_snippet = snippet2;
        }
    }
    dma_register_code_range(best_snippet);

    best_snippet->merge_back();

    // end_cycle_idx in all_cycles
    if (best_snippet->disas_end_cycle_idx < 0) {
        // nothing completed, first cycle in snippet invalid
        // accept this as disasresult, try again with 2nd cycle
        *all_cycles_next_idx = all_cycles_start_idx + 1;
    }
    else {
        // update global call_level
        *call_level = best_snippet->disas_current_call_level;
        assert(best_snippet->disas_end_cycle_idx < (int)best_snippet->size());
        *all_cycles_next_idx = best_snippet->all_cycles_idx[best_snippet->disas_end_cycle_idx] + 1;
    }
    if (*all_cycles_next_idx >= all_cycles->size())
        // next cycle after snippet end is not in list anymore
        return false;

    // read pointer in cycle list must advance, else endless loop
    assert(*all_cycles_next_idx > all_cycles_start_idx);

    //    if (snippet_start_cycle_idx < best_snippet->disas_end_cycle_idx) {
    //        // no progress, nothing more to disassemble:
    //        // end of all_cycles !
    //        return false;
    //    } 
        // what if DMA candidate follows DMA candidate ?
    return true;
}


/*
Normalize Call levels
mark skipped DMA candidates as "DMA"
*/
void DisasApplication::fixup() {
    int i;

    CycleList *cycles = &Cycles;

    /*** mark skipped DMA cycle properly ***/
    // Skipped DMA cycles are still marked with DISAS_CYCLE_CLASS_UNKNOWN
    // and have a disas_dma_candidate_id > 0
    // 
    unsigned dma_cur_candidate_id = 0;
    int dma_cur_block_no = 0; // linear number of DMA block
    int dma_cur_cycle_no = 0; // linear number of word within current DMA block
    for (i = 0; i < cycles->size(); i++) {
        pdp11bus_cycle_t *bc = cycles->get(i);
        if (bc->disas_class == DISAS_CYCLE_CLASS_UNKNOWN && bc->disas_dma_candidate_id > 0) {
            if (dma_cur_candidate_id != bc->disas_dma_candidate_id) {
                // new DMA block
                dma_cur_block_no++; // count from 1
                dma_cur_cycle_no = 0;
                dma_cur_candidate_id = bc->disas_dma_candidate_id;
            }
            sprintf_s(bc->disas_comment, MAX_INFO_STRING_LEN, "DMA 0%o.0%o =  0x%x.0x%x",
                dma_cur_block_no, dma_cur_cycle_no, dma_cur_block_no, dma_cur_cycle_no);
            dma_cur_cycle_no++; // count from 0
            // cleanup strings
            bc->disas_address_info[0] = 0;
            bc->disas_opcode[0] = 0;
            bc->disas_operands[0] = 0;
            bc->disas_sym_label[0] = 0;
        }
    }


    /*** normalize call levels ***
    * lowest mapped to 0
    * (code sequence can start in subroutine, so rts before first jsr(trap/intr
    */
    int min_call_level = 0xffffff; // start with max, get smaller
    // 1. get smallest call level in use
    for (i = 0; i < cycles->size(); i++) {
        pdp11bus_cycle_t *bc = cycles->get(i);
        if (min_call_level > bc->disas_call_level)
            min_call_level = bc->disas_call_level;
    }
    // 2. normalize it to 0
    for (i = 0; i < cycles->size(); i++) {
        pdp11bus_cycle_t *bc = cycles->get(i);
        bc->disas_call_level -= min_call_level;
    }
}





// disassemble all cycles 
void DisasApplication::disas_all(CycleList *all_cycles) {
    unsigned snippet_count = 0; // counts use of snippet loop

    instruction_table_init();

    switch (param_cpu) {
    case DISAS_CPU_34:
        disassembler = new Disassembler_KD11E();
        disassembler->instruction_sets = IS_BASIC | IS_40 | IS_EIS | IS_MMU | IS_PSW;
        break;
    case DISAS_CPU_34FPP:
        disassembler = new Disassembler_KD11E();
        disassembler->instruction_sets = IS_BASIC | IS_40 | IS_EIS | IS_MMU | IS_PSW | IS_FPP ;
        break;
    case DISAS_CPU_05:
        disassembler = new Disassembler_KD11B();
        disassembler->instruction_sets = IS_BASIC ;
        break;
    case DISAS_CPU_KDF11:
        disassembler = new Disassembler_KDF11();
		// first guess: like 11/34, ... TODO!
        disassembler->instruction_sets = IS_BASIC | IS_40 | IS_EIS | IS_MMU | IS_PSW;
        break;
    case DISAS_CPU_KD11FHLNPQ:
        disassembler = new Disassembler_KD11FHLNPQ();
		// first guess: like 11/34, ... TODO!
        disassembler->instruction_sets = IS_BASIC ; // todo: upgrade for variants
        break;
    default:
        // legacy default
        disassembler = new Disassembler_KD11E();

    }

    snippet1 = new Snippet(all_cycles, "1", MAX_SNIPPET_SIZE);
    snippet2 = new Snippet(all_cycles, "2", MAX_SNIPPET_SIZE);

    // clear global cycle list
    // repeated for each snippet
    disassembler->initialize(all_cycles);

    dma_find_candidates(all_cycles);

    int cur_cycle_idx = 0;
    int	disas_call_level = 0;
    bool ready = false;
    while (!ready) {
        // cur_cycle_idx is moved forward for each snippet
        ready = !disas_next_snippet(all_cycles, snippet_count++, cur_cycle_idx, &cur_cycle_idx, &disas_call_level);
        //if (cur_cycle_idx > 1100000) ready = true;
    }

    fixup();

    delete snippet1;
    delete snippet2;
    delete disassembler;
}


//extern "C" 
void dll_application_disas(pdp11bus_cycle_t *pdp11bus_cycles, unsigned pdp11bus_cycles_count, 
	pdp11_symbol_t *addrsymbols, unsigned addrsymbols_count) {

    DisasApplication application;
    // link samples to main object
    application.Cycles.pdp11bus_cycles = pdp11bus_cycles;
    application.Cycles.capacity = pdp11bus_cycles_count;
    application.Cycles.pdp11bus_cycles_count = pdp11bus_cycles_count;

    pdp11SymbolTable.fill(addrsymbols, addrsymbols_count) ;

    application.disas_all(&application.Cycles);

}
