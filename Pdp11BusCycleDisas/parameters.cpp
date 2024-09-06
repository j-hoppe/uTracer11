
/*
    List of parameters, wich control behaviour of disassembler.
    */
#include <string.h>

#include "parameters.hpp"

#include "version.h"
#include "disas.hpp"
#include "dma.hpp"

    /*
     *  param_set()
     *  save a parameter.
     + called from DLL "C" environment
     *	result: 1 = ok, else 0
     */
//extern "C"
int dll_param_set(parameter_t *param)
{
    switch (param->param) {
    case PARAM_S_VERSION:
        return 0; // read only
    case  PARAM_B_IGNORE_UNIBUS_CONTROL:
        param_ignore_unibus_control = !!param->num;
        return 1;
    case PARAM_N_CPU:
        param_cpu = param->num;
        return 1;
    case PARAM_N_MIN_DMA_BLOCKSIZE:
        param_dma_min_blocksize = param->num;
        return 1;
    default:
        return 0;
    }
}

/*
 *  param_get()
 * get the value (str or num) for the 'param'.
 + called from DLL "C" environment
 */
//extern "C"
int dll_param_get(parameter_t *param)
{
    switch (param->param) {
    case PARAM_S_VERSION:
        strcpy_s(param->str, sizeof(param->str), VERSION_STR);
        return 0; // read only
    case  PARAM_B_IGNORE_UNIBUS_CONTROL:
        param->num = param_ignore_unibus_control;
        return 1;
    case PARAM_N_CPU:
        param->num = param_cpu;
        return 1;
    default:
        return 0; // illegal param
    }
}



//extern "C"
void	params_init(void)
{
    param_ignore_unibus_control = 0;
    param_cpu = DISAS_CPU_34; // PDP-11/34
    param_dma_min_blocksize = 2;
}
