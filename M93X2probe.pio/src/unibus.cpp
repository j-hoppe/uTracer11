/* unibus.cpp - UNIBUS signal capture and generation

 Uses MCP registers 0.0, 0.1, 0.2, 0.3, 1.0, 1.1, 1.2

 Capture:
 ADDRESS is captured by MCP23017 on MSYN_H 0->1
 DATA is captured by MCP23017/74ls374 octal latches on SSYN_H 0->1
    MCP too slow: UNIBUS MSYN may go inactive immediately after SYN high,
    DATA only guaranted for 75ns.
    MCP interrupt time 400ns => 74ls374 capture DATA for MCP

*/
#include "unibus.h"
#include "console.h" // LOG
#include "hardware.h"

Unibus theUnibus;

void Unibus::setup() {
	// see Hardware::setupMcpUnibusRegisters(), minor fixup here
    duplicate_MSYN_INT = false;
    duplicate_SSYN_INT = false;
    cycleRequestedPending = false ;
}

const char *Unibus::signalToText(Unibus::Signal signal) {
    switch (signal) {
    case Signal::addr:
        return "ADDR";
    case Signal::data:
        return "DATA";
    case Signal::c0:
        return "C0";
    case Signal::c1:
        return "C1";
    case Signal::msyn:
        return "MSYN";
    case Signal::ssyn:
        return "SSYN";
    case Signal::pa:
        return "PA";
    case Signal::pb:
        return "PB";
    case Signal::intr:
        return "INTR";
    case Signal::br4:
        return "BR4";
    case Signal::br5:
        return "BR5";
    case Signal::br6:
        return "BR6";
    case Signal::br7:
        return "BR7";
    case Signal::bg4:
        return "BG4";
    case Signal::bg5:
        return "BG5";
    case Signal::bg6:
        return "BG6";
    case Signal::bg7:
        return "BG7";
    case Signal::npr:
        return "NPR";
    case Signal::npg:
        return "NPG";
    case Signal::sack:
        return "SACK";
    case Signal::bbsy:
        return "BBSY";
    case Signal::init:
        return "INIT";
    case Signal::aclo:
        return "ACLO";
    case Signal::dclo:
        return "DCLO";
    default:
        return "???";
    }
}

Unibus::Signal Unibus::textToSignal(char *signalText) {
    switch (toupper(signalText[0])) {
    // speed things up a little ...
    case 'A':
        if (!strcasecmp(signalText, "ACLO"))
            return Signal::aclo;
        if (!strcasecmp(signalText, "ADDR"))
            return Signal::addr;
        break;
    case 'B':
        if (!strcasecmp(signalText, "BBSY"))
            return Signal::bbsy;
        if (!strcasecmp(signalText, "BG4"))
            return Signal::bg4;
        if (!strcasecmp(signalText, "BG5"))
            return Signal::bg5;
        if (!strcasecmp(signalText, "BG6"))
            return Signal::bg6;
        if (!strcasecmp(signalText, "BG7"))
            return Signal::bg7;
        if (!strcasecmp(signalText, "BR4"))
            return Signal::br4;
        if (!strcasecmp(signalText, "BR5"))
            return Signal::br5;
        if (!strcasecmp(signalText, "BR6"))
            return Signal::br6;
        if (!strcasecmp(signalText, "BR7"))
            return Signal::br7;
        break;
    case 'C':
        if (!strcasecmp(signalText, "C0"))
            return Signal::c0;
        if (!strcasecmp(signalText, "C1"))
            return Signal::c1;
        break;
    case 'D':
        if (!strcasecmp(signalText, "DATA"))
            return Signal::data;
        if (!strcasecmp(signalText, "DCLO"))
            return Signal::dclo;
        break;
    case 'I':
        if (!strcasecmp(signalText, "INIT"))
            return Signal::init;
        if (!strcasecmp(signalText, "INTR"))
            return Signal::intr;
        break;
    case 'M':
        if (!strcasecmp(signalText, "MSYN"))
            return Signal::msyn;
        break;
    case 'N':
        if (!strcasecmp(signalText, "NPG"))
            return Signal::npg;
        if (!strcasecmp(signalText, "NPR"))
            return Signal::npr;
        break;
    case 'P':
        if (!strcasecmp(signalText, "PA"))
            return Signal::pa;
        if (!strcasecmp(signalText, "PB"))
            return Signal::pb;
        break;
    case 'S':
        if (!strcasecmp(signalText, "SACK"))
            return Signal::sack;
        if (!strcasecmp(signalText, "SSYN"))
            return Signal::ssyn;
        break;
    }
    return Signal::none;
}

