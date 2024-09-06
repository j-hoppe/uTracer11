#ifndef _DISAS_PDP11_H_
#define _DISAS_PDP11_H_

/*** type of BUS DATA cycle: UNIBUS C1,C0 signal ***/

// disassembler works only with "logical" buscycle types
#define BUSCYCLE_NONE	0	// not to be disassembled
#define BUSCYCLE_DATI	1	//  One word of data from slave to master
#define BUSCYCLE_DATIP 2  // Same as DATI, but inhibits restore cycle in
//              destructive read-out devices. Must be followed
//				by DATO or DATOB to the same location.
#define BUSCYCLE_DATO	3	// One word of data from master to slave.
#define BUSCYCLE_DATOB	4 // One byte of data from master to slave.
// additional for buscycle_physical (QBUS)
#define BUSCYCLE_DATIO_I 5 // DATI part of of DATIO
#define BUSCYCLE_DATIO_O 6 // DATO part of of DATIO
#define BUSCYCLE_DATIOB_I 7 // DATI part of of DATIOB
#define BUSCYCLE_DATIOB_O 8 // DATO part of of DATIOB
#define BUSCYCLE_DATBI 9 // DATI block
#define BUSCYCLE_DATBO 10 // DATO block

#define BUSCYCLE_REFRESH	0x$f0
#define BUSCYCLE_INVALID 0xf1 // signal mismatch
#define BUSCYCLE_MULTI 	0xf2  // temporary flag to mark DATIO/block before fixup
#define BUSCYCLE_FILLUP 0xf3 // added after end of list, when disas needs complete multi-cycle opcode

/*** data types ***/
typedef unsigned char byte;

typedef unsigned short int word;

typedef struct {
    unsigned long int f;
    unsigned long int m;
} dfword;

typedef struct {
    word par;
    word pdr;
} pagedesc;

typedef struct {
    word lsb;
    word msb;
} ubamap;

extern const char *pdp11_regname[8] ;

unsigned pdp11_buscontrol_txt2val(char *txt)  ;

const char *pdp11_buscontrol_val2txt(unsigned val) ;

#endif // _DISAS_PDP11_H_
