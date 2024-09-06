#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_



#ifndef C_INTERFACE_ONLY

#include "cycles.hpp"
#include "snippet.hpp"
//#include "disas_kd11e.hpp"

class DisasApplication {
private:
    // alternative disassembly with or without DMA candidate sequence
    // Optimization globally allocated with max size
    Snippet *snippet1, *snippet2;

    bool disas_next_snippet(CycleList *all_cycles, int snippet_id, int all_cycles_start_idx, int *all_cycles_next_idx, int *call_level);

    void fixup(void);


public:
    DisasApplication();
    // global objects 
    CycleList	Cycles; // cycles as input by caller

    Disassembler_Base *disassembler;

    void disas_all(CycleList *all_cycles);
};

#endif // C_INTERFACE_ONLY

//#ifdef __cplusplus
//extern "C"
//#endif
//extern "C" 
void dll_application_disas(pdp11bus_cycle_t* pdp11bus_cycles, unsigned pdp11bus_cycles_count, pdp11_symbol_t *addrsymbols, unsigned addrsymbols_count);


#endif
