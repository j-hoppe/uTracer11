
/* variables - uint data which can be accessed by name and compared to changes

- MessagetStateVars
- other entities used for break conditions
	Variable MPC
	Variable: UnibusAddress
	Variable: UnibusCycle
	Variable: UnibusData


*/
#ifndef  __VARIABLES_H__
#define  __VARIABLES_H__


#if !defined(PLATFORM_ARDUINO)
// M32X2 probe Arduino does not report variable lists,
// special KY11/KM11 I/O has to be interpreted by GUI.

#include <string>
#include <stdint.h>


// needs variable directory
class Variable {
public:
    //std::string moduleName ; // later namespace : CPU, KM11A, DL11, disk, tape, ...
    std::string name ; // ResponseStateDef: short name, like "R0,PC,BA".
    int		bitCount = 0 ; // ResponseStateDef: size in bits, > 1
    // int base ; // 8, 10, 16, not used

    // endpoint: data source in producing emulator, displaying control in GUI
    void	*endpoint = nullptr  ;
    int	endpointSizeof=0 ; // size of pointer in byte: uint8=1, uint16=2, uint32=4
    // endpointSizeof only needed intern in emulator, neither rendered nor parsed
    uint32_t value = 0 ; // RequestStateVal
    uint32_t prevValue = 0 ; // for hasChanged()
    Variable() {
        bitCount = 0 ;
        endpoint = nullptr ;
        endpointSizeof = 0;
        value = 0;
        prevValue = 0 ;
    }
    void setValue(uint32_t newValue) {
        prevValue = value ;
        value = newValue ;
    }

    // last setValue() had differing value?
    bool hasChanged() {
        return (value != prevValue);
    }
} ;


#include <map>
#include <cassert>
#include "utils.h"

// registers variables, access by name, hold pointers
// variable names case insensitive
class VariableMap      {
private:
	std::map<std::string,Variable*> m ; // compositionn instead of inheritance
public:
    void add(Variable *variable) {
        std::string key = uppercase(variable->name) ;
        bool found = (m.find(key) != m.end()) ;
        assert(!found) ;
        m[key] = variable ;
    }

    Variable *get(std::string &name) {
        std::string key = uppercase(name) ;
        auto it = m.find(key) ;
        if (it == m.end())
            return nullptr ;
        else
            return it->second ;
    }
} ;

#endif // PLATFORM_ARDUINO
#endif // __VARIABLES_H__
