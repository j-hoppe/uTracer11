/* ky11lb.cpp - function to handle 11/34,04 KY11LB-LB interface

MAN_CLK_ENABLE (MCE) and MAN_CLK (MC) are routed through from KY11-LB
So ustepping possible
- over KY11LB in MAINT mode (HALT, Ctrl-1, HALT), CONST, CONT ...
- uTracer   W Y MCE 1, W Y MC P, W Y MC P, ....

Incompatibilities:
- uTracer can not ustep when KY11-LB HALTed the CPU.
  uStepping cycles normally, but ends at uPC= 000
  Theory: KY11-LB sets SACK, when it halts the CPU.
    Then the uMachine finishes current macro instruction
    and cycles at MPC000 until SACK released.
    Todo: KY11-LB releases SACK sometimes to unlock uMachine from 000?
  
  uStepping not possible in mixed KY11-LB/uTracer operation.

*/

#include "ky11lb.h"
#include "hardware.h"
#include "mcp23017.h"
#include "utils.h"

// Singleton
KY11LB theKY11LB;

void KY11LB::setup() {
	// already done in Hardware::setupMcpKY11Registers(), no fixup here
}

const char *KY11LB::signalToText(KY11LB::Signal signal) {
    switch (signal) {
    case Signal::MPC:
        return "MPC";
    case Signal::MAN_CLK_ENAB:
        return "MCE";
    case Signal::MAN_CLK:
        return "MC";
    case Signal::FP11:
        return "FP11";
    case Signal::SBF:
        return "SBF";
    case Signal::LIR:
        return "LIR";
    case Signal::PBP:
        return "PBP";
    default:
        return "???";
    }
}

KY11LB::Signal KY11LB::textToSignal(char *signalText) {
    switch (toupper(signalText[0])) {
    // speed things up a little ...
    case 'F':
        if (!strcasecmp(signalText, "FP11"))
            return Signal::FP11;
        break;
    case 'L':
        if (!strcasecmp(signalText, "LIR"))
            return Signal::LIR;
        break;
    case 'M':
        if (!strcasecmp(signalText, "MC"))
            return Signal::MAN_CLK;
        if (!strcasecmp(signalText, "MCE"))
            return Signal::MAN_CLK_ENAB;
        if (!strcasecmp(signalText, "MPC"))
            return Signal::MPC;
        break;
    case 'P':
        if (!strcasecmp(signalText, "PBP"))
            return Signal::PBP;
        break;
    case 'S':
        if (!strcasecmp(signalText, "SBF"))
            return Signal::SBF;
        break;
    default:
        return Signal::none;
    }
}

uint16_t KY11LB::readMPC(uint8_t gpio20a, uint8_t gpio20b) {
    uint16_t upc = (~gpio20a & 0x7f)         // MPC0..6
                   | (~gpio20b & 0x07) << 7; // MPC7..9
    return upc;
}

uint16_t KY11LB::readMPC() {
    return readMPC(theHardware.mcp[2][0].read(Mcp23017::Register::GPIOA),
                   theHardware.mcp[2][0].read(Mcp23017::Register::GPIOB));
}

// MAN CLK ENAB
void KY11LB::writeMCE(bool val) {
    theHardware.mcp[2][0].writeBit(Mcp23017::Register::OLATA, 7, !val);
}

// MAN CLK
void KY11LB::writeMC(bool val) {
    theHardware.mcp[2][0].writeBit(Mcp23017::Register::OLATB, 7, !val);
}

// set MAN_CLK and MAN_CLK_ENAB
// result: false = couldn't do
bool KY11LB::writeSignal(Signal signal, bool val) {
    switch (signal) {
    case Signal::MAN_CLK_ENAB:
        writeMCE(val);
        return true;
    case Signal::MAN_CLK:
        writeMC(val);
        return true;
    default:
        return false;
    }
}

// hex digit string: <uPC> <FP11> <SBF> <LIR> <PBP>
ResponseKY11LBSignals KY11LB::getSignalsAsResponse(MsgTag _tag) {
    uint8_t gpio20a, gpio20b;
    gpio20a = theHardware.mcp[2][0].read(Mcp23017::Register::GPIOA);
    gpio20b = theHardware.mcp[2][0].read(Mcp23017::Register::GPIOB);
    uint16_t _mpc = readMPC(gpio20a, gpio20b);
    uint8_t _fp11 = !(gpio20b & 0x08); // gpb3
    uint8_t _sbf = !(gpio20b & 0x10);  // gpb4
    uint8_t _lir = !(gpio20b & 0x20);  // gpb5
    uint8_t _pbp = !(gpio20b & 0x40);  // gpb6
    return ResponseKY11LBSignals(_tag, _mpc, _fp11, _sbf, _lir, _pbp);
}
