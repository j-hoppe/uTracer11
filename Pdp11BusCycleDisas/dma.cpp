/*
    Recognition of DMA sequences

    Device DMA interrupts code execution with DATI/DATO cycle sequences.
    These must be marked and excluded from disassembly.

    Without Bus GRANT/ACK signals, separation from CPU cycles is difficult.
    Worst scenario: am executable program file is saved to disc.
    then lot of DATI cycles with valid opcodes are on the UNIBUS visible.
    Difference to execution:
    - only linear ascending addresses
    - a minimum block size ... 16? for serial lines/DMZ, 128 for block devices?
    - no operand access cycles, only opcode and operand fetch.
    - PC content can be guessed from CPU cycles.
      DMA is to/from different addresses
      In general: the next "expected" PC value can be guessed
        and should be used as address, if cycle is opcode fetch.
      If last instruction was branch, two potential "next PC" values can be compared
      JMP, JSR, RTS, RTI are difficult to handle.
      TRAPs can be recognized by target address.
*/

#include <stdlib.h>
#include <assert.h>
#include "utils.hpp"
#include "dma.hpp"    // own defs

// LA trace size: 2M cycles, how many DMA transfer fit into that? 
#define DMA_MAX_CANDIDATE_COUNT     500000		// for 2M file

// ignore potential DMA sequences shorter than limit
// must be 2 for MSCP
int param_dma_min_blocksize = 2;

// index = id, dma_sequences[0] not used
dma_sequence_t dma_sequences[DMA_MAX_CANDIDATE_COUNT];

unsigned dma_code_position;


/* find cycle sequences which maybe DMA tramsfers*/
// recognize the next DMA sequence after dma_start_idx
// advances cycle index "dma_end_idx" until >= cycle count
void dma_find_candidates(CycleList *disas_cycles)
{
    int block_start_idx;
    int block_end_idx;
    int idx;
    unsigned candidate_id ; // linear number of DMA block identified

    // clear all markers
    for (idx = 0; idx < DMA_MAX_CANDIDATE_COUNT; idx++) {
        dma_sequence_t *dmaseq = &dma_sequences[idx];
        dmaseq->id = 0;
    }

    for (idx = 0; idx < disas_cycles->size(); idx++) {
        pdp11bus_cycle_t *cycle = disas_cycles->get(idx);
        cycle->disas_dma_candidate_id = 0;
    }

    block_start_idx = 0;
    candidate_id = 1; // dma_sequences[0] not used
    while (block_start_idx < disas_cycles->size()) {
        // for current block
        bool block_end = false;
        block_end_idx = block_start_idx + 1; // next cycle behind current block
        pdp11bus_cycle_t *start_cycle = disas_cycles->get(block_start_idx);
        if (start_cycle->bus_cycle != BUSCYCLE_DATI && start_cycle->bus_cycle != BUSCYCLE_DATO)
            block_end = true; // only 16bit accesses allowed 
        while (!block_end) {
            block_end = true;
            if (block_end_idx >= disas_cycles->size())
                continue;  // end of list: block end
            pdp11bus_cycle_t *cycle = disas_cycles->get(block_end_idx);
            if (start_cycle->bus_cycle != cycle->bus_cycle)
                continue; // DATI/DATO change: block end
            unsigned expected_addr = start_cycle->bus_address.val + 2 * (block_end_idx - block_start_idx);
            if (cycle->bus_address.val != expected_addr)
                continue; // addrs not ascending: block end
            block_end = false; // this cycle is part of the block, try next
            block_end_idx++;
        }
        // block_end_idx now on first word AFTER block
        if ((block_end_idx - block_start_idx) < param_dma_min_blocksize) {
            // block too small: skip
            block_start_idx = block_end_idx;
            continue;
        }

        // mark block as DMA candidate
        assert(candidate_id < DMA_MAX_CANDIDATE_COUNT);
        dma_sequence_t *dmaseq = &dma_sequences[candidate_id];
        dmaseq->id = candidate_id ; // first entry [0] not used
        dmaseq->start_cycle_idx = block_start_idx;
        dmaseq->end_cycle_idx = block_end_idx - 1;
        assert(disas_cycles->is_idx_valid(dmaseq->start_cycle_idx));
        assert(disas_cycles->is_idx_valid(dmaseq->end_cycle_idx));
        dmaseq->start_address = start_cycle->bus_address.val;
        dmaseq->end_address = start_cycle->bus_address.val + 2 * (block_end_idx - block_start_idx);
        dmaseq->bus_control = start_cycle->bus_cycle;

        // mark cycles in DMA block as "candidate"
        for (idx = block_start_idx; idx < block_end_idx; idx++) {
            pdp11bus_cycle_t *cycle = disas_cycles->get(idx);
            cycle->disas_dma_candidate_id = candidate_id;
            // comment used for debugging
            sprintf_s(cycle->disas_comment, MAX_INFO_STRING_LEN, "DMA candidate: id=%u", candidate_id);
        }
        // try next block
        candidate_id++;
        block_start_idx = block_end_idx;
    }
}



// find next DMA candidate sequences behind "start_idx"
// NULL if none
dma_sequence_t *dma_next_candidate(int start_cycle_idx) {
    // scan list of DMA candidate sequences
    for (int id = 1; id < DMA_MAX_CANDIDATE_COUNT; id++) {
        dma_sequence_t *dmaseq = &dma_sequences[id];
        if (dmaseq->id == 0)
            return NULL; // end of list reached
        // stop on first DMA sequence behind "start_idx"
        if (dmaseq->start_cycle_idx >= start_cycle_idx)
            return dmaseq;
    }
    return NULL; // not reached
}


/* Tell the DMA logic about previous code fetches.
 DMA transfers will most likely occur to different addresses,
 so this info is used to exclude DMA candidates.
"disas_cycles" is the cycle list of a recently disassembled snippet.
*/
void dma_register_code_range(CycleList	*disas_cycles) {
    // as minimal implementation, just save the last code fetch address
    for (int idx = 0; idx < disas_cycles->size(); idx++) {
        pdp11bus_cycle_t *cycle = disas_cycles->get(idx);
        if (cycle->disas_class == DISAS_CYCLE_CLASS_OPCODE) {
            dma_code_position = cycle->bus_address.val;
        }
    }
}

/* Many DMA candidates are in fact code fetches with ascending addresses
    Eliminate them fast!
*/
bool dma_candidate_is_code(unsigned dma_candidate_id) {
    if (dma_code_position == 0xffffffff)
        return false; // not known

    assert(dma_candidate_id < DMA_MAX_CANDIDATE_COUNT);
    dma_sequence_t *dmaseq = &dma_sequences[dma_candidate_id];
    assert(dmaseq->id != 0);
    if (dmaseq->bus_control != BUSCYCLE_DATI)
        return false; //  code fetches always with DATI
    if (abs((int)dmaseq->start_address - (int)dma_code_position) < DMA_MIN_CODE_DISTANCE)
        return true;
    if (abs((int)dmaseq->end_address - (int)dma_code_position) < DMA_MIN_CODE_DISTANCE)
        return true;
    return false;
}



void dma_init(void) {
    dma_code_position = 0xffffffff;
}
