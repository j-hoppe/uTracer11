/* mcp23017.h - low level access to MCP23017 on I2C
 Adafruit MCP23xx library seems to weak
 use "Wire.h", see https://elektro.turanis.de/html/prj128/index.html

*/
#ifndef __MCP23017_H__
#define __MCP23017_H__
#include "utils.h"
#include "messages.h"
#include <Arduino.h>

class Mcp23017 {
  public:
    // register offsets
    enum class Register : uint8_t {
        IODIRA = 0x00, // Pin direction register
        IODIRB = 0x01, // Pin direction register
        IPOLA = 0x02,
        IPOLB = 0x03,
        GPINTENA = 0x04,
        GPINTENB = 0x05,
        DEFVALA = 0x06,
        DEFVALB = 0x07,
        INTCONA = 0x08,
        INTCONB = 0x09,
        IOCONA = 0x0A,
        IOCONB = 0x0B,
        GPPUA = 0x0C,
        GPPUB = 0x0D,
        INTFA = 0x0E,
        INTFB = 0x0F,
        INTCAPA = 0x10,
        INTCAPB = 0x11,
        GPIOA = 0x12,
        GPIOB = 0x13,
        OLATA = 0x14,
        OLATB = 0x15
    };
    bool used = false; // not all group/addr pairs are in use

    // last written register values, for bit manipulation
    uint8_t regval[0x16]; // range(IODIRA..OLATB)
    // each MCP is addressed
    // - with a group code 0..3, which sets MCP.A2=0 when selected
    // - and group-internal address 0..3, which sets MCP.A0 and MCP.A1
    // I2C address is 0x20 + addr, group must be set via Arduino GPIOs
    uint8_t group;
    uint8_t addr;

    void setup(uint8_t _group, uint8_t _addr);

    uint8_t read(Register reg);
    void write(Register reg, uint8_t val);

    bool readBit(Register reg, uint8_t bitno);
    bool readCachedBit(Register reg, uint8_t bitno);
    void writeBit(Register reg, uint8_t bitno, bool bitval);

	ResponseMcp23017Registers getRegistersAsResponse(MsgTag _tag, char half) ;

    // short cuts
    uint8_t getPortA() {
        return read(Register::GPIOA);
    }
    uint8_t getPortB() {
        return read(Register::GPIOB);
    }
    void setPortA(uint8_t val) {
        return write(Register::OLATA, val); // input bits discarded
    }
    void setPortB(uint8_t val) {
        return write(Register::OLATB, val); // input bits discarded
    }
};

#endif // __MCP23017_H__