const char *Unibus::c1c0ToText(uint8_t c1c0) {
    static const char *str[5] = {"DATI", "DATIP", "DATO", "DATOB"};
    if (c1c0 < 4)
        return str[c1c0];
    else
        return "???";
}

// for DATO/DATI some signals must be readable at random
// for optimization use prefetched MCP values
uint32_t Unibus::readADDR(uint8_t gpio00a, uint8_t gpio00b, uint8_t gpio01a) {
    uint32_t addr = ((uint32_t)(~gpio01a & 0x3f) << 12)  // A12..17 = mcp01.gpa0..5
                    | ((uint32_t)(~gpio00b & 0x3f) << 6) // A06..11 = mcp00.gpb0..5
                    | (~gpio00a & 0x3f);                 // A00..05 = mcp00.gpa0..5
    return addr;
}

uint16_t Unibus::readDATA(uint8_t gpio02a, uint8_t gpio02b, uint8_t gpio03a) {
    uint16_t data = ((uint16_t)(~gpio03a & 0x0f) << 12)  // D12..15 = mcp03.gpa0..3
                    | ((uint16_t)(~gpio02b & 0x3f) << 6) // D06..11 = mcp02.gpb0..5
                    | (~gpio02a & 0x3f);                 // D00..05 = mcp02.gpa0..5
    return data;
}

bool Unibus::readMSYN(uint8_t gpio00a) {
    // MSYN_H has positive logic
    bool msyn = !!(gpio00a & 0x40); // msyn_h = mcp00.gpa6
    return msyn;
}

bool Unibus::readSSYN(uint8_t gpio02a) {
    // SSYN_H has positive logic
    bool ssyn = !!(gpio02a & 0x40); // ssyn_h = mcp02.gpa6
    return ssyn;
}

uint8_t Unibus::readC1C0(uint8_t gpio01b) {
    uint8_t c1c0 = ~gpio01b & 3; // C0 = GBP0, C1 = GPB1
    return c1c0;
}

uint32_t Unibus::readADDR() {
    return readADDR(theHardware.mcp[0][0].read(Mcp23017::Register::GPIOA),
                    theHardware.mcp[0][0].read(Mcp23017::Register::GPIOB),
                    theHardware.mcp[0][1].read(Mcp23017::Register::GPIOA));
}

uint16_t Unibus::readDATA() {
    return readDATA(theHardware.mcp[0][2].read(Mcp23017::Register::GPIOA),
                    theHardware.mcp[0][2].read(Mcp23017::Register::GPIOB),
                    theHardware.mcp[0][3].read(Mcp23017::Register::GPIOA));
}
bool Unibus::readMSYN() {
    return readMSYN(theHardware.mcp[0][0].read(Mcp23017::Register::GPIOA));
}

bool Unibus::readSSYN() {
    return readSSYN(theHardware.mcp[0][2].read(Mcp23017::Register::GPIOA));
}

uint8_t Unibus::readC1C0() {
    return readC1C0(theHardware.mcp[0][1].read(Mcp23017::Register::GPIOB));
}

