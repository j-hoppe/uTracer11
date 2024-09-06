
#include <stdio.h>
#include <stdarg.h>

#include "utils.hpp"

#include "cycles.hpp"

void dbg_msg(const char *fmt, ...) {
#if _DEBUG // Visual Studio
    static char buffer[1024];
    va_list args; // that again ...
    va_start(args, fmt);
    vsprintf_s(buffer, sizeof(buffer), fmt, args);
    OutputDebugStringA(buffer);
#else
    UNREFERENCED_PARAMETER(fmt);
#endif
}


void dbg_break(bool condition) {
#if _DEBUG // Visual Studio
    if (condition)        
         dbg_msg("dbg_break\n");
#else
    UNREFERENCED_PARAMETER(condition);
#endif
}

void dbg_instruction_opcode(CycleList *disas_cycles, int opcode_idx) {
    struct pdp11bus_cycle_struct *opcode_cycle = disas_cycles->get(opcode_idx);
    dbg_msg("Opcode:snippet idx = %d, addr=%06o opcode=%s %s ; %s\n",
        opcode_idx, opcode_cycle->bus_address.val, opcode_cycle->disas_opcode,
        opcode_cycle->disas_operands, opcode_cycle->disas_comment);
}

