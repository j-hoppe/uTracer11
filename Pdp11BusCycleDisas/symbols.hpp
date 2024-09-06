#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include "disas.hpp"
#include "cycles.hpp"


// what type is the special address?
//#define	ADDRTYPE_NULL		0x00	// list terminator
#define	ADDRTYPE_ASSIGNED	0x01	// MACRO11 assignment: symbol = value
#define	ADDRTYPE_SYMBOL	    0x02	// label for code or data position
#define	ADDRTYPE_VECTOR_PC	0x03	// PC of a vector
#define	ADDRTYPE_VECTOR_PSW	0x04	// PSW of a vector
#define	ADDRTYPE_DEVICE_REG 0x05	// register of a device

#define	ADDRATTR_GLOBAL 0x01	// MACRO11 listing: "G" 

/* list of symbol records, as input into disassembler */

typedef struct {
    pdp11_address_val_t addrval ;
    unsigned addrtype;  // ADDRTYPE_*
    unsigned attribute; // bit mask of ADDRATTR_*
    char	group[MAX_SYM_LEN]; // "trap", "serial", "cpu", "rl11"
    char	text[MAX_SYM_LEN]; // registername: "emt", "power on", "csr", "r5"
    char	info[MAX_INFO_STRING_LEN]; // explanation
} pdp11_symbol_t;



#ifndef C_INTERFACE_ONLY

#include <map>

// sortable pair of address and iopage flag
class Pdp11AddressKey {
public:
	pdp11_address_val_t addr ;
	Pdp11AddressKey(pdp11_address_val_t addr) ;
	bool operator<(const Pdp11AddressKey &other) const ;
};

/* Hash map for symbols, key is pdp11_address_val_t record */
class Pdp11SymbolTable: public std::map<Pdp11AddressKey,pdp11_symbol_t> {
public:
	// load NULL-terminated record list into has map
	void fill(pdp11_symbol_t *addrsymbols, unsigned addrsymbols_count) ;

	// get a symbol record by code address
	pdp11_symbol_t *get(pdp11_address_val_t addr) ;
} ;

extern Pdp11SymbolTable pdp11SymbolTable;

/*

void symbols_clear(void);
void symbols_add(pdp11_symbol_t *symlist, unsigned symlist_count);

void symbols_prepare(void);
pdp11_symbol_t *addrsymbol_get(unsigned addr);
*/

#endif // C_INTERFACE_ONLY

#endif
