/* controller.cpp - application logic, executes user commands

    Communication to host via RS232 @ 115200
    - Receives serial request messages from host
    - execute
    - produce responses

 */
#include "controller.h" // own stuff
#include "bootlogic.h"
#include "console.h" // user response, debug logging
#include "hardware.h"
#include "ky11lb.h"
#include "messages.h"
#include "unibus.h"
#include "utils.h"
#include <avr/wdt.h>

// Singleton
Controller theController;

// initialization
void Controller::setup() {
}

// called for each addr/data pair in Flash or EEPROM.
// function code: 1 = LOG(), 2 = Deposit
// iteration stops when "false" returned
bool Controller::onPdp11DataIteration(int functionCode, uint16_t addr, uint16_t data) {
    switch (functionCode) {
    case 1:
        LOG("    addr=%06o, data=%06o\n", addr, data);
        return true;
    case 2: // deposit
        // one DATO for each addr/data pair
        theUnibus.cycleRequestedPending = false; // signal: is asynchronuos
        bool nxm;
        theUnibus.datoHead(addr, data, &nxm);
        theUnibus.datxTail();
        return !nxm; // iteration stops on nxm
    default:
        return false;
    }
}

// check if UNIBUS has captured MSYN/SSYN
// result: nullptr = nothing happened
// true: return the captured cycle in "response"
// response: callers must delete!
ResponseUnibusCycle *Controller::unibusEventAsResponse() {
    uint32_t addr;
    uint8_t c1c0;
    uint16_t data;
    bool msynCaptured = theUnibus.getMSYNcaptured(&addr, &c1c0);
    bool ssynCaptured = theUnibus.getSSYNcaptured(&data);
    if (!msynCaptured && ssynCaptured) {
        // not impossible! UNIBUS MSYN missed, because it
        // appeared between getMSYNcaptured() and getSSYNcaptured() ?
        msynCaptured = theUnibus.getMSYNcaptured(&addr, &c1c0); // retry
    }
    if (!msynCaptured)
        return nullptr; // nothing happened

    // SSYN after MSYN?
    if (!ssynCaptured) {
        // maybe we're polling in the mids of a DATI/DATO
        // wait for SSYN timeout, poll again
        delayMicroseconds(20);                           // UNIBUS SSYN timeout variable, use 20 usec
        ssynCaptured = theUnibus.getSSYNcaptured(&data); // again
    }
    bool nxm = !ssynCaptured; // no SSYN after MSYN: Non eXisting Memory
    // Report
    const char *c1c0Str = theUnibus.c1c0ToText(c1c0);
    if (nxm)
        LOG("MSYN %s addr=%06lo, SSYN NXM\n", c1c0Str, addr);
    else
        LOG("MSYN %s addr=%06lo, SSYN data=%06o\n", c1c0Str, addr, data);

    bool requested = theUnibus.cycleRequestedPending;
    theUnibus.cycleRequestedPending = false;
    return new ResponseUnibusCycle(c1c0, addr, data, nxm, requested);

    // caller must delete!
}

// return Versionstring
ResponseVersion Controller::versionAsResponse() {
    return ResponseVersion(
        "M93X2probe compiled " __DATE__ " " __TIME__);
}

// background operation
void Controller::loop() {
    // asnychonous events to reports ?
    ResponseUnibusCycle *event = unibusEventAsResponse();
    if (event != nullptr) {
        theConsole.xmtLine(event->render());
        delete event;
    }
}

// define process() for ALL message classes, even unused.
// else strange errors like "<artificial>:(.text+0x1970): undefined reference to `vtable for ResponseError'"
void *ResponseOK::process() { return nullptr; }
void *ResponseError::process() { return nullptr; }
void *ResponseUnibusSignals::process() { return nullptr; }
void *ResponseKY11LBSignals::process() { return nullptr; }
void *ResponseKM11Signals::process() { return nullptr; }
void *ResponseSwitches::process() { return nullptr; }
void *ResponseVersion::process() { return nullptr; }
void *ResponseUnibusCycle::process() { return nullptr; }
void *ResponseMcp23017Registers::process() { return nullptr; }

// hardware reset, via watchdog
void *RequestReset::process() {
    wdt_enable(WDTO_250MS);
    while (true)
        ;
    // OK response on reboot
}

void *RequestUnibusSignalsRead::process() {
    return theUnibus.getSignalsAsResponse().render();
    // response free'd, but render() to static buffer
}

