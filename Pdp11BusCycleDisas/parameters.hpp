
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "cycles.hpp"

// #include "dllmain.h"


// strings and numerical parameters allowed

// Exported by DLL interface , therefore byte-aligned
#pragma pack(push, 1)
#ifdef _MSC_VER
#pragma warning(disable : 4201)
#endif
typedef struct {
    unsigned param; // PARAM_*
    union {
        char	str[MAX_INFO_STRING_LEN];
        unsigned	num;
    } ;
} parameter_t;

#pragma pack(pop)

/*
Delphi:
{$A-}
type parameter_t = packed record

    case param: UInt32 of
    PARAM_VERSION:
    (
        str: array[0..MAX_INFO_STRING_LEN-1] of AnsiChar ;
    ) ;
    PARAM_DATIP_BEFORE_DATO:
        num: (UInt32 );
    end ;
*/

// PARAM_S_: string
// PARAM_N_: unsigned number
// PARAM_B_: boolean as int 32
#define PARAM_S_VERSION	1	// string: 

// ignore extra DATI before DATO: limitations of micro code for CLR
// #define PARAM_B_TOLERATE_DATIP_BEFORE_DATO	2	// bool,

// don't check correct UNIBUS control cycle type (DATI, DATO, ...)
#define PARAM_B_IGNORE_UNIBUS_CONTROL	3	// bool, 

// which CPU? use one of DISAS_CPU_* 
#define PARAM_N_CPU	4

// ignore potential DMA sequences shorter than this limit
// must be 2 for MSCP
#define PARAM_N_MIN_DMA_BLOCKSIZE 5


#ifdef __cplusplus
//extern "C"
#endif
int dll_param_set(parameter_t *param);
#ifdef __cplusplus
//extern "C"
#endif
int dll_param_get(parameter_t *param);

#ifdef __cplusplus
//extern "C"
#endif
void params_init(void );


#ifndef C_INTERFACE_ONLY


void params_init(void);
#endif // C_INTERFACE_ONLY

#endif // !_PARAMETERS_H_
