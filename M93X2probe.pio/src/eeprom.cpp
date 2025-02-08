/* eeprom.cpp - low level access to I2C eeprom 24LC1025-I/P"
  from deep seek
  "give arduino code to interface a i2c eeprom 24LC1025-I/P"
*/
#include <Wire.h>
#include "eeprom.h"

void Eeprom::setup(uint8_t _deviceAddress) {
    deviceAddress = _deviceAddress ;

}


// Function to write data to EEPROM
void Eeprom::write(uint32_t memoryAddress, uint8_t* data, uint8_t dataLength) {
    Wire.beginTransmission(deviceAddress);

    // Send memory address (24-bit address for 24LC1025)
    Wire.write((uint8_t)((memoryAddress >> 16) & 0xFF)); // High byte
    Wire.write((uint8_t)((memoryAddress >> 8) & 0xFF));  // Middle byte
    Wire.write((uint8_t)(memoryAddress & 0xFF));         // Low byte

    // Send data to write
    for (uint8_t i = 0; i < dataLength; i++) {
        Wire.write(data[i]);
    }

    Wire.endTransmission();
    delay(5); // Wait for EEPROM to complete write cycle
}

// Function to read data from EEPROM
void Eeprom::read(uint32_t memoryAddress, uint8_t* data, uint8_t dataLength) {
    Wire.beginTransmission(deviceAddress);

    // Send memory address (24-bit address for 24LC1025)
    Wire.write((uint8_t)((memoryAddress >> 16) & 0xFF)); // High byte
    Wire.write((uint8_t)((memoryAddress >> 8) & 0xFF));  // Middle byte
    Wire.write((uint8_t)(memoryAddress & 0xFF));         // Low byte

    Wire.endTransmission();

    // Request data from EEPROM
    Wire.requestFrom(deviceAddress, dataLength);
    for (uint8_t i = 0; i < dataLength; i++) {
        if (Wire.available()) {
            data[i] = Wire.read();
        }
    }
}


/*

Explanation of the Code

    I2C Address:

        The 24LC1025 has a base I2C address of 0x50. If multiple EEPROMs are used, the address can be adjusted using the A0, A1, and A2 pins.

    Memory Addressing:

        The 24LC1025 uses a 24-bit address (3 bytes) to access its 128 KB memory. The address is split into high, middle, and low bytes.

    Write Function:

        writeEEPROM() sends the memory address followed by the data to be written. A small delay is added to ensure the EEPROM completes the write cycle.

    Read Function:

        readEEPROM() sends the memory address and then requests the data from the EEPROM.

    Data Handling:

        The example writes the ASCII values for "Hello" and reads them back, printing the result to the Serial Monitor.

void _setup() {
  Serial.begin(9600);
  Wire.begin(); // Initialize I2C communication

  // Write data to EEPROM
  uint32_t memoryAddress = 0x0000; // Starting address
  uint8_t dataToWrite[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello" in ASCII
  writeEEPROM(EEPROM_I2C_ADDRESS, memoryAddress, dataToWrite, sizeof(dataToWrite));

  // Read data back from EEPROM
  uint8_t dataRead[sizeof(dataToWrite)];
  readEEPROM(EEPROM_I2C_ADDRESS, memoryAddress, dataRead, sizeof(dataRead));

  // Print the read data
  Serial.print("Data read from EEPROM: ");
  for (uint8_t i = 0; i < sizeof(dataRead); i++) {
    Serial.print((char)dataRead[i]); // Print as characters
  }
  Serial.println();
}
*/