// make a multi-token octal/hex string containing all signals
// order like in UNIBUS spec:
// <addr> <data> <c1,c0> <msyn> <ssyn> <pb,pa> <intr> <br7..4> <bg7..4> <npr> <npg> <sack> <bbsy> <init> <aclo> <dclo>
ResponseUnibusSignals Unibus::getSignalsAsResponse() {
    // cache all MCP inputs
    uint8_t gpio00a, gpio00b, gpio01a, gpio01b, gpio02a, gpio02b, gpio03a, gpio03b, gpio10a, gpio10b;
    // force toggle of DATA_LATCH, to latched DATA into 74LS374
    theHardware.mcp[0][2].writeBit(Mcp23017::Register::OLATB, 7, 0);
    theHardware.mcp[0][2].writeBit(Mcp23017::Register::OLATB, 7, 1);
    // gather raw gpiodata
    gpio00a = theHardware.mcp[0][0].read(Mcp23017::Register::GPIOA);
    gpio00b = theHardware.mcp[0][0].read(Mcp23017::Register::GPIOB);
    gpio01a = theHardware.mcp[0][1].read(Mcp23017::Register::GPIOA);
    gpio01b = theHardware.mcp[0][1].read(Mcp23017::Register::GPIOB);
    gpio02a = theHardware.mcp[0][2].read(Mcp23017::Register::GPIOA);
    gpio02b = theHardware.mcp[0][2].read(Mcp23017::Register::GPIOB);
    gpio03a = theHardware.mcp[0][3].read(Mcp23017::Register::GPIOA);
    gpio03b = theHardware.mcp[0][3].read(Mcp23017::Register::GPIOB);
    gpio10a = theHardware.mcp[1][0].read(Mcp23017::Register::GPIOA);
    gpio10b = theHardware.mcp[1][0].read(Mcp23017::Register::GPIOB);
    // most gpio contain bit-inverse unprocessed UNIBUS levels
    // LOG("gpio00a-b=%02x-%02x, gpio01a-b=%02x-%02x\n", (unsigned)gpio00a, (unsigned)gpio00b, (unsigned)gpio01a, (unsigned)gpio01b);
    uint32_t _addr = readADDR(gpio00a, gpio00b, gpio01a);
    uint16_t _data = readDATA(gpio02a, gpio02b, gpio03a);
    uint8_t _c1c0 = (~gpio01b & 0x03);  // c1,c0 = mcp01.gpb1,0
    uint8_t _msyn = !!(gpio00a & 0x40); // msyn_h = mcp00.gpa6
    uint8_t _ssyn = readSSYN(gpio02a);
    uint8_t _pbpa = ((~gpio03a >> 4) & 0x03); // pb,pa = mcp03.gpa5,4
    uint8_t _intr = !(gpio01b & 0x04);        // intr = mcp01.gpb2
    uint8_t _br74 = (~gpio10a & 0x0f);        // br7..4 = mcp10.gpa3..0
    uint8_t _bg74 = gpio10b & 0x0f;           // bg7..4_h = mcp10.gpb3..0 not inverted
    uint8_t _npr = !(gpio10a & 0x10);         // npr = mcp10.gpa4
    uint8_t _npg = !!(gpio10a & 0x20);         // npr = mcp10.gpa5 not inverted
    uint8_t _sack = !(gpio10a & 0x40);        // npr = mcp10.gpa6
    uint8_t _bbsy = !(gpio01b & 0x08);        // npr = mcp01.gpb3
    uint8_t _init = !(gpio03b & 0x04);        // init = mcp03.gpb2
    uint8_t _aclo = !(gpio03b & 0x01);        // init = mcp03.gpb0
    uint8_t _dclo = !(gpio03b & 0x02);        // init = mcp03.gpb1
    return ResponseUnibusSignals(_addr, _data, _c1c0, _msyn, _ssyn,
                                _pbpa, _intr, _br74, _bg74, _npr, _npg, _sack, _bbsy,
                                _init, _aclo, _dclo) ;
}

void Unibus::writeADDR(uint32_t val) {
    uint8_t regVal;
    bool regBit;
    regVal = (val & 0xff); // mcp11.GPA = A00..07
    theHardware.mcp[1][1].write(Mcp23017::Register::OLATA, regVal);
    regVal = ((val >> 8) & 0xff); // mcp11.GPB = A08..15
    theHardware.mcp[1][1].write(Mcp23017::Register::OLATB, regVal);
    regBit = !!(val & (1L << 16)); // mcp01.GPA7 = A16
    theHardware.mcp[0][1].writeBit(Mcp23017::Register::OLATA, 7, regBit);
    regBit = !!(val & (1L << 17)); // mcp01.GPB7 = A17
    theHardware.mcp[0][1].writeBit(Mcp23017::Register::OLATB, 7, regBit);
}

void Unibus::writeDATA(uint16_t val) {
    uint8_t regVal;
    regVal = (val & 0xff); // mcp12.GPA = D00..07
    theHardware.mcp[1][2].write(Mcp23017::Register::OLATA, regVal);
    regVal = ((val >> 8) & 0xff); // mcp12.GPB = D08..15
    theHardware.mcp[1][2].write(Mcp23017::Register::OLATB, regVal);
}
// C0 always inactive, need only write C1: 0=DATI, 1=DATO
void Unibus::writeC1(bool val) {
    bool regBit = !!val; // mcp03.GPB5 = C1
    theHardware.mcp[0][3].writeBit(Mcp23017::Register::OLATB, 5, regBit);
}

