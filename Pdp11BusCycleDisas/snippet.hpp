#ifndef _SNIPPET_H_
#define _SNIPPET_H_

#include "cycles.hpp"

/* size of longest instruction
 opcode = 1
  + defered source fetch = 2
  + deferred destiantion fetch = 2
  + data read = 1
  + data write = 1
  + bus timeout trap 4
  = 11
*/

#define SNIPPET_MIN_CODE_PREFETCH_SIZE	12

#define MAX_SNIPPET_SIZE 0x10000+ SNIPPET_MIN_CODE_PREFETCH_SIZE


// cycle sequence form main cycle list
class Disassembler_Base;

class Snippet : public CycleList {
private:

public:
    Snippet(CycleList *all_cycles, const char *dbg_name, int cycle_count);
    ~Snippet();

    int id;
    char dbg_name[40];

    CycleList    *all_cycles; // link to full list where this is a snippet from
    // crossreference: all_cycles_idx[i] = index in all_cycles
    int *all_cycles_idx;

    // after disas: pointer to last cycle of last complete one.
    int  disas_end_cycle_idx; // -1: nothing completed

    // subroutine call level tracing for first and alst cycle of disassembly
    int disas_initial_call_level;
    int disas_current_call_level;


    void fetch(int cycle_start_idx, bool with_dma);

    // generate code for cycles in snippet.
    // sets "last_instruction_cycle_idx for end of last good instruction
    // returns quality indicator
    // ration total disassembeld cycles / error count
    void disas(Disassembler_Base *disassembler);

    void disas_add_code_penalty(void);

    int disas_errorcount; // errors in disassembled code

    double disas_error_ratio();

    // Write disas results back into main cycle list "TheCycles"
    void merge_back(void);

};


#endif
