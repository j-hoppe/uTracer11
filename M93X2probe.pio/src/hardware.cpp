/* hardware.cpp - low level functions for onboard IO, LEDs and switches
    Bits: pos of bits in RS232 data
    KY11LB interface:
    MCP     Ports   Bits    Signal
    2.0     A0..A6  in0..6  MCP0..6
            B0..B2  in7..9  MCP7..9
            B3      in10
            B4
            B5
            B6
            B7
            A7      out MAN CLK ENABLE

 */
#include <Arduino.h>
#include <Wire.h>

#include "console.h" // debug logging

#include "hardware.h" // own stuff
#include "mcp23017.h"

Hardware theHardware;

// declare reset function at address 0
void (*resetFunc)(void) = 0;


void LED::setup(int _gpio) {
    gpio = _gpio;
    pinMode(gpio, OUTPUT);
    next_on_millis = 0;
    next_off_millis = 0;
    digitalWrite(gpio, 1); // output inverting: OFF
    on = false;
}

// let LED blink.
// feed in global time millis() by caller
void LED::loop(uint32_t cur_millis) {
    if (on) {
        // conditions to switch OFF: timeout or permament ON?
        if (cur_millis >= next_off_millis && off_period_millis > 0) {
            // change state, setup next state change time
            next_off_millis = cur_millis + on_period_millis;
            next_on_millis = cur_millis + off_period_millis;
            digitalWrite(gpio, 1); // output inverting: OFF
            on = false;
        }
    } else {
        // conditions to switch ON: or permament off?
        if (cur_millis >= next_on_millis && on_period_millis > 0) {
            // change state, setup next state change time
            next_off_millis = cur_millis + on_period_millis;
            next_on_millis = cur_millis + off_period_millis;
            digitalWrite(gpio, 0); // output inverting: ON
            on = true;
        }
    }
}

// just digital outputs

void PinAux::setupInput(int _gpio) {
    gpio = _gpio;
    pinMode(gpio, INPUT_PULLUP);
}

void PinAux::setupOutput(int _gpio) {
    gpio = _gpio;
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, 0); // OFF
}

void PinAux::setVal(bool val) {
    digitalWrite(gpio, val);
}

// 6 fold switch connected to A0..3,A6,A7
void OptionSwitch::setup() {
    pinMode(A0, INPUT_PULLUP); // inverting, switches to GND
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode(A3, INPUT_PULLUP);
    pinMode(A6, INPUT_PULLUP);
    pinMode(A7, INPUT_PULLUP);
}

uint8_t OptionSwitch::getVal() {
    uint8_t result = 0;
    // inputs inverting
    if (!digitalRead(A0))
        result |= 0x01;
    if (!digitalRead(A1))
        result |= 0x02;
    if (!digitalRead(A2))
        result |= 0x04;
    if (!digitalRead(A3))
        result |= 0x08;
    if (!digitalRead(A6))
        result |= 0x10;
    if (!digitalRead(A7))
        result |= 0x20;
    return result;
}

// program MCP23017s registers for 11/34 KY11LB interface
// default levels for KY11LB header outputs depend on PDP-11 model
void Hardware::setupMcpKY11LBRegisters() {

    // First define outpt pin levels, the switch pins to OUTPUT
    // KY11LB
    // Levels depend on the PDP-11 we're plugged in!
    theHardware.mcpWriteOptimization = false;      // force setup
    mcp[2][0].write(Mcp23017::Register::OLATA, 0); // init all 0
    mcp[2][0].write(Mcp23017::Register::OLATB, 0);
    mcp[2][0].write(Mcp23017::Register::GPPUA, 0xff); // inputs 100kOhm pullup
    mcp[2][0].write(Mcp23017::Register::GPPUB, 0xff); // for test with unconnected KY11

    // CLOCK outputs H=inactive (see diode logic), 
    // so KY11LB can control by default
    mcp[2][0].writeBit(Mcp23017::Register::OLATA, 7, 1); // man_clk_enab_l = inactive
    mcp[2][0].writeBit(Mcp23017::Register::OLATB, 7, 1); // man_clk_l = inactive, for active pulse

    // enable outputs
    //  bit "1" = in "0" = out
    mcp[2][0].write(Mcp23017::Register::IODIRA, 0x7f); // GPA7 output
    mcp[2][0].write(Mcp23017::Register::IODIRB, 0x7f); // GPB7 output
    theHardware.mcpWriteOptimization = true;
}