void Unibus::writeMSYN(bool val) {
    bool regBit = !!val; // mcp03.GPA7 = MSYN
    theHardware.mcp[0][3].writeBit(Mcp23017::Register::OLATA, 7, regBit);
}
void Unibus::writeSSYN(bool val) {
    bool regBit = !!val; // mcp03.GPB7 = MSYN
    theHardware.mcp[0][3].writeBit(Mcp23017::Register::OLATB, 7, regBit);
}
void Unibus::writeSACK(bool val) {
    bool regBit = !!val; // mcp03.GPB6 = SACK
    theHardware.mcp[0][3].writeBit(Mcp23017::Register::OLATB, 6, regBit);
}
void Unibus::writeBBSY(bool val) {
    bool regBit = !!val; // mcp03.GPB4 = BBSY
    theHardware.mcp[0][3].writeBit(Mcp23017::Register::OLATB, 4, regBit);
}
void Unibus::writeACLO(bool val) {
    bool regBit = !!val; // mcp00.GPA7 = ACLO
    theHardware.mcp[0][0].writeBit(Mcp23017::Register::OLATA, 7, regBit);
}
void Unibus::writeDCLO(bool val) {
    bool regBit = !!val; // mcp00.GPB7 = DCLO
    theHardware.mcp[0][0].writeBit(Mcp23017::Register::OLATB, 7, regBit);
}
void Unibus::writeINIT(bool val) {
    bool regBit = !!val; // mcp02.GPA7 = INIT
    theHardware.mcp[0][2].writeBit(Mcp23017::Register::OLATA, 7, regBit);
}

// write a UNIBUS signal level onto bus
// only ADDR, DATA, MSYN, SSYN, SACK, BBSY, ACLO DCLO, INIT can be set
// result: false = couldn't do
bool Unibus::writeSignal(Signal signal, uint32_t val) {
    bool ok = true;
    switch (signal) {
    case Signal::addr:
        writeADDR(val);
        break;
    case Signal::data:
        writeDATA(val & 0xffff);
        break;
    case Signal::c1:
        writeC1(!!val);
        break;
    case Signal::msyn:
        writeMSYN(!!val);
        break;
    case Signal::ssyn:
        writeSSYN(!!val);
        break;
    case Signal::sack:
        writeSACK(!!val);
        break;
    case Signal::bbsy:
        writeBBSY(!!val);
        break;
    case Signal::aclo:
        writeACLO(!!val);
        break;
    case Signal::dclo:
        writeDCLO(!!val);
        break;
    case Signal::init:
        writeINIT(!!val);
        break;
    default:
        ok = false;
    }
    return ok;
}

// execute simple DATI (read) cycle
// no check for BBSY, no NPR/NPG/SACK hand shake
// result nxm: true when no slave responded with SSYN
uint16_t Unibus::datiHead(uint32_t addr, bool *nxm) {
    uint16_t data = 0;
    // assume msyn, ssy inactive
    writeADDR(addr);
    writeC1(0);   // control lines C1=0, C0=0 "DATI"
    writeMSYN(1); // clock ADDR and C1C0 into slave
    // I2C cycle is min 40usec @ 1MHz, UNIBUS timeout passed already
    if (readSSYN()) {
        // slave responded
        data = readDATA();
        *nxm = false;
    } else {
        // slave did not respond
        *nxm = true;
    }
    writeMSYN(0); // end master cycle, slave inactivates SSYN
    return data;
}

// faster DATI, without data transfer, to probe for mem response NXM
void Unibus::datiProbeHead(uint32_t addr, bool *nxm) {
    // assume msyn, ssy inactive
    writeADDR(addr);
    writeC1(0);   // control lines C1=0, C0=0 "DATI"
    writeMSYN(1); // clock ADDR and C1C0 into slave
    // I2C cycle is min 40usec @ 1MHz, UNIBUS timeout passed already
    *nxm = !readSSYN();
    writeMSYN(0); // end master cycle, slave inactivates SSYN
}

