/* binary - helper to process binary data
*/
#include "binary.h"


uint64_t getBitSubField(uint64_t value, unsigned bitFrom, unsigned bitTo) {
    uint64_t result = 0;
    unsigned bitDestIdx, bitSrcIdx;
    for (bitDestIdx = 0, bitSrcIdx = bitFrom; bitSrcIdx <= bitTo; bitSrcIdx++, bitDestIdx++)
        if (value & bitMask(bitSrcIdx))
            result |= bitMask(bitDestIdx);
    return result;
}


// member string is converted to a value, member value untouched
// "0001 1100" => 0x1d (msbFirst) or 0x38 (!msbFirst)
uint64_t BinaryString::getValueFromString() {
    uint64_t result = 0;
    unsigned bitIdx = 0; // indexes bits in value
    for (char& c : text) {
        // skip non-binary filler chars
        if (c == '0' || c == '1') {
            if (c == '1') {
                if (msbFirst)
                    result |= bitMask(bitCount - bitIdx - 1);
                else result |= bitMask(bitIdx);
            }
            bitIdx++;
        }
    }
    return result;
}


// member value is converted to a string, member text untouched
// length of string = bitCount
std::string BinaryString::getStringFromValue() {
    std::string result(bitCount, '0'); // enough 0s, now set only the '1's
    unsigned i; // indexes bits in value
    for (i = 0; i < bitCount; i++) {
        if (getBit(value, i)) { // bitMask set?
            // set '1's 'from left or right
            if (msbFirst)
                result.at(bitCount - i - 1) = '1'; // for lower bits set right most '1's
            else
                result.at(i) = '1'; // low bitMask, low string position
        }
    }
    return result;
}


