
/* variables - uint32 data which can be accessed by name and compared to changes
*/
#ifndef  __VARIABLES_H__
#define  __VARIABLES_H__


#if !defined(PLATFORM_ARDUINO)
// M32X2 probe Arduino does not report variable lists,
// special KY11/KM11 I/O has to be interpreted by GUI.

#include <string>
#include <stdint.h>
#include <map>
#include <vector>
#include <cassert>
#include "utils.h"


// ORable type flags
enum class VariableType {
    none = 0,
    // data source
    basic = 0x01, // base functionalities: MPC, UNIBUS addr/cycle/data
    signal = 0x02, // signal from M93X2 probe
    reg = 0x04, // queried via ResponseRegDef
    // usage in GUI
    trace = 0x08 // show in trace protocol
} ;

// bitmasks work
inline VariableType operator|(VariableType a, VariableType b) {
    return static_cast<VariableType>(static_cast<int>(a) | static_cast<int>(b));
}

inline VariableType operator&(VariableType a, VariableType b) {
    return static_cast<VariableType>(static_cast<int>(a) & static_cast<int>(b));
}


class Variable {
public:
    //std::string moduleName ; // later namespace : CPU, KM11A, DL11, disk, tape, ...
    std::string name ; // ResponseStateDef: short name, like "R0,PC,BA".
    std::string info ; // long description
    VariableType type ;
    int		bitCount = 0 ; // ResponseStateDef: size in bits, > 1
    // int base ; // 8, 10, 16, not used

    // endpoint: data source in producing emulator, displaying control in GUI
    void	*endpoint = nullptr  ;
    int	endpointSizeof=0 ; // size of pointer in byte: uint8=1, uint16=2, uint32=4
    // endpointSizeof only needed intern in emulator, neither rendered nor parsed
    uint32_t value = 0 ; // RequestStateVal
    unsigned	index ; // linear position, insertion order in VariableMap.v[]

    Variable() {}

    Variable(std::string _name, std::string _info, int _bitcount, VariableType _type) {
        name = _name ;
        info = _info ;
        bitCount = _bitcount ;
        type = _type;
        endpoint = nullptr ;
        endpointSizeof = 0;
        index = 0 ; // set by map.add()
        value = 0;
    }

    void setValue(uint32_t newValue) {
        value = newValue ;
    }

    // typeflag set?
    bool isType(VariableType typeFlag) {
        if ((type & typeFlag) != (VariableType)0)
            return true ;
        else return false ;
    }


    // return value formatted as octal with correct bit count
    std::string valueText(uint32_t _value) {
        int digitCount = (bitCount+2) / 3 ; // 1..3->1, 4..6->2, ...
        //std::string result = format_string("%06o", (unsigned)value) ;
        std::string result = format_string("%0*o", digitCount, _value) ;
        return result ;
    }

    std::string valueText() {
        return valueText(value) ;
    }

   
} ;

// registers variables, access by name, hold pointers
// variable names case insensitive
class VariableMap      {
private:
    // store and access variable by name
    std::map<std::string,Variable> m ;
    // access by order of add()ing
    std::vector<Variable *> v ;

public:

    void clear() {
        v.clear() ;
        m.clear() ;
    }

    size_t size() {
        assert(m.size() == v.size()) ;
        return m.size() ;
    }


    void add(Variable variable) {
        //std::string key = uppercase(variable.name) ;
        std::string key = variable.name ;
        bool found = (m.find(variable.name) != m.end()) ;
        assert(!found) ;
        variable.index = m.size() ; // add pos in v[]
        m[key] = variable ; // store and link with name
        v.push_back(&m[key]) ; // keep insertion order. ('&variable' points to temp val!)
        assert(m.size() == v.size()) ;
    }

    bool exists(std::string name) {
        auto it = m.find(name) ;
        return (it != m.end()) ;
    }

    // return pointer to named var
    Variable *get(std::string name) {
        //std::string key = uppercase(name) ;
        auto it = m.find(name) ;
        bool found = (it != m.end()) ;
        assert(found) ;
        return &(it->second) ;
    }

    // return pointer to var by insertion order
    Variable *get(unsigned index) {
        assert(index < v.size()) ;
        return v[index] ;
    }

   
} ;

#endif // PLATFORM_ARDUINO
#endif // __VARIABLES_H__
