/* binary - helper to process binary data
*/

#ifndef __BINARY_H__
#define __BINARY_H__

#include <string>
#include <stdint.h>

// a string of "0"s and "1"s
class BinaryString {
public:
    unsigned bitCount ; // # of bits
    std::string text ;
    bool msbFirst = true ;
    uint64_t value ;

    // helper: extract the bit usbfieled at value[bitFrom...BitTo)]
    static uint64_t getBitSubField(uint64_t value, unsigned bitFrom, unsigned bitTo) {
        uint64_t result = 0 ;
        unsigned i, bitIdx;
        for (i=0, bitIdx = bitFrom ; bitIdx <= bitTo ; bitIdx++, i++)
            if (value & ((uint64_t)1 << bitIdx))
                result |= ((uint64_t)1 << i) ;
        return result ;
    }


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
    uint64_t getValueFromString() {
        uint64_t result = 0 ;
        unsigned i = 0 ; // indexes bits in value
        for (char& c : text) {
            if (c != '0')
                if (msbFirst)
                    result |= ( (uint64_t)1 << (bitCount-i-1)) ;
                else result |= ( (uint64_t)1 << i) ;
            i++ ;
        }
        return result ;
    }

    // member value is converted to a string, member text untouched
    // length of string = bitCount
    std::string getStringFromValue() {
        std::string result(bitCount, '0') ; // enough 0s, now set onyl '1's'
        unsigned i ; // indexes bits in value
        for (i = 0; i < bitCount; i++) {
            if (value & ((uint64_t)1 << i)) // bit set?
                // set '1's 'from left or right
                if (msbFirst)
                    result.at(bitCount - i - 1) = '1'; // for lower bits set right most '1's
                else
                    result.at(i) = '1'; // low bit, low string position
        }
        return result ;
    }
} ;

#endif // __BINARY_H__
