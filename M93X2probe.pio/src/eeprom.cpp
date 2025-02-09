/* eeprom.cpp - low level access to I2C eeprom 24LC1025-I/P"
 */
#include <Wire.h>
#include "console.h" // LOG()
#include "eeprom.h"

void Eeprom::setup(uint8_t _deviceAddress) {
    deviceAddress = _deviceAddress ;

}


// Function to write data to EEPROM
// last byte = memoryAddress + datalength <= 0x1ffff
void Eeprom::write(uint32_t memoryAddress, uint8_t* data, uint8_t dataLength) {
// 7 bit control word is 1 0 1 <a16> <a1> <a0> <rw>,
// => device address = 0x50
    // LOG("write(addr=%lx, len=%d, data= %02x %02x %02x ...\n", memoryAddress, (int)dataLength,
    //    (unsigned)data[0], (unsigned)data[1], (unsigned)data[2]) ;
    uint8_t _actualAddress = deviceAddress;
    if (memoryAddress >= 0x10000) _actualAddress |= 0x04;  // page bit b0 = addressbit 16
    Wire.beginTransmission(_actualAddress);

    // Send lower 16 bit of memory address (24-bit address for 24LC1025)
    //Wire.write((uint8_t)((memoryAddress >> 16) & 0xFF)); // High byte
    Wire.write((uint8_t)((memoryAddress >> 8) & 0xFF));  // address <15:8>
    Wire.write((uint8_t)(memoryAddress & 0xFF));         // address<7:0>Low byte

    // Send data to write
    for (int i = 0; i < dataLength; i++) {
        Wire.write(data[i]);
    }

    Wire.endTransmission();

    // Wait for EEPROM to complete write cycle, by ACK polling
    // TODO: error handling
    while (true) {
        Wire.beginTransmission(_actualAddress);
        if (Wire.endTransmission() == 0) {
			// EEPROM is ready (ACK received)
            break;
        }
        delay(1); // Small delay to prevent excessive I2C traffic
    }

    //  delay(5); // 5ms fix delay ... where does it come from? 
}



// Function to read data from EEPROM
// last byte = memoryAddress + datalength <= 0x1ffff
void Eeprom::read(uint32_t memoryAddress, uint8_t* data, uint8_t dataLength) {
    LOG("read(addr=%lx, len=%d\n", memoryAddress, (int)dataLength) ;
    uint8_t _actualAddress = deviceAddress;
    if (memoryAddress >= 0x10000) _actualAddress |= 0x04;  // page bit b0 = addressbit 16
    Wire.beginTransmission(_actualAddress);

    // Send lower 16 bit of memory address (24-bit address for 24LC1025)
    Wire.write((uint8_t)((memoryAddress >> 8) & 0xFF));  // address <15:8>
    Wire.write((uint8_t)(memoryAddress & 0xFF));         // address<7:0>Low byte
    Wire.endTransmission();

    // Request data from EEPROM
    Wire.requestFrom((int)_actualAddress, (int)dataLength);
    for (int i = 0; i < dataLength; i++) {
        if (Wire.available()) {
            data[i] = Wire.read();
        } else
            data[i] = 0x55; // error
    }
    LOG(" ==> data= %02x %02x %02x ...\n",
        (unsigned)data[0], (unsigned)data[1], (unsigned)data[2]) ;

}

