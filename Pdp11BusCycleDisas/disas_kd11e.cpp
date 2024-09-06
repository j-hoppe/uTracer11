
/*
    Disassembly function for PDP-11/34 CPU "KD11E"
*/
#include "utils.hpp" // UNREFERENCED_PARAMETER
#include "disas_kd11e.hpp"


Disassembler_KD11E::Disassembler_KD11E() : Disassembler_Base() {
    param_tolerate_datip_before_dato = true;

    // access order of cycles for trap processing
    trapCycle = { fetchNewPSW, pushOldPSW, pushOldPC, fetchNewPC };

	
	// mark CPU-internal addresses not visible on QBUS
	// cpuInternalAddr[0177776] = true ; // PSW

}