void *RequestUnibusSignalWrite::process() {
    // TODO: BG74, BR740>BG4,5,6,7; BR4,5,6,7: false name conversion, but BR*,BG* not settable on hardware
    Unibus::Signal signal = theUnibus.textToSignal(signalName);
    if (signal == Unibus::Signal::none)
        return ResponseError("%s: \"%s\" not a UNIBUS signal", syntaxInfo, signalName).render();
    if (!theUnibus.writeSignal(signal, val))
        return ResponseError("%s: \"%s\" not writeable", syntaxInfo, signalName).render();
    return theUnibus.getSignalsAsResponse().render();
    // response free'd, but render() to static buffer
}

// EXAM response with cycle info
// response indirect via polling of UNIBUS cycles
void *RequestUnibusExam::process() {
    bool _nxm;
    uint16_t _data = theUnibus.datiHead(addr, &_nxm);
    theUnibus.cycleRequestedPending = true; // signal: is Response due to Request
    theUnibus.datxTail();
    // uint8_t _c1c0 = 0; // response = DATI
    // uint32_t _addr = addr ;
    // uint16_t _data ;
    // return ResponseUnibusCycle(_c1c0, _addr, _data, _nxm).render() ;
}

// DEPOSIT
// response indirect via polling of UNIBUS cycles
void *RequestUnibusDeposit::process() {
    bool _nxm;
    theUnibus.cycleRequestedPending = true; // signal: is Response due to Request
    theUnibus.datoHead(addr, data, &_nxm);
    theUnibus.datxTail();
    return nullptr;
    // uint8_t _c1c0 = 2; // response = DATO
    // uint32_t _addr = addr ;
    // uint16_t _data = data;
    //  return ResponseUnibusCycle(_c1c0, _addr, _data, _nxm).render() ;
}

// Mem sizer response with DATI of first NXM cycle
void *RequestUnibusSize::process() {
    uint8_t _c1c0 = 0; // response = DATI
    uint32_t _addr = theUnibus.sizeMem();
    uint16_t _data = 0; // doesn't care
    bool _nxm = true;
    bool _requested = true;
    // a lot of UNIBUS cycles where generated an captured
    theController.unibusEventAsResponse(); // query&clear
    return ResponseUnibusCycle(_c1c0, _addr, _data, _nxm, _requested).render();
}

// do memory test, auto sizing, final a result text
void *RequestUnibusTest::process() {
    char report[80];
    uint32_t endAddr = theUnibus.sizeMem();

    if (theUnibus.testMem(seed, endAddr, report, sizeof(report)))
        return ResponseOK(report).render();
    else
        return ResponseError(report).render();
}

// stop PDP11 CPU, download code file <n>, set power-on vector 24/26, restart CPU via ACLO/DCLO
void *RequestBoot::process() {
    theController.bootProgram(fileNr);
}

void *RequestKY11LBSignalsRead::process() {
    return theKY11LB.getSignalsAsResponse().render();
}

// set micro clock or microclock enable
void *RequestKY11LBSignalWrite::process() {
    KY11LB::Signal signal = theKY11LB.textToSignal(signalName);
    if (signal == KY11LB::Signal::none)
        return ResponseError("%s: <signal> not a writeable KY11LB signal name", syntaxInfo).render();
    if (signal == KY11LB::Signal::MAN_CLK && val == 2) {
        // "MC P"
        theKY11LB.writeSignal(signal, 1);
        theKY11LB.writeSignal(signal, 0);
    } else {
        if (!theKY11LB.writeSignal(signal, val))
            return ResponseError("%s: <signal> <val> not a writeable", syntaxInfo).render();
    }
    return theKY11LB.getSignalsAsResponse().render();
    // return ResponseOK().render();
}

// read all 28 signals from KM11A/B
// raw output of KM11 gpio values
void *RequestKM11SignalsRead::process() {
    Mcp23017 *mcp0, *mcp1;
    if (channel == 'A') {
        mcp0 = &theHardware.mcp[3][0];
        mcp1 = &theHardware.mcp[3][1];
    } else { // channel B
        mcp0 = &theHardware.mcp[3][2];
        mcp1 = &theHardware.mcp[3][3];
    }
    uint8_t val0A = mcp0->getPortA();
    uint8_t val0B = mcp0->getPortB();
    uint8_t val1A = mcp1->getPortA();
    uint8_t val1B = mcp1->getPortB();
    return ResponseKM11Signals(channel, val0A, val0B, val1A, val1B).render();
}

