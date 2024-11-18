/* messages.h - definition of message records between M93X2 PCB and host PC

!!! To be used on Arduino, so
- no std:: lib, no std::string
- no typeid
- no RTI, no dynamic_cast

Caller (Arduino or PC host ) must execute code depending on result of Message::parse()
This is done by implementing Message::process()
Differently on Arduino or PC host, in a separate Module C++ file.
Typically, Arduino side REquest*::execute() cotnain code, where
    Response*::process() is {}.
On PC host its the other way round

!!! define process() for all Message Subclasses, else
!!! else strange errors like "<artificial>:(.text+0x1970): undefined reference to `vtable for ResponseError'"

Memory allocation:
The Arduino has no RAM (4K). So only small buffers can be used,
the big "State" Message is neither needed on the M93x2probe, nor can it be implmented
The MACRO PLATFORM_ARDUINO controls buffer size and implemented messages

*/
#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

// Compatibility Linux-Visual Studio
#ifdef _MSC_VER
#ifndef strcasecmp
#define strcasecmp _stricmp	// grmbl
#endif
#pragma warning(disable:4996)   // no strcpy_s on Arduino?
#endif

// base class for Requests and Responses from Host PC to PCB
class Message {
    /// class static parser/render  helper
public:
    // limits for fields and sizes

    // message borders in parsed textstreams. CR, LF and ';'
    static const char* separatorChars; // "\n\r;";

    // class helper for parsing and rendering.
    // define the static vars in the .cpp for the linker
    static const int maxTokenCount = 36; // csvlists max 32
    static int tokenCount;
    static char* token[maxTokenCount]; // args to opcode
    // split into token

    // compare argv[i] with value
    static void tokenize(const char* line);
    static bool isToken(int tokenIdx, const char* val);
    static void buildSyntaxError(const char* syntax, const char* msg);

    // eval parsed tokenlist.
    // argTokenIdx = first token after opcodes
    // argStringIdx = first char of argument string
    // string = "OK error fixed" -> ResponseOK,
    //				argTokenIdx=1
    //				argString = "error fixed"
    // returns nullptr on success or error message
    // one of those both is used by messages to parse the commandline params
    virtual const char* initFromArgString(const char* argString) ;
    //virtual const char* initFromArgString(const char* argString) { return nullptr;}
    virtual const char* initFromArgToken(int startTokenIdx) ;
    // virtual const char* initFromArgToken(int startTokenIdx) {return nullptr;}

    // fix class buffer len for all purposes, higly insecure!
    // caller must allocate via Messages::bufferINit()
#if defined(PLATFORM_ARDUINO)
    static const int txtBufferSize = 120;
#else
    static const int txtBufferSize = 1050; // 1k payload plus some header space
#endif
    static char txtBuffer[txtBufferSize];
    // fix class error buffer
    static const int errorBufferSize = 120;
    static char errorBuffer[errorBufferSize];

    static uint8_t parseUniBusCycleArg0();
    static Message* parse(const char* buffer);  // factory, return one subclass

    // instances
public:
    virtual ~Message() = default; // now delete msg works polymorphic

    const char* syntaxInfo = nullptr; // helper text with command syntax

    virtual const char* render() {
        return nullptr;    // sub class must implement
    }
    //assert at least ONE virtual methode here which is not pure = 0

    // process some and optionally produce a response
    virtual void* process() {
        return nullptr;
    }

	// data consistency check
	virtual bool isValid() {
		return true ;
	}
};

// just "OK", or optional OK <msg>
class ResponseOK : public Message {
public:
    static const int bufflen = 80;
    char msg[bufflen];
    ResponseOK() {
        msg[0] = 0; // no message
    }
    ResponseOK(const char* _fmt, ...) {
        va_list argptr;
        va_start(argptr, _fmt);
        vsnprintf(msg, bufflen, _fmt, argptr);
        msg[bufflen - 1] = 0; // terminate on trunc
    }
    const char *initFromArgString(const char *argString) override ;

