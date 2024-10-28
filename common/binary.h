/* binary - helper to process binary data
*/

#ifndef __BINARY_H__
#define __BINARY_H__

#include <string>
#include <stdint.h>


// helper: extract the bit sub field at value[bitFrom...BitTo]
uint64_t getBitSubField(uint64_t value, unsigned bitFrom, unsigned bitTo)  ;
bool getBit(uint64_t value, unsigned bitIdx) ;


// a string of "0"s and "1"s
class BinaryString {
public:
    unsigned bitCount ; // # of bits
    std::string text ;
    bool msbFirst = true ;
    uint64_t value ;

    // create from string, derive value
    BinaryString(std::string _text, bool _msbFirst): text(_text), msbFirst(_msbFirst) {
        bitCount = _text.length() ;
        value = getValueFromString() ;
    }

// create from value, derive string
    BinaryString(uint64_t _value, unsigned _bitCount, bool _msbFirst):  value(_value), bitCount(_bitCount), msbFirst(_msbFirst)  {
        text = getStringFromValue() ;
    }

    // member string is converted to a value, member value untouched
    // "0001 1100" => 0x1d (msbFirst) or 0x38 (!msbFirst)
    uint64_t getValueFromString() ;

    // member value is converted to a string, member text untouched
    // length of string = bitCount
    std::string getStringFromValue() ;
} ;

#endif // __BINARY_H__