// execute DATO (write) cycle on Unibus
// no check for BBSY, no NPR/NPG/SACK hand shake
// nxm: true when no slave responded with SSYN
void Unibus::datoHead(uint32_t addr, uint16_t data, bool *nxm) {
    // assume msyn, ssy inactive
    writeADDR(addr);
    writeDATA(data);
    writeC1(1);   // control lines C1=1, C0=0 "DATO"
    writeMSYN(1); // clock ADDR and C1C0 into slave
    // I2C cycle is min 40usec @ 1MHz, UNIBUS timeout passed already
    *nxm = !readSSYN(); // slave responded, or not!
    writeMSYN(0);       // end master cycle, slave inactivates SSYN
}

// remove addr, data, c1 and msyn from signal lines
void Unibus::datxTail() {
    writeADDR(0); // free bus
    writeDATA(0);
    writeC1(0);
}

// scans Unibus, returns first invalid address
// assume addr has existing memory,
// return start of next non-existing block
uint32_t Unibus::sizeMem(uint32_t addr, uint32_t blockSize) {
    bool nxm = false;
    // 1. find first invalid 256 block
    while (!nxm && addr <= 0x3ffff) { // UNIBUS 18bit addresses
        datiProbeHead(addr, &nxm);
        // LOG("mem[%06lo] %s\n", addr,  nxm?"nxm":"ok") ;
        if (!nxm)
            addr += blockSize;
    }
    datxTail();
    return addr;
}

// optimization: first scan for 256 blocks, then fine-tune
// hardcoded binary search.
uint32_t Unibus::sizeMem() {
    uint32_t addr, blockSize;
    bool nxm;
    // any mem?
    datiProbeHead(0, &nxm);
    if (nxm)
        return 0; // no, even addr 0 invalid
    // 18 bit address space bin search into 6-6-6
    blockSize = 64 * 64;
    addr = sizeMem(0, blockSize);
    addr -= blockSize; // start of last good block

    blockSize = 64;
    addr = sizeMem(addr, blockSize);
    addr -= blockSize; // start of last good block

    addr = sizeMem(addr, 2);
    return addr; // first bad addr
}

// fill UNIBUS memory with random number upto endAddr,
// then compare
// result: false = error
// buffer: succcess or error message
bool Unibus::testMem(uint32_t seed, uint32_t endAddr, char *buffer, int bufferSize) {
    uint32_t addr;
    uint16_t testData=0, curData=0;
    bool nxm;

    // randomSeed(0) crashed the generator???
    if (seed == 0)
        seed = 0x1139fa50; // aritrary

    /*
        LOG("A seed=%ld\n", seed);
        randomSeed(seed);
        for (addr = 0; addr < 4; addr++) {
            testData = random(0x10000) & 0xffff;
            LOG("A%d %06o\n", addr, testData);
        }
        LOG("B seed=%ld\n", seed);
        randomSeed(seed);
        for (addr = 0; addr < 4; addr++) {
            testData = random(0x10000) & 0xffff;
            LOG("B%d %06o\n", addr, testData);
        }
    */
    // 1. write.
    // set random sequence
    randomSeed(seed);
    nxm = false;
    for (addr = 0; !nxm && addr < endAddr; addr += 2) {
        testData = random(0x10000) & 0xffff;
        // LOG("mem[%06lo] <= %06o\n", addr, testData);
        datoHead(addr, testData, &nxm);
    }
    datxTail();
    // 2. read back
    // set random sequence
    randomSeed(seed);
    bool mismatch = false;
    for (addr = 0; !nxm && !mismatch && addr < endAddr; addr += 2) {
        testData = random(0x10000) & 0xffff;
        curData = datiHead(addr, &nxm);
        // LOG("mem[%06lo] => soll %06o, ist %06o\n", addr, testData, curData);
        mismatch = (testData != curData);
    }
    datxTail();
    addr -= 2; // last tested addr
    if (nxm)
        snprintf(buffer, bufferSize, "NXM @ %06lo", addr);
    else if (mismatch)
        snprintf(buffer, bufferSize, "mismatch @ %06lo, written=%06o, read back=%06o",
                 addr, testData, curData);
    else
        snprintf(buffer, bufferSize, "000000..%06lo tested OK", addr);
    return !nxm && !mismatch;
}

