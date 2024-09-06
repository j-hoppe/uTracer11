#ifndef _DISAS_KD11FLPHNQ_H_
#define _DISAS_KD11FLPHNQ_H_

#include "disas.hpp"


// LSI-11/03 on M7264 (quad) and M7220 (dual) have lot of variants
// but use the same WD chip set
// -F, -L, -P, -H, -N, -Q
class Disassembler_KD11FHLNPQ : public Disassembler_Base {
private:

public:

    Disassembler_KD11FHLNPQ();

};


#endif