// program MCP23017s registers for KM11A/B interfaces
// default levels for KM11 outputs do not depend on PDP-11 model ?
// HIGH = inactive?
void Hardware::setupMcpKM11Registers() {
    // KM11 A/B
    // 4 MCPs, 7 inputs, GPx7 = output = default High
    theHardware.mcpWriteOptimization = false; // force setup
    mcp[3][0].write(Mcp23017::Register::OLATA, 0x80);
    mcp[3][0].write(Mcp23017::Register::OLATB, 0x80);
    mcp[3][1].write(Mcp23017::Register::OLATA, 0x80);
    mcp[3][1].write(Mcp23017::Register::OLATB, 0x80);
    mcp[3][2].write(Mcp23017::Register::OLATA, 0x80);
    mcp[3][2].write(Mcp23017::Register::OLATB, 0x80);
    mcp[3][3].write(Mcp23017::Register::OLATA, 0x80);
    mcp[3][3].write(Mcp23017::Register::OLATB, 0x80);
    // set 100kOhm pullup for inputs, for test with unconnected KM11
    mcp[3][0].write(Mcp23017::Register::GPPUA, 0xff);
    mcp[3][0].write(Mcp23017::Register::GPPUB, 0xff);
    mcp[3][1].write(Mcp23017::Register::GPPUA, 0xff);
    mcp[3][1].write(Mcp23017::Register::GPPUB, 0xff);
    mcp[3][2].write(Mcp23017::Register::GPPUA, 0xff);
    mcp[3][2].write(Mcp23017::Register::GPPUB, 0xff);
    mcp[3][3].write(Mcp23017::Register::GPPUA, 0xff);
    mcp[3][3].write(Mcp23017::Register::GPPUB, 0xff);

    // enable outputs
    mcp[3][0].write(Mcp23017::Register::IODIRA, 0x7f); // GPA7 output
    mcp[3][0].write(Mcp23017::Register::IODIRB, 0x7f); // GPB7 output
    mcp[3][1].write(Mcp23017::Register::IODIRA, 0x7f);
    mcp[3][1].write(Mcp23017::Register::IODIRB, 0x7f);
    mcp[3][2].write(Mcp23017::Register::IODIRA, 0x7f);
    mcp[3][2].write(Mcp23017::Register::IODIRB, 0x7f);
    mcp[3][3].write(Mcp23017::Register::IODIRA, 0x7f);
    mcp[3][3].write(Mcp23017::Register::IODIRB, 0x7f);
    theHardware.mcpWriteOptimization = true;
}

