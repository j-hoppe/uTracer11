
/*
    Disassembly function for PDP-11/23 CPU "KDF11", based on FONZ Chipset
    
*/
#include "utils.hpp" // UNREFERENCED_PARAMETER
#include "disas_kdf11.hpp"


Disassembler_KDF11::Disassembler_KDF11() : Disassembler_Base() {
    param_tolerate_dati_before_dato = true;

    // access order of cycles for trap processing
    trapCycle = { fetchNewPC,  fetchNewPSW, pushOldPSW, pushOldPC };

	// mark CPU-internal addresses not visible on QBUS
	cpuInternalAddr[0177776] = true ; // PSW

}


