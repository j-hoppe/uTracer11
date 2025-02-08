/* eeprom.cpp - low level access to I2C eeprom 24LC1025-I/P"
 */
#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "utils.h"
#include "messages.h"
#include <Arduino.h>

class Eeprom {
public:
    // Default I2C address for 24LC1025
    static const uint8_t defaultDeviceAddress = 0x50 ;

    uint8_t deviceAddress = defaultDeviceAddress ;

    void setup(uint8_t _deviceAddress) ;
    void write(uint32_t memoryAddress, uint8_t* data, uint8_t dataLength) ;
    void read(uint32_t memoryAddress, uint8_t* data, uint8_t dataLength) ;

};

#endif // __EEPROM_H__
