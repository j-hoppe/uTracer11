
/*
    Disassembly function for LSI-11/03  CPU variants, based on WD Chipset
*/
#include "utils.hpp" // UNREFERENCED_PARAMETER
#include "disas_kd11fhlnpq.hpp"


Disassembler_KD11FHLNPQ::Disassembler_KD11FHLNPQ() : Disassembler_Base() {
    param_tolerate_datip_before_dato = true;
    param_tolerate_dati_before_dato = true;

    // access order of cycles for trap processing
    trapCycle = { fetchNewPC,  fetchNewPSW, pushOldPSW, pushOldPC };

	
	// mark CPU-internal addresses not visible on QBUS
	// cpuInternalAddr[0177776] = true ; // PSW

}


