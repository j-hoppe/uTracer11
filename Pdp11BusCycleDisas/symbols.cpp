
#include <stdlib.h>
#include <assert.h>

#include "utils.hpp"
#include "symbols.hpp"
#include "cycles.hpp"



// only one table: Singleton
Pdp11SymbolTable pdp11SymbolTable;


/* PDp11AddressKey:
 * address value and iopage flag made sortable
 */
Pdp11AddressKey::Pdp11AddressKey(pdp11_address_val_t addr) {
	if (addr.iopage)
		addr.val &= 0x1fff ; // only ower 13 bits relevant
	this->addr = addr ;
}

bool Pdp11AddressKey::operator<(const Pdp11AddressKey & other) const {
	if (this->addr.iopage != other.addr.iopage)
		return (this->addr.iopage < other.addr.iopage) ;
	else if (this->addr.val < other.addr.val)
			return true ;
	else return false ;
	}


// register all usable symbol into the hash map,
// key = address
// iopage addresses are held and search via lower 13 bits and iopage-flag
// list NULL terminated, ignore count for now
void  Pdp11SymbolTable::fill(pdp11_symbol_t *addrsymbols, unsigned addrsymbols_count) {
    if (addrsymbols == nullptr)
        return;
    for (unsigned idx = 0; idx < addrsymbols_count ; idx++) {
            pdp11_symbol_t *sym = &(addrsymbols[idx]) ;
        // MACRO11 assignments ("name = value") are suppressed,
        // these may mean anything, not just addresses.
        // only symbols found in binary output are valid
        if (sym->addrtype !=ADDRTYPE_ASSIGNED ) {
            if (sym->addrval.iopage)
                sym->addrval.val &= 0x1fff;
			Pdp11AddressKey addrkey(sym->addrval) ; // convert addr,iopage to hash key
            insert(std::pair<Pdp11AddressKey,pdp11_symbol_t>(addrkey, *sym) ) ;
        }
    }
}

/* search for symbol by address 
 if IOpage address: search by lower 13 bits + iopage flag,
    so tolerant to 16/18/22 bit addresses. */
pdp11_symbol_t *Pdp11SymbolTable::get(pdp11_address_val_t addr) {
    std::map<Pdp11AddressKey,pdp11_symbol_t>::iterator it;
	Pdp11AddressKey addrkey(addr) ; // convert addr,iopage to hash key
    it = find(addrkey) ;
    if (it == end())
        return NULL ;
    else return &(it->second) ;
}
/*

// return symbol info for a 16 bit address
pdp11_symbol_t *addrsymbol_get(unsigned addr)
{
    pdp11_symbol_t *sym;
    assert(addr <= MAX_ADDRESS); // 18 bit!
    sym = &(symbol_table->addrsymbols[addr]);
    if (sym->addrtype == ADDRTYPE_INVALID
        || sym->addrtype == ADDRTYPE_ASSIGNED)
        return NULL;
    else return sym;
}
*/