void Hardware::setupMcpUnibusRegisters() {
    // program MCP23017s registers for UNIBUS interface
    // default levels for UNIBUs signals outputs are all LOW,
    //  ULN2803 low side drivers disabled
    // internal LATCH_DATA default High
    // default output levels
    theHardware.mcpWriteOptimization = false; // force setup
    theHardware.mcp[0][0].write(Mcp23017::Register::OLATA, 0x00);
    theHardware.mcp[0][0].write(Mcp23017::Register::OLATB, 0x00);
    theHardware.mcp[0][1].write(Mcp23017::Register::OLATA, 0x00);
    theHardware.mcp[0][1].write(Mcp23017::Register::OLATB, 0x00);
    theHardware.mcp[0][2].write(Mcp23017::Register::OLATA, 0x00);
    theHardware.mcp[0][2].write(Mcp23017::Register::OLATB, 0x80); // LATCH_DATA
    theHardware.mcp[0][3].write(Mcp23017::Register::OLATA, 0x00);
    theHardware.mcp[0][3].write(Mcp23017::Register::OLATB, 0x00);
    theHardware.mcp[1][0].write(Mcp23017::Register::OLATA, 0x00);
    theHardware.mcp[1][0].write(Mcp23017::Register::OLATB, 0x00);
    theHardware.mcp[1][1].write(Mcp23017::Register::OLATA, 0x00);
    theHardware.mcp[1][1].write(Mcp23017::Register::OLATB, 0x00);
    theHardware.mcp[1][2].write(Mcp23017::Register::OLATA, 0x00);
    theHardware.mcp[1][2].write(Mcp23017::Register::OLATB, 0x00);

    // Capture ADDRESS and CONTROL from Chip 0.A, 0.B and 1.A
    // on MSYN_H MCP00A/B, MCP01A/B
    // INTx outputs not used

    // MCP00A/B,MCP01A/B: Capture on MSYN_H GPx6 0->1,
    // INTCONx=1: compare against DEFVAL 0
    // GPINTENx=1: interrupt/capture enable
    theHardware.mcp[0][0].write(Mcp23017::Register::INTCONA, 0x40);
    theHardware.mcp[0][0].write(Mcp23017::Register::DEFVALA, 0x00);
    theHardware.mcp[0][0].write(Mcp23017::Register::GPINTENA, 0x40);
    theHardware.mcp[0][0].write(Mcp23017::Register::INTCONB, 0x40);
    theHardware.mcp[0][0].write(Mcp23017::Register::DEFVALB, 0x00);
    theHardware.mcp[0][0].write(Mcp23017::Register::GPINTENB, 0x40);
    theHardware.mcp[0][1].write(Mcp23017::Register::INTCONA, 0x40);
    theHardware.mcp[0][1].write(Mcp23017::Register::DEFVALA, 0x00);
    theHardware.mcp[0][1].write(Mcp23017::Register::GPINTENA, 0x40);
    theHardware.mcp[0][1].write(Mcp23017::Register::INTCONB, 0x40);
    theHardware.mcp[0][1].write(Mcp23017::Register::DEFVALB, 0x00);
    theHardware.mcp[0][1].write(Mcp23017::Register::GPINTENB, 0x40);

    // MCP02A/B,MCP03A: Capture on SSYN_H GPx6 0->1, compare against DEFVAL 0
    theHardware.mcp[0][2].write(Mcp23017::Register::INTCONA, 0x40);
    theHardware.mcp[0][2].write(Mcp23017::Register::DEFVALA, 0x00);
    theHardware.mcp[0][2].write(Mcp23017::Register::GPINTENA, 0x40);
    theHardware.mcp[0][2].write(Mcp23017::Register::INTCONB, 0x40);
    theHardware.mcp[0][2].write(Mcp23017::Register::DEFVALB, 0x00);
    theHardware.mcp[0][2].write(Mcp23017::Register::GPINTENB, 0x40);
    theHardware.mcp[0][3].write(Mcp23017::Register::INTCONA, 0x40);
    theHardware.mcp[0][3].write(Mcp23017::Register::DEFVALA, 0x00);
    theHardware.mcp[0][3].write(Mcp23017::Register::GPINTENA, 0x40);

    // enable outputs
    theHardware.mcp[0][0].write(Mcp23017::Register::IODIRA, 0x7f); // ACLO_OUT
    theHardware.mcp[0][0].write(Mcp23017::Register::IODIRB, 0x7f); // DCLO_OUT
    theHardware.mcp[0][1].write(Mcp23017::Register::IODIRA, 0x7f); // A16_OUT
    theHardware.mcp[0][1].write(Mcp23017::Register::IODIRB, 0x7f); // A17_OUT
    theHardware.mcp[0][2].write(Mcp23017::Register::IODIRA, 0x7f); // INIT_OUT
    theHardware.mcp[0][2].write(Mcp23017::Register::IODIRB, 0x7f); // LATCH_DATA
    theHardware.mcp[0][3].write(Mcp23017::Register::IODIRA, 0x7f); // MSYN_OUT
    theHardware.mcp[0][3].write(Mcp23017::Register::IODIRB, 0x0f); // BBSY, C1_OUT, SACK_OUT, SSYN_OUT
    theHardware.mcp[1][0].write(Mcp23017::Register::IODIRA, 0xff);
    theHardware.mcp[1][0].write(Mcp23017::Register::IODIRB, 0xff);
    theHardware.mcp[1][1].write(Mcp23017::Register::IODIRA, 0x00); // A<00:07>_OUT
    theHardware.mcp[1][1].write(Mcp23017::Register::IODIRB, 0x00); // A<08:15>_OUT
    theHardware.mcp[1][2].write(Mcp23017::Register::IODIRA, 0x00); // D<00:07>_OUT
    theHardware.mcp[1][2].write(Mcp23017::Register::IODIRB, 0x00); // D<08:15>_OUT
    theHardware.mcpWriteOptimization = true;
}



