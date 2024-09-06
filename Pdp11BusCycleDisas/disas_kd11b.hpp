#ifndef _DISAS_KD11B_H_
#define _DISAS_KD11B_H_

#include "disas.hpp"


// PDP-11/05 CPU "KD11-B"
// implemenetation of CPU specific cycle patterns
// limited microcode so some extra fetches
class Disassembler_KD11B : public Disassembler_Base {
private:
    bool ignore_pdp1105_extras(const char *info);

public:

    virtual InstructionDefinition *instr_opcode_fetch(void);
    virtual void instr_jsr(InstructionDefinition * instruction_def, char *operands_text);

    Disassembler_KD11B();
    virtual ~Disassembler_KD11B() override {}


};


#endif
