#ifndef _DISAS_KDF11_H_
#define _DISAS_KDF11_H_


#include "disas.hpp"


// PDP-11/23 CPU "KDF11", based on FONZ Chipset
// implementation of CPU specific cycle patterns
class Disassembler_KDF11 : public Disassembler_Base {
public:

    Disassembler_KDF11();
    virtual ~Disassembler_KDF11() override {}
};


#endif
