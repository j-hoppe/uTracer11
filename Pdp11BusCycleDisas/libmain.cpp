// libmain.cpp : entry points if called as static library
// Und das interface

//#include <windows.h>
//#define C_INTERFACE_ONLY
#include "libmain.h"
// limit header file only to "struct" definitions
#include "cycles.hpp"
#include "symbols.hpp"
#include "application.hpp"
#include "parameters.hpp"



/* main function: gets a list of bus cycles,
 * disassembles cycles
 * and fills in 'disas_*' fields.
 * gets list of symbols (address-name pairs)
 */
void disas_pdp11buscycles(
    pdp11bus_cycle_t *pdp11bus_cycles,
    unsigned	pdp11bus_cycles_count,
    pdp11_symbol_t *addrsymbols,
    unsigned addrsymbols_count
)
{

    // link C-structs to C++ application object
    dll_application_disas(pdp11bus_cycles, pdp11bus_cycles_count, addrsymbols, addrsymbols_count);

}

// hide "parameter_t", only numericals settable
int disas_param_set(unsigned _param, unsigned _num) {
    parameter_t p;
    p.param = _param;
    p.num = _num;
    return dll_param_set(&p);
}

/*
int disas_param_get(parameter_t *param) {
    return dll_param_get(param);
}
*/
