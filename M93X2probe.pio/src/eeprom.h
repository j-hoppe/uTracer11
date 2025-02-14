/* eeprom.cpp - low level access to I2C eeprom 24LC1025-I/P"
 */
#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "utils.h"
#include "messages.h"
#include <Arduino.h>

class Eeprom {
private:
    static const uint32_t readPageBoundary = 0x10000 ;
    static const uint32_t writePageBoundary = 0x80 ; // 128
    void writeInPage(uint32_t address, uint8_t* data, unsigned dataLength) ;
    void readInPage(uint32_t address, uint8_t* data, unsigned dataLength) ;
public:
    // Default I2C address for 24LC1025
    static const uint8_t defaultDeviceAddress = 0x50 ;

    uint8_t deviceAddress = defaultDeviceAddress ;

    void setup(uint8_t _deviceAddress) ;
    void write(uint32_t address, uint8_t* data, unsigned dataLength) ;
    void read(uint32_t address, uint8_t* data, unsigned dataLength) ;

};

#endif // __EEPROM_H__