    const char* render() override {
        if (strlen(msg))
            snprintf(txtBuffer, txtBufferSize, "OK %s", msg);
        else
            strcpy(txtBuffer, "OK");
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// ERROR <string>
class ResponseError : public Message {
public:
    static const int bufflen = 80;
    char errormsg[bufflen];
    ResponseError() {
        errormsg[0] = 0; // no message
    }
    ResponseError(const char* _fmt, ...) {
        va_list argptr;
        va_start(argptr, _fmt);
        vsnprintf(errormsg, bufflen, _fmt, argptr);
        errormsg[bufflen - 1] = 0; // terminate on trunc
    }
    const char *initFromArgString(const char *argString) override ;
    const char* render() override {
        if (strlen(errormsg))
            snprintf(txtBuffer, txtBufferSize, "ERROR %s", errormsg);
        else
            strcpy(txtBuffer, "ERROR");
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

class RequestReset : public Message {
    const char* render() override {
        return "RESET";
    }
    const char *initFromArgToken(int startTokenIdx) override ;
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};


// electrical state of UNIBUS
class UnibusSignals {
public:
    uint32_t addr = 0;
    uint16_t data = 0;
    uint8_t c1c0 = 0;
    uint8_t msyn = 0;
    uint8_t ssyn = 0;
    uint8_t pbpa = 0;
    uint8_t intr = 0;
    uint8_t br74 = 0;
    uint8_t bg74 = 0;
    uint8_t npr = 0;
    uint8_t npg = 0;
    uint8_t sack = 0;
    uint8_t bbsy = 0;
    uint8_t init = 0;
    uint8_t aclo = 0;
    uint8_t dclo = 0;
    UnibusSignals() {}
    UnibusSignals(uint32_t _addr, uint16_t _data, uint8_t _c1c0, uint8_t _msyn, uint8_t _ssyn,
                  uint8_t _pbpa, uint8_t _intr, uint8_t _br74, uint8_t _bg74, uint8_t _npr, uint8_t _npg, uint8_t _sack, uint8_t _bbsy,
                  uint8_t _init, uint8_t _aclo, uint8_t _dclo) :
        addr(_addr), data(_data), c1c0(_c1c0), msyn(_msyn), ssyn(_ssyn),
        pbpa(_pbpa), intr(_intr), br74(_br74), bg74(_bg74), npr(_npr), npg(_npg), sack(_sack), bbsy(_bbsy),
        init(_init), aclo(_aclo), dclo(_dclo) {}
    void setSignal(const char* signalName, uint32_t val);

} ;

class RequestUnibusSignalsRead : public Message {
    const char* render() override {
        return "R US";
    }
    const char *initFromArgToken(int startTokenIdx) override ;
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};


class ResponseUnibusSignals : public Message {
public:
    UnibusSignals	signals ;
    ResponseUnibusSignals() {} // all fields default to 0
    ResponseUnibusSignals(UnibusSignals _signals): signals(_signals) {} // all fields default to 0
    ResponseUnibusSignals(uint32_t _addr, uint16_t _data, uint8_t _c1c0, uint8_t _msyn, uint8_t _ssyn,
                          uint8_t _pbpa, uint8_t _intr, uint8_t _br74, uint8_t _bg74, uint8_t _npr, uint8_t _npg, uint8_t _sack, uint8_t _bbsy,
                          uint8_t _init, uint8_t _aclo, uint8_t _dclo) :
        signals(_addr, _data, _c1c0, _msyn, _ssyn,
                _pbpa, _intr, _br74, _bg74, _npr, _npg, _sack, _bbsy,
                _init, _aclo, _dclo) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize,
                 "US %05lx %04x %d %d %d %d" 			 //
                 " %d %x %x %d %d %d %d"				 //
                 " %d %d %d",							 //
                 (unsigned long)signals.addr, signals.data, signals.c1c0, signals.msyn, signals.ssyn, signals.pbpa,	 //
                 signals.intr, signals.br74, signals.bg74, signals.npr, signals.npg, signals.sack, signals.bbsy, //
                 signals.init, signals.aclo, signals.dclo						 //
                );
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

class RequestUnibusSignalWrite : public Message {
public:
    char signalName[10];
    uint32_t val;
    RequestUnibusSignalWrite() {}
    RequestUnibusSignalWrite(const char* _signalName, uint32_t _val) {
        strcpy(signalName, _signalName);
        val = _val;
    }
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "W US %s %x", signalName, val);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

class RequestKY11LBSignalsRead : public Message {
public:
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        return "R Y";
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// response: hex digit string: <microPC> <FP11> <SBF> <LIR> <PBP>
class ResponseKY11LBSignals : public Message {
public:
    uint16_t mpc = 0;
    uint8_t fp11 = 0;
    uint8_t sbf = 0;
    uint8_t lir = 0;
    uint8_t pbp = 0;
    ResponseKY11LBSignals() {}
    ResponseKY11LBSignals(uint16_t _mpc, uint8_t _fp11, uint8_t _sbf, uint8_t _lir, uint8_t _pbp) :
        mpc(_mpc), fp11(_fp11), sbf(_sbf), lir(_lir), pbp(_pbp) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "Y %x %d %d %d %d", mpc, fp11, sbf, lir, pbp);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

class RequestKY11LBSignalWrite : public Message {
public:
    char signalName[10]; // "MC", "MCE"
    uint16_t val; // 0, 1, 2 = P)ulse
    RequestKY11LBSignalWrite() {}
    RequestKY11LBSignalWrite(const char* _signalName, uint16_t _val) {
        strcpy(signalName, _signalName);
        val = _val;
    }
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        if (val <= 1)
            snprintf(txtBuffer, txtBufferSize, "W Y %s %x", signalName, val);
        else
            snprintf(txtBuffer, txtBufferSize, "W Y %s P", signalName);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

class RequestKM11SignalsRead : public Message {
public:
    char	channel;  // A, B
    RequestKM11SignalsRead() {}
    RequestKM11SignalsRead(char _channel) : channel(_channel) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "R M %c", channel);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// 28 inputs of KM11 A or B
class ResponseKM11Signals : public Message {
public:
    char	channel;  // 'A' or 'B'
    uint8_t gpio0a = 0, gpio0b = 0, gpio1a = 0, gpio1b = 0;
    // for A and B each 28 signals from 2 MSCP23017
    ResponseKM11Signals() {}
    ResponseKM11Signals(char _channel, uint8_t _gpio0a, uint8_t _gpio0b, uint8_t _gpio1a, uint8_t _gpio1b) :
        channel(_channel), gpio0a(_gpio0a), gpio0b(_gpio0b), gpio1a(_gpio1a), gpio1b(_gpio1b) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "M %c %x %x %x %x", channel, gpio0a, gpio0b, gpio1a, gpio1b);
        return txtBuffer;
    }

#if !defined(PLATFORM_ARDUINO)
	// GUI packs/unpacks GPIO registers values to/from FlipChip pin signals
    bool K2,N1,V1,P2,J1,C1,T2 ; // gpio0a
	bool U2,P1,L1,F1,D1,R1,K1 ; // gpio0b
	bool S2,M1,S1,M2,F2,E1,N2 ; // gpio1a
    bool D2,H2,J2,R2,E2,H1,L2 ; // gpio1b
    void getFlipchipSignalsFromGpioVals();
	void setGpioValsFromFlipchipSignals() ;
#endif
	
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// Set 4 outputs of KM11A or B
class RequestKM11SignalsWrite : public Message {
public:
    char	channel;  // A, B
    uint8_t val03; // out11, out10, out 01, out00
    RequestKM11SignalsWrite() {}
    RequestKM11SignalsWrite(char _channel, uint8_t _val03) :
        channel(_channel), val03(_val03) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "W M %c %x", channel, val03);
        return txtBuffer;
    }
#if !defined(PLATFORM_ARDUINO)
    bool A1,B2,U1,V2;
	void setVal03FromFlipchipSignals() ;
	void getFlipchipSignalsFromVal03() ;
#endif

	
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// raw state of a MCP23017 GPIO register on M93X2 PCB
class RequestMcp23017RegistersRead : public Message {
public:
    uint8_t	group; // address group
    uint8_t	addr; // I2C address within group
    char	half;  // 'A' or 'B' register half
    RequestMcp23017RegistersRead() {}
    RequestMcp23017RegistersRead(uint8_t _group, uint8_t _addr, char _half) :
        group(_group), addr(_addr), half(_half) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "R MCP %d %d %c", group, addr, half);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

class ResponseMcp23017Registers : public Message {
public:
    uint8_t	group = 0; // address group
    uint8_t	addr = 0; // I2C address within group
    char	half = 'A';  // 'A' or 'B' register half

    uint8_t iodir = 0, ipol = 0, gpinten = 0, defval = 0, // register of MCP23017 chip
            intcon = 0, iocon = 0, gppu = 0, intf = 0,
            intcap = 0, gpio = 0, olat = 0;
    ResponseMcp23017Registers() {}
    ResponseMcp23017Registers(uint8_t _group, uint8_t _addr, char _half,
                              uint8_t _iodir, uint8_t _ipol, uint8_t _gpinten, uint8_t _defval,
                              uint8_t _intcon, uint8_t _iocon, uint8_t _gppu, uint8_t _intf,
                              uint8_t _intcap, uint8_t _gpio, uint8_t _olat) :
        group(_group), addr(_addr), half(_half),
        iodir(_iodir), ipol(_ipol), gpinten(_gpinten), defval(_defval),
        intcon(_intcon), iocon(_iocon), gppu(_gppu), intf(_intf),
        intcap(_intcap), gpio(_gpio), olat(_olat) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        const char* fmt = "MCP%d%d%c: IODIR=%02x IPOL=%02x GPINTEN=%02x DEFVAL=%02x INTCON=%02x IOCON=%02x GPPU=%02x INTF=%02x INTCAP=%02x GPIO=%02x OLAT=%02x";
        snprintf(txtBuffer, txtBufferSize, fmt, (int)group, (int)addr,
                 half, iodir, ipol, gpinten, defval, intcon, iocon, gppu, intf, intcap, gpio, olat);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};



// write 2 LEDs, with on- end off period blinking
// set ON and OFF times in milli seconds
// always on: on=1,off=0
// always off: on=0,off=1
// freeze current statte: on=0, off=0
class RequestLedWrite : public Message {
public:
    uint8_t unit;  // 0..1
    uint32_t	off_period_millis;
    uint32_t	on_period_millis;
    RequestLedWrite() {}
    RequestLedWrite(uint8_t _unit, uint32_t _off_period_millis, uint32_t _on_period_millis) :
        unit(_unit), off_period_millis(_off_period_millis), on_period_millis(_on_period_millis) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "W LED %d %lu %lu", unit, (long unsigned) off_period_millis, (long unsigned) on_period_millis);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};


// R SW	  read option switch state as hex string
class RequestSwitchesRead : public Message {
public:
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        return "R SW";
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

//	SW val
class ResponseSwitches : public Message {
public:
    uint8_t val05 = 0;  // 6bits
    ResponseSwitches() {}
    ResponseSwitches(uint8_t _val05) : val05(_val05) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "SW %x", val05);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// R VERSION	  read versionstring
class RequestVersionRead : public Message {
public:
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        return "R VERSION";
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

//	VERSION <string>
class ResponseVersion : public Message {
public:
    static const int bufflen = 80;
    char version[bufflen];
    ResponseVersion() {}
    ResponseVersion(const char* _fmt, ...) {
        va_list argptr;
        va_start(argptr, _fmt);
        vsnprintf(version, bufflen, _fmt, argptr);
        version[bufflen - 1] = 0; // terminate on trunc
    }
    const char *initFromArgString(const char *argString) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "VERSION %s", version);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// E <addr> 	   Exam UNIBUS octal address with DATI
// addr octal
// response DATI addr data or NXM
class RequestUnibusExam : public Message {
public:
    uint32_t addr;
    RequestUnibusExam() {}
    RequestUnibusExam(uint32_t _addr) : addr(_addr) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "E %lo", (long unsigned) addr);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// DATI|DATIP|DATO|DATOB <addr> <data>|NXM
// addr,data octal
class ResponseUnibusCycle : public Message {
public:
    static const char* const cycleText[4]; // actual strings goto *.cpp
    uint8_t c1c0; //0=dati, 1= datip, 2=dato, 3=datob
    uint32_t addr;
    uint16_t data;
    bool nxm = false;
    uint8_t requested; // 1 = answer to request. 0 = asynchronuos capture '!'
    ResponseUnibusCycle() {}
    ResponseUnibusCycle(uint8_t _c1c0, uint32_t _addr, uint16_t _data, bool _nxm, bool _requested) :
        c1c0(_c1c0), addr(_addr), data(_data), nxm(_nxm), requested(_requested) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        if (nxm)
            snprintf(txtBuffer, txtBufferSize - 2, "%s %lo NXM", cycleText[c1c0], (unsigned long) addr);
        else
            snprintf(txtBuffer, txtBufferSize - 2, "%s %lo %o", cycleText[c1c0], (unsigned long) addr, data);
        if (!requested)
            strcat(txtBuffer, " !");
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
    bool isValid() override ;
};

// D <addr> <data>  DEPOSIT octal val to UNIBUS octal address with DATO (only even addr)
// addr,data octal
// response DATO addr data or NXM
class RequestUnibusDeposit : public Message {
public:
    uint32_t addr;
    uint16_t data;
    RequestUnibusDeposit() {}
    RequestUnibusDeposit(uint32_t _addr, uint16_t _data) : addr(_addr), data(_data) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "D %lo %o", (unsigned long) addr, data);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// UBSIZE		scan UNIBUS address space,
// response: > DATI <first octal NXM address> NXM
class RequestUnibusSize : public Message {
public:
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        return "UBSIZE";
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// UBTEST		write, read and compare full UNIBUS address range
// response: OK or Status error
class RequestUnibusTest : public Message {
public:
    uint32_t seed;
    RequestUnibusTest() {}
    RequestUnibusTest(uint32_t _seed) : seed(_seed) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "UBTEST %x", seed);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// BOOT <fileNr>    stop PDP11 CPU, download code file <n>, set power-on vector 24/26, restart CPU via ACLO/DCLO
// response: > DATO <last download address / first NXM address> NXM
class RequestBoot : public Message {
public:
    uint16_t fileNr ;
    RequestBoot() {}
    RequestBoot(uint16_t _fileNr) : fileNr(_fileNr) {}
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "BOOT %ld", (unsigned long) fileNr);
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

#if !defined(PLATFORM_ARDUINO)

#include <vector>
// only to be implemented on host PC and remote emulator

// needs variable directory
class MessagesStateVar {
public:
    static const int nameSize = 32 ;
    char name[nameSize+1] ; // ResponseStateDef: short name, like "R0,PC,BA".
    int		bitCount = 0 ; // ResponseStateDef: size in bits, > 1
    // !! integer width deri
    void	*object = nullptr  ; // data source in producing emulator
    // displaying control in GUI
    int	objectSizeof=0 ; // size of pointer in byte: uint8=1, uint16=2, uint32=4
    // objectSizeof only needed intern in emulator, neither rendered nor parsed
    uint32_t value = 0 ; // RequestStateVal
    MessagesStateVar() {
        name[0] = 0;
        bitCount = 0 ;
        object = nullptr ;
        value = 0;
    }
} ;


// R STATEDEF
// request definition of internal registers, used for emulators which deliver more data
// response: STATEDEF
class RequestStateDef : public Message {
public:
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "R STATEDEF");
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// parsing instantiates the name and size fields of stateVars
// text representation: "name:size name:bitcount ....", bitcount decimal
// stateVars: name and bitwidth filled
class ResponseStateDef : public Message {
public:
    std::vector<MessagesStateVar> stateVars;
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        strcpy(txtBuffer, "STATEDEF");
        for (auto it = stateVars.begin() ; it != stateVars.end() ; it++) {
            if (strlen(txtBuffer) +  strlen(it->name) + 5 > txtBufferSize)
                return "ERROR BUFFER OVERFLOW" ;
            strcat(txtBuffer, " ") ; // mount " name:size"
            strcat(txtBuffer, it->name) ;
            strcat(txtBuffer, ":") ;
            char numBuffer[20] ;
            sprintf(numBuffer, "%d", it->bitCount) ;  // decimal
            strcat(txtBuffer, numBuffer) ;
        }
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// R STATEVAL
// request values of internal registers, used for emulators which deliver more data
// response: STATEVAL
class RequestStateVal : public Message {
public:
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        snprintf(txtBuffer, txtBufferSize, "R STATEVAL");
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

// data of internal registers
// data delivered in stateVars vector, "value" field
// list of hex values
// stateVars: only value field filled
class ResponseStateVals : public Message {
public:
    std::vector<MessagesStateVar> stateVars;
    const char *initFromArgToken(int startTokenIdx) override ;
    const char* render() override {
        strcpy(txtBuffer, "STATEVAL");
        for (auto it = stateVars.begin() ; it != stateVars.end() ; it++) {
            if (strlen(txtBuffer) +  strlen(it->name) + 5 > txtBufferSize)
                return "ERROR BUFFER OVERFLOW" ;
            char numBuffer[20] ;
            sprintf(numBuffer, " %x", it->value) ;  // hex
            strcat(txtBuffer, numBuffer) ;
        }
        return txtBuffer;
    }
    void* process() override; // to be defined differently in M93X2 PCB and PC host
};

#endif //  !defined(PLATFORM_ARDUINO)


#endif

