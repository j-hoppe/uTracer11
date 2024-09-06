#ifndef _DMA_H_

#include "cycles.hpp"

// DMa blocks must be separate from code fetch addresses
//#define DMA_MIN_CODE_DISTANCE 256	// 128 words
#define DMA_MIN_CODE_DISTANCE 64	// RSX11M boot MSCP

// describes a (possible) DMA seqence in cycle list
typedef struct {
    unsigned id; // 0 = invalid
    int start_cycle_idx; // first cycle in global cycle list
    int end_cycle_idx; // last first cycle in global cycle list
    unsigned start_address; // UNIBUS address range
    unsigned end_address;
    unsigned bus_control; // BUSCYCLE_DAT*
} dma_sequence_t;

extern int param_dma_min_blocksize;

// dma_sequences[0] not used
extern dma_sequence_t dma_sequences[];

void dma_all(CycleList	*disas_cycles);

void dma_find_candidates(CycleList	*disas_cycles);

dma_sequence_t *dma_next_candidate(int start_cycle_idx);

void dma_register_code_range(CycleList	*disas_cycles);
bool dma_candidate_is_code(unsigned dma_candidate_id);


void dma_init(void);



#endif



