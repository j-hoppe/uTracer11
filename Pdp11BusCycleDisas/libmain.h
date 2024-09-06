

// interface

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the PDP11DISASDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// PDP11DISASDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef PDP11BUSCYCLEDISAS_DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API		// used in test app
//#define DLL_API __declspec(dllimport)
#endif

#include "symbols.hpp"
#include "cycles.hpp"
//#include "disas.h"


// Callable and exported
void disas_pdp11buscycles(
    pdp11bus_cycle_t *pdp11bus_cycles,
    unsigned	pdp11bus_cycles_count,
    pdp11_symbol_t *addrsymbols,
    unsigned addrsymbols_count
);

// reduced, no getter
int disas_param_set(unsigned param, unsigned num);
