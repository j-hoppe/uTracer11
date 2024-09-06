/* memoryimage.cpp - reading and saving several memory file formats

 Copyright (c) 2017, Joerg Hoppe, j_hoppe@t-online.de, www.retrocmp.com

 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 - Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 - Neither the name of the copyright holder nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 12-nov-2018  JH      entered beta phase
 18-Jun-2017  JH      created

 */

#ifndef _MEMORYIMAGE_HPP_
#define _MEMORYIMAGE_HPP_

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <map>
#include <string>

 /*** Address map ***/
class codelabel_map_c : public std::map<std::string, unsigned> {
public:
	// clear()
	void add(std::string label, unsigned address) {
		insert(std::make_pair(label, address));
	}

	bool is_defined(std::string label) {
		return count(label) > 0;
	}

	unsigned get_address(std::string label) {
		return at(label);
	}

	void relocate(int delta);

	void print(FILE* f);

};

typedef enum {
	fileformat_none = 0,
	fileformat_addr_value_text = 1,
	fileformat_macro11_listing = 2,
	fileformat_papertape = 3,
	fileformat_binary = 4
} memory_fileformat_t;

#define MEMORY_ADDRESS_INVALID	0x7fffffff

#define MEMORY_WORD_COUNT	0x20000 // 18 bit addresses = 256KB = 128KWords
#define MEMORY_END_ADDRESS  0x40000 // first invalid, 256KB
#define MEMORY_IOPAGE_SIZE  0x4000 // 8KB
#define MEMORY_IOPAGE_ADDRESS (MEMORY_END_ADDRESS-MEMORY_IOPAGE_SIZE)
#define MEMORY_DATA_MASK	0xffff	// lower 16bits are valid

// a memory location on UNIBUS
typedef struct {
	bool	valid; // has cell been queried?
	bool nxm;	// if known: NXM on that address?
	uint16_t	data; // data value for this memory location
} memory_cell_t;

class memoryimage_c {
public:
	void put_word(unsigned addr, uint16_t w) {
		memory_cell_t* cell = get_cell(addr);
		cell->data = w;
		cell->valid = true;
		cell->nxm = false;
	}

	void put_nxm(unsigned addr) {
		memory_cell_t* cell = get_cell(addr);
		cell->data = 0;
		cell->valid = true;
		cell->nxm = true;
	}

	// if w is a PC relative address, marked in listings with '
	// 165124 105767  165004'		   T4:	tstb	data1			; test a byte, if we get here...
	// => 165126 165004'
	// => 165126 165004 - pc
	// => 165126 165004 - (165126 + 2)
	// => 165126 177654
	uint16_t pc_relative_relocation(unsigned addr, uint16_t w) {
		w = (w - (addr + 2)) & 0xffff;
		return w;
	}

	void put_byte(unsigned addr, unsigned b);

	void assert_address(unsigned addr) {
		assert(addr / 2 < MEMORY_WORD_COUNT);
	}

public:

	// word idx IS NOT the addr, addr = idx + 2
	memory_cell_t	cells[MEMORY_WORD_COUNT];// array with data words

	memoryimage_c() {
	}

	void init(void);

	memory_cell_t* get_cell(unsigned addr) {
		assert_address(addr);
		unsigned wordidx = addr / 2;
		return &cells[wordidx];
	}

	// void fill(uint16_t fillword);

	void get_addr_range(unsigned* first, unsigned* last);
	unsigned get_word_count(void);
	void set_addr_range(unsigned first, unsigned last);

	bool load_addr_value_text(const char* fname);

	bool load_macro11_listing(const char* fname, codelabel_map_c* codelabels);

	bool load_papertape(const char* fname, codelabel_map_c* codelabels);

	bool load_binary(const char* fname);
	void save_binary(const char* fname, unsigned bytecount);

	void info(FILE* f);
	void dump(FILE* f);

};

#endif /* MEMORY_H_ */
