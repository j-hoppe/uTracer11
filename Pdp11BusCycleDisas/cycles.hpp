
#ifndef _CYCLES_H_
#define _CYCLES_H_

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "disas_pdp11.hpp"



// type of bus cycle as calculated by disassembler
#define DISAS_CYCLE_CLASS_UNKNOWN	0
#define DISAS_CYCLE_CLASS_OPCODE	1	// opcode fetch by cpu
#define DISAS_CYCLE_CLASS_OPERAND	2	// instruction data processing by cpu
#define DISAS_CYCLE_CLASS_VECTOR	3	// fetch of PC or PSW
#define DISAS_CYCLE_CLASS_ERROR	4	// non-cpu traffic
#define DISAS_CYCLE_CLASS_DMA	    5	// non-cpu traffic, recognized as DMA
#define DISAS_CYCLE_CLASS_END		0xff	// cycle list ends here

// cycles initiated by CPU code execution?
#define DISAS_CYCLE_CLASS_IS_CPU(c) ((c) == DISAS_CYCLE_CLASS_OPCODE || (c) == DISAS_CYCLE_CLASS_OPERAND || (c) == DISAS_CYCLE_CLASS_VECTOR )

#define	MAX_SYM_LEN	16  // size of symbols (MACRO-11: 6)
#define	MAX_OPCODE_STRING_LEN	32
#define	MAX_OPERAND_STRING_LEN	32
#define	MAX_INFO_STRING_LEN		128

// Central struct: one sample of bus signals,
// as part of a cycle stream on unibus,
// Processed in several stages:
// 1. raw signal sample by the Logic Analyzer.
// 2. extraction of UNIBUs cycles
// 3. adding of disassembly information
// Exported by DLL interface , therefore byte-aligned
#pragma pack(push, 1)

    // numeric value of address, use as key for symbol search
typedef struct {
    unsigned val; // 16,18,22 bit value
    unsigned iopage; // 0/1: address is in physical iopage
    // base = 160000 / 760000 / 17760000
    // only lower 13 bits of "val" significant then.
} pdp11_address_val_t;


typedef struct pdp11bus_cycle_struct {
    unsigned	id; // index in parent list 0..
    // with "holes" for redcued cycle lists

    // from logic analyzer
    unsigned 	tag; //	sample numbers from LA
    int64_t		timestamp; // start of signal in nano seconds or SSYN clocks, not used

    // physical signals on the bus
    unsigned	bus_timeout; // 1, if no SSYN whithin 20 µs (CPU dependend)

    // a typical logic analyzer sample contains 22 valid bits for Iopage addresses
    // and just 13 (or so) bits or so for memory adresses
//    unsigned	bus_address_iopage; // 0/1: Address in range of PDP-11 I/O page?
    pdp11_address_val_t bus_address ;
//    unsigned	bus_address; // numeric value
    unsigned	bus_address_lsb_bit_count; // count of valid LSB bits. 0 = not truncated
    unsigned	bus_data; // data lines 0..15
    unsigned	bus_cycle; // logical, for disassembler: NONE,DATI,DATIP,DATO,DATOB
    unsigned	bus_cycle_physical ; // QBUS: is DATIO,DATBI,DATBO, INVALID

    // filled by disassembler
    unsigned    disas_dma_candidate_id; // disas private: maybe a DMA sequence
    unsigned disas_class; // DISAS_CYCLE_CLASS*
    // the three columns of an assembler listing
    char disas_sym_label[MAX_SYM_LEN];	// optional: symbolic label of opcode fetch: "MYPROC:"
    char disas_opcode[MAX_OPCODE_STRING_LEN];	// if opcode fetch: "mov", "add"
    char disas_operands[MAX_OPERAND_STRING_LEN]; // if opcode fetch  "@#<addr1>,r2"
    char disas_comment[MAX_INFO_STRING_LEN];	// info: "value of addr 1"
    char disas_address_info[MAX_INFO_STRING_LEN];	// address: "Serial DL11.XBUF"
    char disas_debug_info[MAX_INFO_STRING_LEN];	// opt. diag from disassembler to GUI
    int   disas_call_level; // subroutine/trap/intr level
} pdp11bus_cycle_t;



#pragma pack(pop)

#ifndef C_INTERFACE_ONLY

/*** CycleList: central object for disassembler
Snippet is then class :: Cycles
***/
class CycleList {
public:
    pdp11bus_cycle_t *pdp11bus_cycles;	// array of sampled cycles
    int 	capacity;   // size of pdp11bus_cycles array
    int	pdp11bus_cycles_count; // this much valid cycles

    // access a valid cycle
    pdp11bus_cycle_t *get(int idx) {
        assert(idx >= 0 && idx < pdp11bus_cycles_count);
        return &pdp11bus_cycles[idx];
    }
    // on valid cycle?
    bool	is_idx_valid(int idx) {
        return (idx >= 0 && idx < pdp11bus_cycles_count);
    }

    // cycle scanning for disassembler
    int	cur_idx;
    pdp11bus_cycle_t *cur(void) {
        assert(cur_idx >= 0 && cur_idx < pdp11bus_cycles_count);
        return &pdp11bus_cycles[cur_idx];
    }

    // cur_idx on valid cycle?
    bool	is_cur_valid(void) {
        return (cur_idx >= 0 && cur_idx < pdp11bus_cycles_count);
    }

    // # of cycles
    int		size(void) {
        return pdp11bus_cycles_count;
    }

    int	cycle_read(FILE *f, pdp11bus_cycle_t *uc);

    void clear(int max_pdp11bus_cycle_count);

    void	cycle_set_endmark(int idx);
    int		cycle_is_endmark(int idx);

    void dump(FILE *f, bool with_disas_fields);
};

char	*pdp11bus_addr_astext(unsigned addr, unsigned lsb_bits);

#endif // C_INTERFACE_ONLY

#endif