// write 4 outputs of KM11A as hex digit//
// response a short "OK"
void *RequestKM11SignalsWrite::process() {
    Mcp23017 *mcp0, *mcp1;
    if (channel == 'A') {
        mcp0 = &theHardware.mcp[3][0];
        mcp1 = &theHardware.mcp[3][1];
    } else {
        mcp0 = &theHardware.mcp[3][2];
        mcp1 = &theHardware.mcp[3][3];
    }
    bool out00_l = !(val03 & 1);
    bool out01_l = !(val03 & 2);
    bool out10_l = !(val03 & 4);
    bool out11_l = !(val03 & 8);
    mcp0->writeBit(Mcp23017::Register::OLATA, 7, out00_l);
    mcp0->writeBit(Mcp23017::Register::OLATB, 7, out01_l);
    mcp1->writeBit(Mcp23017::Register::OLATA, 7, out10_l);
    mcp1->writeBit(Mcp23017::Register::OLATB, 7, out11_l);
    // suppress long answer?
    return ResponseOK().render();
}

void *RequestMcp23017RegistersRead::process() {
    Mcp23017 *mcp = theHardware.getMcpByAddr(group, addr);
    if (mcp == nullptr)
        return ResponseError("%s: <group><addr> not valid", syntaxInfo).render();
    // let Mcp chip do the output
    return mcp->getRegistersAsResponse(half).render();
}

void *RequestLedWrite::process() {
    // set LED: LED 0|1 <off-ms> <on-ms>, params decimal
    int ledNr = unit ? 1 : 0; // map to 0/1
    // update blink period values
    theHardware.leds[ledNr].on_period_millis = on_period_millis;
    theHardware.leds[ledNr].off_period_millis = off_period_millis;
    return ResponseOK().render();
}

void *RequestSwitchesRead::process() {
    uint8_t val05 = theHardware.optionSwitch.getVal();
    return ResponseSwitches(val05).render();
}

// received via serial
void *RequestVersionRead::process() {
    return theController.versionAsResponse().render();
}

// eval line as command string, do and print results back to host
// input like "D UA 1234 123456"
// with opcode = D (deposit), bus = UA (Unibus address), address = 1234, value=123456
char *Controller::doHostCommand(char *line) {
	// line owned by console, available until console.loop()
    Message *request = Message::parse(line);
    char *responseText; // rendered by Message::render() to static buffer
    if (!request) {
        // parse error
        ResponseError response(Message::errorBuffer);
        responseText = response.render();
    } else {
        responseText = (char *)request->process(); // may be nullptr
        delete request;                            // minmal heap operations
    }
    return responseText;
}

/* Boot PDP11 with embedded program
 programData != nullptr:
  Deposit code from "pdp11data" into memory
  Operate SACK,BBSY,ACLO;DCLO for proper start
*/
void Controller::bootProgram(uint16_t fileNr) {
    auto programData = thePdp11Data.getRecordListByNumber(fileNr);

    // side effect: Bus signal generation via I2C MSP23017 is slow,
    // we don't need any timing delays here.

    if (programData != nullptr) {
        // load code before power cycle

        // set BBSY,SACK: no other bus member may yield own bus cycles
        // this technique used on 11/34 KY11LB console
        theUnibus.writeSACK(true);
        theUnibus.writeBBSY(true);

#ifdef TODO
        // size memory, get highest address
        uint32_t maxAddr;
        maxAddr = min(maxAddr, 0157776);
        // get highest address of program
        // add offset to all program addresses, so that
        // end address of program

        // record should be marked "absolute" or "relcoatable", onyl the latter be moved

#endif
        // write memory with boot loader
        // iterate through data, deposit each
        thePdp11Data.setIterationCallback(&theController, &Pdp11DataUser::onPdp11DataIteration);
        thePdp11Data.iterate(programData, 2); // function 2 = deposit
        // may return false, if callback noticed NXM

        // Write power-on vector.
        // 024 = entry addr
        bool nxm;
        auto entryAddr = thePdp11Data.getWord(&programData->entryAddress); // progmem->ram
        theUnibus.datoHead(024, entryAddr, &nxm);
        theUnibus.datxTail();
        // 026 = PSW 0340 level 7
        theUnibus.datoHead(026, 0340, &nxm);
        theUnibus.datxTail();
    } // write progamData

    // simulate "Power-OFF"
    theUnibus.writeACLO(true);
    delayMicroseconds(5000); // ACLO->DCLO specified as min. 5 milli sconds
                             // CPU now toggles DCLO and tries to come up
    theUnibus.writeDCLO(true);
    // BBSY,SACK back to normal
    theUnibus.writeBBSY(false);
    theUnibus.writeSACK(false);

    // simulate "Power-ON"
    // CPU should now fetch "Power-Fail vector" and branch into code deposited before
    theUnibus.writeACLO(false);
    theUnibus.writeDCLO(false);
    // these ACLO/DCLO produce an "edge detect" .. do not use it to boot
    theBootLogic.acloDetected = false; //
    delayMicroseconds(70000);          // CPU running 70 ms after DCLO 1->0
}
