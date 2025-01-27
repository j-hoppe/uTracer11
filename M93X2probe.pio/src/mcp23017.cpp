/* mcp23017.h - low level access to MCP23017 on I2C

*/

#include "mcp23017.h"
#include "hardware.h"
#include "utils.h"
#include "console.h" // debug logging
#include <Wire.h>

void Mcp23017::setup(uint8_t _group, uint8_t _addr) {
    group = _group;
    addr = _addr;
    used = true;
}

// I2C sequence to read one register value
// 4 bytes: W chip-address register-address; R chip-address data
// Needs 133 usec @ 400kHz SCL (scope measurement)
uint8_t Mcp23017::read(Register reg) {
    theHardware.setMcpGroup(group);
    Wire.beginTransmission(0x20 + addr);
    Wire.write((uint8_t)reg); // 2 I2C
    Wire.endTransmission();
    Wire.requestFrom(0x20 + addr, 1); // request one byte of data from MCP20317
    return Wire.read();               // store the incoming byte into "inputs"
}

// I2C sequence to  write on registers value
// 3 bytes: W chip-address register-address data
// Needs 92 usec @ 400kHz SCL (scope measurement)
// if optimize: suppress writes if no change of value
void Mcp23017::write(Register reg, uint8_t val) {

    if (theHardware.mcpWriteOptimization && (regval[(uint8_t)reg] == val))
        return;
    theHardware.setMcpGroup(group);
    // optionally supress redundant writes
    Wire.beginTransmission(0x20 + addr);
    Wire.write((uint8_t)reg);
    Wire.write(val);
    Wire.endTransmission();
    regval[(uint8_t)reg] = val; // save for optimization and bit manipulation
}

bool Mcp23017::readBit(Register reg, uint8_t bitno) {
    uint8_t mask = 1 << bitno;
    uint8_t val = read(reg);
    return (bool)(val & mask);
}

bool Mcp23017::readCachedBit(Register reg, uint8_t bitno) {
    uint8_t mask = 1 << bitno;
    uint8_t val = regval[(uint8_t)reg];
    return (bool)(val & mask);
}

// change a single bit in a register, uses shadowed register value
void Mcp23017::writeBit(Register reg, uint8_t bitno, bool bitval) {
    uint8_t mask = 1 << bitno;
    uint8_t val = regval[(uint8_t)reg];
    if (bitval)
        val |= mask; // set bit
    else
        val &= ~mask; // clr bit
    write(reg, val);
}

// dump a register block, A or B
// half: 'A', 'B' register block
ResponseMcp23017Registers Mcp23017::getRegistersAsResponse(MsgTag _tag, char half) {
    // all 11 registers
    uint8_t iodir, ipol, gpinten, defval, intcon, iocon, gppu, intf, intcap, gpio, olat;

    if (half == 'A') {
        iodir = read(Mcp23017::Register::IODIRA);
        ipol = read(Mcp23017::Register::IPOLA);
        gpinten = read(Mcp23017::Register::GPINTENA);
        defval = read(Mcp23017::Register::DEFVALA);
        intcon = read(Mcp23017::Register::INTCONA);
        iocon = read(Mcp23017::Register::IOCONA);
        gppu = read(Mcp23017::Register::GPPUA);
        intf = read(Mcp23017::Register::INTFA);
        intcap = read(Mcp23017::Register::INTCAPA);
        gpio = read(Mcp23017::Register::GPIOA);
        olat = read(Mcp23017::Register::OLATA);
    } else {
        iodir = read(Mcp23017::Register::IODIRB);
        ipol = read(Mcp23017::Register::IPOLB);
        gpinten = read(Mcp23017::Register::GPINTENB);
        defval = read(Mcp23017::Register::DEFVALB);
        intcon = read(Mcp23017::Register::INTCONB);
        iocon = read(Mcp23017::Register::IOCONB);
        gppu = read(Mcp23017::Register::GPPUB);
        intf = read(Mcp23017::Register::INTFB);
        intcap = read(Mcp23017::Register::INTCAPB);
        gpio = read(Mcp23017::Register::GPIOB);
        olat = read(Mcp23017::Register::OLATB);
    }
    return ResponseMcp23017Registers(_tag, group, addr, half,
                                     iodir, ipol, gpinten, defval,
                                     intcon, iocon, gppu, intf,
                                     intcap, gpio, olat) ;
}
