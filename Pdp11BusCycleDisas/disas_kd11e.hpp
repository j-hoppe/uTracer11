#ifndef _DISAS_KD11E_H_
#define _DISAS_KD11E_H_


#include "disas.hpp"


// PDP-11/34 CPU "KD11-E"
// implementation of CPU specific cycle patterns
class Disassembler_KD11E : public Disassembler_Base {
public:

    Disassembler_KD11E();
    virtual ~Disassembler_KD11E() override {}

};


#endif
