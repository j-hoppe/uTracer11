
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include "utils.hpp"
#include "disas_pdp11.hpp"

/********************************************************
 * register names
 */
const char *pdp11_regname[8] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "sp", "pc"
};



unsigned pdp11_buscontrol_txt2val(char *txt)
{
    if (!strcasecmp("DATI", txt))
        return BUSCYCLE_DATI; // c1,c0
    else if (!strcasecmp("DATIP", txt))
        return  BUSCYCLE_DATIP; // c1,c0
    else if (!strcasecmp("DATO", txt))
        return  BUSCYCLE_DATO; // c1,c0
    else if (!strcasecmp("DATOB", txt))
        return  BUSCYCLE_DATOB; // c1,c0
    else
        return  BUSCYCLE_INVALID;
}

const char *pdp11_buscontrol_val2txt(unsigned val)
{
    switch (val) {
    case BUSCYCLE_DATI:
        return "DATI";
    case BUSCYCLE_DATIP:
        return "DATIP";
    case BUSCYCLE_DATO:
        return "DATO";
    case BUSCYCLE_DATOB:
        return "DATOB";
    default:
        return "???";
    }
}