/*
    Check, if MSYN 1->0 edge was captured
    if true, return address and C1,C0
    Latch DADDR DATA only on 1->0 edge.

     MSYN/SSYN may stuck at 0 for current micro step,
    INT are re-armed after CAP* readout,
    and fired again on same level.
    INTs are filtered out until MSYN is found "inactve" again => duplicate_MSYN_INT = 0
    or maybe 1 inactive again.
*/
bool Unibus::getMSYNcaptured(uint32_t *addr, uint8_t *c1c0) {
    // theHardware.pinaux[0].setVal(true) ;
    //  MSYN interrupt flag? identical om mcp00a,b and mcp01a
    bool intf = theHardware.mcp[0][0].readBit(Mcp23017::Register::INTFA, 6);
    if (!intf) {
        // theHardware.pinaux[0].setVal(false) ;
        return false;
    }
    // mount UNIBUS address and control
    // read & clear INT conditions
    // MSYN may be inactive or still active. first INTCAPA rearms INT
    uint8_t intcap00a = theHardware.mcp[0][0].read(Mcp23017::Register::INTCAPA);
    uint8_t intcap00b = theHardware.mcp[0][0].read(Mcp23017::Register::INTCAPB);
    uint8_t intcap01a = theHardware.mcp[0][1].read(Mcp23017::Register::INTCAPA);
    uint8_t intcap01b = theHardware.mcp[0][1].read(Mcp23017::Register::INTCAPB);

    // reg2 = self.mcp23017_1.read(INTCAPA) # clear INTA first
    // reg1 = self.mcp23017_0.read(INTCAPB)
    // reg0 = self.mcp23017_0.read(INTCAPA) # else 2nd INT
    bool curMSYN = readMSYN(); // current state, not the captured one
    // new INT is fired immediately if MSYN long-active (compare against "inactive")
    // the_logger.print("duplicate_MSYN_INT=" + str(self.duplicate_MSYN_INT))
    if (duplicate_MSYN_INT) {
        duplicate_MSYN_INT = curMSYN;
        // theHardware.pinaux[0].setVal(false) ;// toggle = duplicates filtered
        // theHardware.pinaux[0].setVal(true) ;
        // theHardware.pinaux[0].setVal(false) ;
        return false;
    }
    *addr = readADDR(intcap00a, intcap00b, intcap01a);
    *c1c0 = readC1C0(intcap01b);
    // sense current MSYN: if still active, this INT must be ignored
    duplicate_MSYN_INT = curMSYN;
    // theHardware.pinaux[0].setVal(false) ;
    return true;
}

// AUX1
bool Unibus::getSSYNcaptured(uint16_t *data) {
    // theHardware.pinaux[1].setVal(true) ;
    //  update UNIBUS_DATA, UNIBUS_NXM when MSYN_L 1->0 or NXM 1->0 occured
    //  SSYN interrupt flag? identical om mcp02a,b and mcp03a
    bool intf = theHardware.mcp[0][2].readBit(Mcp23017::Register::INTFA, 6);
    if (!intf) {
        // theHardware.pinaux[1].setVal(false) ;
        return false;
    }
    // mount UNIBUS DATA
    // read & clear INT conditions
    uint8_t intcap02a = theHardware.mcp[0][2].read(Mcp23017::Register::INTCAPA);
    uint8_t intcap02b = theHardware.mcp[0][2].read(Mcp23017::Register::INTCAPB);
    uint8_t intcap03a = theHardware.mcp[0][3].read(Mcp23017::Register::INTCAPA);

    // reg2 = self.mcp23017_3.read(INTCAPA) # clear INTA first
    // reg1 = self.mcp23017_2.read(INTCAPB)
    // reg0 = self.mcp23017_2.read(INTCAPA) # else 2nd INT
    bool curSSYN = readSSYN(); // current state, not the captured one
    if (duplicate_SSYN_INT) {  // see MSYN above
        duplicate_SSYN_INT = curSSYN;
        // theHardware.pinaux[1].setVal(false) ; // flicker to indicate "duplicate filtered"
        // theHardware.pinaux[1].setVal(true) ;
        // theHardware.pinaux[1].setVal(false) ;
        return false;
    }
    *data = readDATA(intcap02a, intcap02b, intcap03a);
    duplicate_SSYN_INT = curSSYN; // see MSYN above
                                  // theHardware.pinaux[1].setVal(false) ;
    return true;
}