// initialization
void Hardware::setup() {
    // enable I2C
    mcpWriteOptimization = false;
    Wire.begin();
    // I2C speed:
    // MCP23017 should run upto 1.7MHz
    // Nano Every: 1.7MHz falls to 100kHz
    // I2C MCP23017 seems to lock with 1MHz, even with 1.7kOhm pullups
    // Wire.setClock(1000000) ; // 1MHz.
    Wire.setClock(400000); // 400kHz.

    leds[0].setup(6); // assign GPIO
    leds[1].setup(7);

    pinaux[0].setupOutput(10);
    pinaux[1].setupOutput(11); // or MOSI 
    pinaux[2].setupOutput(12); // or MISO 
    pinaux[3].setupOutput(13); // or SCK 
    optionSwitch.setup();

    // MCP23017 global Reset
    pinMode(2, OUTPUT);

    // MCP23017 group select pins
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);

    activeMcpGroup = 0xff; // undefined

    // Wire MCP23017s with <group> and <addr>, see schematic
    mcp[0][0].setup(0, 0); // UNIBUS, A00..A11,ACLO,DCLO
    mcp[0][1].setup(0, 1); // UNIBUS, A12..A17,C0,C1,INTR,BBSY
    mcp[0][2].setup(0, 2); // UNIBUS, D00..D11, INIT
    mcp[0][3].setup(0, 3); // UNIBUS, D12..D15, PA,PB;ACLO,DCLO,INIT,SSYN
    mcp[1][0].setup(1, 0); // UNIBUS, BR4..7, NPR,NPG,SACK,BG4..7
    mcp[1][1].setup(1, 1); // UNIBUS, out A00..A15
    mcp[1][2].setup(1, 2); // UNIBUS, out D00..D15

    mcp[2][0].setup(2, 0); // 11/34 interface KY11LB, MCP00..MCP09,FP11,SBF,LIR,FBP,CLK

    mcp[3][0].setup(3, 0); // KM11, KM11A_IN00..31, OUT00,OUT01
    mcp[3][1].setup(3, 1); // KM11, KM11A_IN32..63, OUT10,OUT11
    mcp[3][2].setup(3, 2); // KM11, KM11B_IN00..31, OUT00,OUT01
    mcp[3][3].setup(3, 3); // KM11, KM11B_IN32..63, OUT10,OUT11

	// connect direct DCLO interrupt for boot 1->0 edge

    pulseMcpReset();
    setupMcpKY11LBRegisters();
    setupMcpKM11Registers();
    setupMcpUnibusRegisters();

	eeprom.setup(Eeprom::defaultDeviceAddress) ;
}

// background operation
void Hardware::loop(uint32_t cur_millis) {
    // blink LEDs
    leds[0].loop(cur_millis);
    leds[1].loop(cur_millis);
}

// get pointer to MCP in use
Mcp23017 *Hardware::getMcpByAddr(uint8_t group, uint8_t addr) {
    if (group > max_mcp_group)
        return nullptr;
    if (addr > max_mcp_addr)
        return nullptr;
    Mcp23017 *result = &(mcp[group][addr]);
    if (!result->used)
        return nullptr;
    return result;
}

// state of global MCP-Reset-Line
void Hardware::pulseMcpReset() {
    digitalWrite(2, 0); // Reset low active
    delay(1);           // 1ms
    digitalWrite(2, 1); // inactive again
}

// set binary MCP23017 group nr to 74LS138 decoder
// optimized
void Hardware::setMcpGroup(uint8_t group) {
    if (group != activeMcpGroup) {
        digitalWrite(3, (group & 1) ? 1 : 0);
        digitalWrite(4, (group & 2) ? 1 : 0);
        digitalWrite(5, (group & 4) ? 1 : 0);
        activeMcpGroup = group;
    }
}
