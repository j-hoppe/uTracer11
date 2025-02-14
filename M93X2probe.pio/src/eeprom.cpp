/* eeprom.cpp - low level access to I2C eeprom 24LC1025-I/P"
 */

//#define LOGGING

#include <Wire.h>
#include "eeprom.h"

#ifdef LOGGING
#include "console.h"
#endif

void Eeprom::setup(uint8_t _deviceAddress) {
    deviceAddress = _deviceAddress ;

}


// Write 'dataLength' bytes from buffer 'data' into EEPROM address 'memoryAddress'
// low level write, data must not cross 128 byte page boundary
void Eeprom::writeInPage(uint32_t address, uint8_t* data, unsigned dataLength) {
    // 7 bit control word is 1 0 1 <a16> <a1> <a0> <rw>,
    // => device address = 0x50

#ifdef LOGGING
    LOG("Eeprom::writeInPage(addr=0x%lx, len=0x%x, data= %02x %02x ... %02x\n", address, dataLength,
        (unsigned)data[0], (unsigned)data[1], (unsigned)data[dataLength-1]) ;
#endif
    uint8_t _actualAddress = deviceAddress;
    if (address >= 0x10000) _actualAddress |= 0x04;  // page bit b0 = addressbit 16
    Wire.beginTransmission(_actualAddress);

    // Send lower 16 bit of memory address (24-bit address for 24LC1025)
    //Wire.write((uint8_t)((address >> 16) & 0xFF)); // High byte
    Wire.write((uint8_t)((address >> 8) & 0xFF));  // address <15:8>
    Wire.write((uint8_t)(address & 0xFF));         // address<7:0>Low byte

    // Send data to write
    for (unsigned i = 0; i < dataLength; i++) {
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

// read 'dataLength' bytes from EEPROM address 'memoryAddress' into buffer 'data'
// low level read, data must cross 0x10000 byte boundary
void Eeprom::readInPage(uint32_t address, uint8_t* data, unsigned dataLength) {
#ifdef LOGGING
    LOG("Eeprom::readInPage(addr=0x%lx, len=0x%x\n", address, dataLength) ;
#endif
    uint8_t _actualAddress = deviceAddress;
    if (address >= 0x10000) _actualAddress |= 0x04;  // page bit b0 = addressbit 16
    Wire.beginTransmission(_actualAddress);

    // Send lower 16 bit of memory address (24-bit address for 24LC1025)
    Wire.write((uint8_t)((address >> 8) & 0xFF));  // address <15:8>
    Wire.write((uint8_t)(address & 0xFF));         // address<7:0>Low byte
    Wire.endTransmission();

    // Request data from EEPROM
    Wire.requestFrom((int)_actualAddress, (int)dataLength);
    for (unsigned i = 0; i < dataLength; i++) {
        if (Wire.available()) {
            data[i] = Wire.read();
        } else
            data[i] = 0x55; // error
    }
#ifdef LOGGING
    LOG(" ==> data= %02x %02x ... %02x\n", (unsigned)data[0], (unsigned)data[1], (unsigned)data[dataLength-1]) ;
#endif
}

// Write 'dataLength' bytes from buffer 'data' into EEPROM address 'memoryAddress'
// last byte = memoryAddress + datalength <= 0x1ffff
void Eeprom::write(uint32_t address, uint8_t* data, unsigned dataLength) {
    // split into separate write actions, which do not cross write address boundary
    while (dataLength > 0) {
        // find last allowed address in this page
        // writeBoundary 0x80: 0x12345 -> 0x1237f
        uint32_t pageEndAddr = address | (writePageBoundary-1) ;
        // bytes to write in this page
        uint32_t pageDataLength = pageEndAddr - address + 1 ; // 32 bit address arithmetic.
        if (pageDataLength > dataLength) // bytes to end of data or end of page
            pageDataLength = dataLength ;
        // pageDataLength max 'EepromMessage::maxDataSize' bytes
        writeInPage(address, data, pageDataLength);
        // skip processed 'pageDataLength' bytes
        address += pageDataLength ;
        data += pageDataLength ;
        dataLength -= pageDataLength ;
    }
}


// Function to read data from EEPROM
// last byte = memoryAddress + datalength <= 0x1ffff
void Eeprom::read(uint32_t address, uint8_t* data, unsigned dataLength) {
    // split into separate read actions, which do not cross read address boundary
    while (dataLength > 0) {
        // find last allowed address in this page
        // readBoundary 0x10000: 0x12345 -> 0x1ffff
        uint32_t pageEndAddr = address | (readPageBoundary-1) ;
        // bytes to read in this page
        uint32_t pageDataLength = pageEndAddr - address + 1 ; // 32 bit address arithmetic.
        if (pageDataLength > dataLength) // bytes to end of data or end of page
            pageDataLength = dataLength ;
        // pageDataLength max 'EepromMessage::maxDataSize' bytes
        readInPage(address, data, (unsigned)pageDataLength);
        // skip processed 'pageDataLength' bytes
        address += pageDataLength ;
        data += pageDataLength ;
        dataLength -= pageDataLength ;
    }
}

