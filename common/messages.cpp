/* messages.cpp - definition of message records between M93X2 PCB and host PC

 See separate docs in "message.txt"

 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h" // UNREFERENCED_PARAMETER
#include "messages.h" // own stuff
#ifdef USED

// convert octal/hex/decimal string to long val.
// result: false = fail
bool parseUInt(int base, char* str, uint32_t* val) {
    char* endptr;
    *val = strtol(str, &endptr, base);
    return (*endptr == 0); // parsed to end of string?
}
#endif


bool TokenList::is(int argIdx, const char* val) {
	if (argIdx >= count)
		return false;
	return !strcasecmp(token[argIdx], val);
}

void TokenList::tokenize(const char* line) {
	strcpy(txtBuffer, line); // save input text in external persistent buffer

    // build vector of arguments
    char* curToken;
    // Keep printing tokens while one of the delimiters present in str[].
	count = 0 ;
    do {
        if (count == 0)
            curToken = strtok(txtBuffer, " ");
        else
            curToken = strtok(NULL, " ");
        if (curToken != nullptr)
            token[count++] = curToken;
    } while (count < maxCount && curToken != nullptr);
}



// Name of UNIBUS Cycle to two bits C1,C0
uint8_t TokenList::parseUniBusCycleArg0() {
    if (is(0, "DATI"))
        return 0;
    else if (is(0, "DATIP"))
        return 1;
    else if (is(0, "DATO"))
        return 2;
    else if (is(0, "DATOB"))
        return 3;
    else return 99; //invalid
}


// set a UNIBUS signal by name.
// signal name is the member name in uppercase
void UnibusSignals::setSignal(const char* signalName, uint32_t val)
{
    // fast access by name
    switch (toupper(signalName[0])) {
    case 'A':
        if (!strcasecmp(signalName, "ACLO"))
            aclo = !!val;
        else if (!strcasecmp(signalName, "ADDR"))
            addr = val & 0x3ffff; // 18 bit
        break;
    case 'B':
        if (!strcasecmp(signalName, "BBSY"))
            bbsy = !!val;
        else if (!strcasecmp(signalName, "BG74"))
            bg74 = val & 0xf;
        else if (!strcasecmp(signalName, "BR74"))
            br74 = val & 0xf;
        break;
    case 'C':
        if (!strcasecmp(signalName, "C1C0"))
            c1c0 = val & 3;
        break;
    case 'D':
        if (!strcasecmp(signalName, "DATA"))
            data = val & 0xffff; // 16 bit
        else if (!strcasecmp(signalName, "DCLO"))
            dclo = !!val;
        break;
    case 'I':
        if (!strcasecmp(signalName, "INIT"))
            init = !!val;
        else if (!strcasecmp(signalName, "INTR"))
            intr = !!val;
        break;
    case 'M':
        if (!strcasecmp(signalName, "MSYN"))
            msyn = !!val;
        break;
    case 'N':
        if (!strcasecmp(signalName, "NPG"))
            npg = !!val;
        else if (!strcasecmp(signalName, "NPR"))
            npr = !!val;
        break;
    case 'P':
        if (!strcasecmp(signalName, "PBPA"))
            pbpa = val & 3;
        break;
    case 'S':
        if (!strcasecmp(signalName, "SACK"))
            sack = !!val;
        else if (!strcasecmp(signalName, "SSYN"))
            ssyn = !!val;
        break;
    }
}


// define class static members for the linker
//const int Message::maxArgC = 20;
const char* Message::separatorChars = "\n\r;";

#if defined(PLATFORM_ARDUINO)
// statics
char Message::sharedTxtBuffer[txtBufferSize]; 
char *Message::renderTxtBuffer = Message::sharedTxtBuffer ;
#endif

// fix class error buffer
char Message::errorBuffer[errorBufferSize];

// access only if isValid()
const char* const ResponseUnibusCycle::cycleText[4] = { "DATI", "DATIP", "DATO", "DATOB" };





// syntax nullptr: <msg>  folowwed by input
// else
void Message::buildSyntaxError(TokenList *tokenList, const char* syntax, const char* msg)
{
    errorBuffer[0] = 0;
    if (syntax == nullptr) {
    }
    else {
        strcat(errorBuffer, "Syntax error on ");
        strcat(errorBuffer, syntax);
        strcat(errorBuffer, ", ");
    }
    strcat(errorBuffer, msg);
    strcat(errorBuffer, ", found \"");
    for (int i = 0; i < tokenList->count; i++) {
        if (i > 0)
            strcat(errorBuffer, " ");
        strcat(errorBuffer, tokenList->token[i]);
    }
    strcat(errorBuffer, "\"");
}

// parse string and return record of correct type
// render/parser consistency:
// Verify: message = parse() => message.render() matches buffer again



// line: volatile input text, update in background
// parseTxtBuffer: modified, persistant
Message* Message::parse(const char* line) {
#if defined(PLATFORM_ARDUINO)
	TokenList tokenList(Message::sharedTxtBuffer) ;
#else
	char parseTxtBuffer[txtBufferSize] ; // thread save, local stack
	TokenList tokenList(parseTxtBuffer) ;
#endif
    const char* curSyntaxInfo = "";
    const char* errormsg = "";
    Message* result = nullptr;
    uint8_t _c1c0;

    strcpy(errorBuffer, "");
    if (strlen(line) >= txtBufferSize) {
        strcpy(errorBuffer, "Message::parse() input overflow");
        return nullptr;
    }
    // first char = '#' -> simple text
    tokenList.tokenize(line);

    if (tokenList.count < 1) {
        errormsg = "No input";
        goto syntaxError;
    }

    // just "OK"
    if (tokenList.is(0, "OK")) {
        curSyntaxInfo = "OK [<string>]";
        result = new ResponseOK();
        // strip off prefix "OK " from raw input
        errormsg = result->initFromArgString(&tokenList, &(line[3])) ;
    }
    else if (tokenList.is(0, "ERROR")) {
        curSyntaxInfo = "ERROR [<string>]";
        result = new ResponseError();
        // strip off prefix "ERROR " from raw input
        errormsg = result->initFromArgString(&tokenList, &(line[6])) ;
    }
    else if (tokenList.is(0, "RESET")) {
        curSyntaxInfo = "RESET";
        result = new RequestReset();
        errormsg = result->initFromArgToken(&tokenList, 1);


    }
    else if (tokenList.is(0, "R") && tokenList.is(1, "US")) {
        curSyntaxInfo = "R US";
        result = new RequestUnibusSignalsRead();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "US")) {
        curSyntaxInfo = "US <addr> <data> <c1c0> <msyn> <ssyn> <pbba> <intr> <br74> <bg74> <npr> <npg> <sack> <bbsy> <init> <aclo> <dclo>";
        result = new ResponseUnibusSignals();
        errormsg = result->initFromArgToken(&tokenList, 1);
    }
    else if (tokenList.is(0, "W") && tokenList.is(1, "US")) {
        curSyntaxInfo = "W US <signal> <val>";
        result = new RequestUnibusSignalWrite();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "R") && tokenList.is(1, "Y")) {
        curSyntaxInfo = "R Y";
        result = new RequestKY11LBSignalsRead();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "Y")) {
        curSyntaxInfo = "Y <mpc> <fp11> <sbf> <lir> <pbp>";
        result = new ResponseKY11LBSignals();
        errormsg = result->initFromArgToken(&tokenList, 1);
    }
    else if (tokenList.is(0, "W") && tokenList.is(1, "Y")) {
        curSyntaxInfo = "W Y <signal=MC,MCE> <val=0,1,P>";
        result = new RequestKY11LBSignalWrite();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "R") && tokenList.is(1, "M")) {
        curSyntaxInfo = "R M A|B";
        result = new RequestKM11SignalsRead();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "M")) {
        curSyntaxInfo = "M A|B <mscp30|32.gpioa> <mscp30|32.gpiob> <mscp31|33.gpioa> <mscp31|33.gpiob>";
        result = new ResponseKM11Signals();
        errormsg = result->initFromArgToken(&tokenList, 1);
        // response: >hex byte string, encodes <mscp30.gpioa> <mscp30.gpiob> <mscp31.gpioa> <mscp31.gpiob>
    }
    else if (tokenList.is(0, "W") && tokenList.is(1, "M")) {
        curSyntaxInfo = "W M A|B <val0-3>";
        result = new RequestKM11SignalsWrite();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "W") && tokenList.is(1, "LED")) {
        curSyntaxInfo = "W LED <unit> <off_period_millis> <on_period_millis>";
        result = new RequestLedWrite();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "R") && tokenList.is(1, "SW")) {
        curSyntaxInfo = "R SW";
        result = new RequestLedWrite();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "SW")) {
        curSyntaxInfo = "SW <val0-5>";
        result = new ResponseSwitches();
        errormsg = result->initFromArgToken(&tokenList, 1);
    }
    else if (tokenList.is(0, "R") && tokenList.is(1, "MCP")) {
        curSyntaxInfo = "R MCP <group> <addr> <half>";
        result = new RequestMcp23017RegistersRead();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (toupper(tokenList.token[0][0]) == 'M' && toupper(tokenList.token[0][1]) == 'C' && toupper(tokenList.token[0][2]) == 'P') {
        // hope the PC host never tries to read this
        curSyntaxInfo = "MCP<group><addr><half> IODIR=<iodir> IPOL=<ipol> GPINTEN=<gpinten> DEFVAL=<defval> INTCON=<intcon> IOCON=<iocon> GPPU=<gppu> INTF=<intf> INTCAP=<intcap> GPIO=<gpio> OLAT=<olat>";
        result = new ResponseMcp23017Registers();
        errormsg = result->initFromArgToken(&tokenList, 0); // params also in opcode
    }
    else if (tokenList.is(0, "R") && tokenList.is(1, "VERSION")) {
        curSyntaxInfo = "R VERSION";
        result = new RequestVersionRead();
        errormsg = result->initFromArgToken(&tokenList, 2);
    }
    else if (tokenList.is(0, "VERSION")) {
        //	VERSION <string>
        curSyntaxInfo = "VERSION <string>";
        result = new ResponseVersion();
        errormsg = result->initFromArgString(&tokenList, &(line[7])) ; // strip off "version "
    }
    else if (tokenList.is(0, "E")) {
        curSyntaxInfo = "E <addr>";
        result = new RequestUnibusExam();
        errormsg = result->initFromArgToken(&tokenList, 1) ;
    }
    else if ((_c1c0 = tokenList.parseUniBusCycleArg0()) < 4) {
        curSyntaxInfo = "DATI|DATO|DATOB <addr> <data>|NXM [!]";
        result = new ResponseUnibusCycle();
        errormsg = result->initFromArgToken(&tokenList, 0) ; // opcode = 1st arg
    }
    else if (tokenList.is(0, "D")) {
        curSyntaxInfo = "D <addr> <data>";
        result = new RequestUnibusDeposit();
        errormsg = result->initFromArgToken(&tokenList, 1) ;
    }
    else if (tokenList.is(0, "UBSIZE")) {
        curSyntaxInfo = "UBSIZE";
        result = new RequestUnibusSize();
        errormsg = result->initFromArgToken(&tokenList, 1) ;
    }
    else if (tokenList.is(0, "UBTEST")) {
        curSyntaxInfo = "UBTEST <seed>";
        result = new RequestUnibusTest();
        errormsg = result->initFromArgToken(&tokenList, 1) ;
    }
    else if (tokenList.is(0, "BOOT")) {
        curSyntaxInfo = "BOOT 0 | <fileNr>";
        result = new RequestBoot();
        errormsg = result->initFromArgToken(&tokenList, 1) ;
    }
#if !defined(PLATFORM_ARDUINO)
    else if (tokenList.is(0, "R") && tokenList.is(1, "REGDEF")) {
        curSyntaxInfo = "R REGDEF";
        result = new RequestRegDef();
        errormsg = result->initFromArgToken(&tokenList, 2) ;
    }
    else if (tokenList.is(0, "REGDEF")) {
        curSyntaxInfo = "REGDEF <name:size> pairs";
        result = new ResponseRegDef();
        errormsg = result->initFromArgToken(&tokenList, 1) ;
    }
    else if (tokenList.is(0, "R") && tokenList.is(1, "REGVAL")) {
        curSyntaxInfo = "R REGVAL";
        result = new RequestRegVal();
        errormsg = result->initFromArgToken(&tokenList, 2) ;
    }
    else if (tokenList.is(0, "REGVAL")) {
        curSyntaxInfo = "REGVAL <value> ...";
        result = new ResponseRegVals();
        errormsg = result->initFromArgToken(&tokenList, 1) ;
    }
#endif // !defined(PLATFORM_ARDUINO)
    else {
        // unknown which message to instantiate, generic error
        curSyntaxInfo = nullptr;
        errormsg = "Illegal opcode";
        goto syntaxError;
    }
    // Success, complete resulting Message
    result->syntaxInfo = curSyntaxInfo;
    if (errormsg == nullptr)
        return result;
    // else fall through
syntaxError:
    if (result != nullptr)
        delete result ;
    buildSyntaxError(&tokenList, curSyntaxInfo, errormsg);
    return nullptr;
}

const char* Message::initFromArgString(TokenList *tokenList, const char* argString) {
    UNREFERENCED_PARAMETER(tokenList);
    UNREFERENCED_PARAMETER(argString);
    return "virtual initFromArgString()";
};

const char* Message::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    UNREFERENCED_PARAMETER(tokenList);
    UNREFERENCED_PARAMETER(startTokenIdx);
    return "virtual initFromArgToken()";
};


const char *ResponseOK::initFromArgString(TokenList *tokenList, const char *argString) {
    if (tokenList->count > 1) {
        // strip off prefix "OK " from raw input
        strcpy(msg, argString);
    }
    return nullptr ; // no error possible
}

const char *ResponseError::initFromArgString(TokenList *tokenList, const char *argString) {
    if (tokenList->count > 1) {
        // strip off prefix "ERROR " from raw input
        strcpy(errormsg, argString);
    }
    return nullptr ; // no error possible
}

const char *RequestReset::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount > 0)
        return "argC != 0"; // no args allowed
    return nullptr ; // else ok
}

const char *RequestUnibusSignalsRead::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 0)
        return "argC != 0"; // R US
    return nullptr ; // else ok
}

const char *ResponseUnibusSignals::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 16)
        return "argC != 16";
    signals.addr = strtol(tokenList->token[startTokenIdx+0], nullptr, 16);
    signals.data = (uint16_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 16);
    signals.c1c0 = (uint8_t)strtol(tokenList->token[startTokenIdx+2], nullptr, 16);
    signals.msyn = (uint8_t)strtol(tokenList->token[startTokenIdx+3], nullptr, 16);
    signals.ssyn = (uint8_t)strtol(tokenList->token[startTokenIdx+4], nullptr, 16);
    signals.pbpa = (uint8_t)strtol(tokenList->token[startTokenIdx+5], nullptr, 16);
    signals.intr = (uint8_t)strtol(tokenList->token[startTokenIdx+6], nullptr, 16);
    signals.br74 = (uint8_t)strtol(tokenList->token[startTokenIdx+7], nullptr, 16);
    signals.bg74 = (uint8_t)strtol(tokenList->token[startTokenIdx+8], nullptr, 16);
    signals.npr = (uint8_t)strtol(tokenList->token[startTokenIdx+9], nullptr, 16);
    signals.npg = (uint8_t)strtol(tokenList->token[startTokenIdx+10], nullptr, 16);
    signals.sack = (uint8_t)strtol(tokenList->token[startTokenIdx+11], nullptr, 16);
    signals.bbsy = (uint8_t)strtol(tokenList->token[startTokenIdx+12], nullptr, 16);
    signals.init = (uint8_t)strtol(tokenList->token[startTokenIdx+13], nullptr, 16);
    signals.aclo = (uint8_t)strtol(tokenList->token[startTokenIdx+14], nullptr, 16);
    signals.dclo = (uint8_t)strtol(tokenList->token[startTokenIdx+15], nullptr, 16);
    return nullptr ; // else ok
}

const char* RequestUnibusSignalWrite::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx; // argument tokens
    if (argTokenCount != 2)
        return "argC != 2";
// errormsg = "<val> not a hex" ;
    int size = sizeof(signalName)-1 ;
    strncpy(signalName, tokenList->token[startTokenIdx], size) ;
    signalName[size-1] = 0;
    val = (uint32_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 16);
// if (!parseUInt(16, argV[3], &_val))
//    goto syntaxError ;
//if (!theUnibus.writeSignal(signal, val))
//	return buildSyntaxErrorResponse(curSyntaxInfo, "<signal> <val> not writeable", argC, argV);
    return nullptr;
}

const char *RequestKY11LBSignalsRead::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 0)
        return "argC != 0";
    return nullptr ; // ok
}


const char *ResponseKY11LBSignals::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 5)
        return "argC != 5";
    mpc = (uint16_t)strtol(tokenList->token[startTokenIdx+0], nullptr, 16);
    fp11 = (uint8_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 16);
    sbf = (uint8_t)strtol(tokenList->token[startTokenIdx+2], nullptr, 16);
    lir = (uint8_t)strtol(tokenList->token[startTokenIdx+3], nullptr, 16);
    pbp = (uint8_t)strtol(tokenList->token[startTokenIdx+4], nullptr, 16);
    return nullptr ; // ok
}

const char *RequestKY11LBSignalWrite::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 2)
        return "argC != 2";
    int size = sizeof(signalName)-1 ;
    strncpy(signalName, tokenList->token[startTokenIdx], size) ;
    signalName[size-1] = 0;

    if (tokenList->is(startTokenIdx, "MCE")) { // enable = 0,1
        val = (uint16_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 16);
    }
    else if (tokenList->is(startTokenIdx, "MC")) { // clock = 0, 1, P
        if (tokenList->is(startTokenIdx+1, "P"))
            val = 2; // P
        else
            val = (uint16_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 16);
    }
    else
        return "MCE=0|1, MC=0|1|P";
    return nullptr ; // ok
}

const char *RequestKM11SignalsRead::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 1)
        return "argC != 1";
    channel = toupper(tokenList->token[startTokenIdx][0]); // first char of arg #2
    if (channel != 'A' && channel != 'B')
        return "channel must be A or B";
    return nullptr ; // ok
}

const char *ResponseKM11Signals::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 5)
        return "argC != 5";

    channel = toupper(tokenList->token[startTokenIdx][0]); // first char of arg #1
    if (channel != 'A' && channel != 'B')
        return "channel must be A or B";
    gpio0a = (uint8_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 16);
    gpio0b = (uint8_t)strtol(tokenList->token[startTokenIdx+2], nullptr, 16);
    gpio1a = (uint8_t)strtol(tokenList->token[startTokenIdx+3], nullptr, 16);
    gpio1b = (uint8_t)strtol(tokenList->token[startTokenIdx+4], nullptr, 16);
    return nullptr ; // ok
}

#if !defined(PLATFORM_ARDUINO)
void ResponseKM11Signals::getFlipchipSignalsFromGpioVals() {
    K2 = getbit(gpio0a, 0);
    N1 = getbit(gpio0a, 1);
    V1 = getbit(gpio0a, 2);
    P2 = getbit(gpio0a, 3);
    J1 = getbit(gpio0a, 4);
    C1 = getbit(gpio0a, 5);
    T2 = getbit(gpio0a, 6);

    U2 = getbit(gpio0b, 0);
    P1 = getbit(gpio0b, 1);
    L1 = getbit(gpio0b, 2);
    F1 = getbit(gpio0b, 3);
    D1 = getbit(gpio0b, 4);
    R1 = getbit(gpio0b, 5);
    K1 = getbit(gpio0b, 6);

    S2 = getbit(gpio1a, 0);
    M1 = getbit(gpio1a, 1);
    S1 = getbit(gpio1a, 2);
    M2 = getbit(gpio1a, 3);
    F2 = getbit(gpio1a, 4);
    E1 = getbit(gpio1a, 5);
    N2 = getbit(gpio1a, 6);

    D2 = getbit(gpio1b, 0);
    H2 = getbit(gpio1b, 1);
    J2 = getbit(gpio1b, 2);
    R2 = getbit(gpio1b, 3);
    E2 = getbit(gpio1b, 4);
    H1 = getbit(gpio1b, 5);
    L2 = getbit(gpio1b, 6);
}

void ResponseKM11Signals::setGpioValsFromFlipchipSignals() {
    gpio0a = setbit(K2, 0) | setbit(N1, 1) | setbit(V1, 2) | setbit(P2, 3)
             | setbit(J1, 4) | setbit(C1, 5) | setbit(T2, 6) ;
    gpio0b = setbit(U2, 0) | setbit(P1, 1) | setbit(L1, 2) | setbit(F1, 3)
             | setbit(D1, 4) | setbit(R1, 5) | setbit(K1, 6) ;
    gpio1a = setbit(S2, 0) | setbit(M1, 1) | setbit(S1, 2) | setbit(M2, 3)
             | setbit(F2, 4) | setbit(E1, 5)	| setbit(N2, 6) ;
    gpio1b = setbit(D2, 0) | setbit(H2, 1) | setbit(J2, 2) | setbit(R2, 3)
             | setbit(E2, 4) | setbit(H1, 5)	| setbit(L2, 6) ;
}

#endif

const char *RequestKM11SignalsWrite::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 2)
        return "argC != 2";

    channel = toupper(tokenList->token[startTokenIdx][0]); // first char of arg #2
    if (channel != 'A' && channel != 'B')
        return "channel must be A or B";
    val03 = (uint8_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 16);
    if (val03 > 15)
        return "<val0-3> is mask of 4 bits only";
    return nullptr ; // ok
}

const char *RequestLedWrite::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 3)
        return "argC != 3";
    unit = (uint8_t)strtol(tokenList->token[startTokenIdx], nullptr, 10);
    off_period_millis = (uint32_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 10);
    on_period_millis = (uint32_t)strtol(tokenList->token[startTokenIdx+2], nullptr, 10);
    if (unit > 1)
        return "<unit> must be 0 or 1";
    return nullptr ; // ok
}

#if !defined(PLATFORM_ARDUINO)
void RequestKM11SignalsWrite::setVal03FromFlipchipSignals() {
    val03 = setbit(A1, 0) | setbit(B2, 1) | setbit(U1, 2) | setbit(V2, 3);
}

void RequestKM11SignalsWrite::getFlipchipSignalsFromVal03() {
    A1 = getbit(val03, 0);
    B2 = getbit(val03, 1);
    U1 = getbit(val03, 2);
    V2 = getbit(val03, 3);
}

#endif

const char *RequestSwitchesRead::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 0)
        return "argC != 0";
    return nullptr ; // ok
}

const char *ResponseSwitches::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 1)
        return "argC != 1";
    val05 = (uint8_t)strtol(tokenList->token[startTokenIdx], nullptr, 16);
    return nullptr ; // ok
}

const char *RequestMcp23017RegistersRead::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 3)
        return "argC != 3";
    group = (uint8_t)strtol(tokenList->token[startTokenIdx], nullptr, 16);
    addr = (uint8_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 16);
    half = (uint8_t)toupper(tokenList->token[startTokenIdx+2][0]);  // first char of arg #5
    if (half != 'A' && half != 'B')
        return "half must be A or B";
    return nullptr ; // ok
}

const char *ResponseMcp23017Registers::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 12)
        return "argC != 12";
    // opcode is contains 3 arguments: MCP<group><addr><half>
    group = (uint8_t)(tokenList->token[startTokenIdx][3] - '0'); // 1st letter: 0..9
    addr = (uint8_t)(tokenList->token[startTokenIdx][4] - '0'); // 2nd letter : 0..9
    half = toupper(tokenList->token[startTokenIdx][5]);	// 3rd letter: A/B
    iodir = (uint8_t)strtol(tokenList->token[startTokenIdx+1] + 6, nullptr, 16); // skip "IODIR="
    ipol = (uint8_t)strtol(tokenList->token[startTokenIdx+2] + 5, nullptr, 16); // skip "IPOL="
    gpinten = (uint8_t)strtol(tokenList->token[startTokenIdx+3] + 8, nullptr, 16); // skip "GPINTEN="
    defval = (uint8_t)strtol(tokenList->token[startTokenIdx+4] + 7, nullptr, 16); // skip "DEFVAL="
    intcon = (uint8_t)strtol(tokenList->token[startTokenIdx+5] + 7, nullptr, 16); // skip "INTCON="
    iocon = (uint8_t)strtol(tokenList->token[startTokenIdx+6] + 6, nullptr, 16); // skip "IOCON="
    gppu = (uint8_t)strtol(tokenList->token[startTokenIdx+7] + 5, nullptr, 16); // skip "GPPU="
    intf = (uint8_t)strtol(tokenList->token[startTokenIdx+8] + 5, nullptr, 16); // skip "INTF="
    intcap = (uint8_t)strtol(tokenList->token[startTokenIdx+9] + 7, nullptr, 16); // skip "INTCAP="
    gpio = (uint8_t)strtol(tokenList->token[startTokenIdx+10] + 5, nullptr, 16); // skip "GPIO="
    olat = (uint8_t)strtol(tokenList->token[startTokenIdx+11] + 5, nullptr, 16); // skip "OLAT="
    return nullptr ; // ok
}

const char *RequestVersionRead::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    UNREFERENCED_PARAMETER(tokenList);
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 0)
        return "argC != 0";
    return nullptr ; // ok
}

const char *ResponseVersion::initFromArgString(TokenList *tokenList, const char *argString) {
    UNREFERENCED_PARAMETER(tokenList);
    int size = sizeof(version)-1 ;
    strncpy(version, argString, size-1);
    version[size] = 0;
    return nullptr ; // ok
}


const char *RequestUnibusExam::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    //LOG("tokencount=%d, starttokenidx=%d,argTokenCount=%d\n", tokenList->count,startTokenIdx,argTokenCount);
    if (argTokenCount != 1)
        return "argC != 1";
    addr = (uint32_t)strtol(tokenList->token[startTokenIdx], nullptr, 8);
    if (addr > 0x3ffff)
        return "<addr> 18bit octal";
    return nullptr ; // ok
}

const char *ResponseUnibusCycle::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount < 3)
        return "argC < 3";
    //1st arg: DATI,DATO,...
    c1c0 = tokenList->parseUniBusCycleArg0() ; // tokenList->token[0]
    addr = (uint32_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 8);
    if (tokenList->is(startTokenIdx+2, "NXM")) {
        data = 0;
        nxm = true;
    }
    else {
        data = (uint16_t)strtol(tokenList->token[startTokenIdx+2], nullptr, 8); ;
        nxm = false;
    }
    // last token a '!' :
    // Then response not reaction to Exam|Deposit request, but asynchronically event
    requested = !(tokenList->count == 4 && tokenList->is(3, "!"));

    return nullptr ; // ok
}

// GUI needs a mean to mark cycles as "invalid"
bool ResponseUnibusCycle::isValid() {
    if (c1c0 > 3)
        return false ;
    else if (addr > 0x3ffff) // 18 bit
        return false ;
    else return true ;
}


const char *RequestUnibusDeposit::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 2)
        return "argC != 2";
    addr = (uint32_t)strtol(tokenList->token[startTokenIdx+0], nullptr, 8);
    data = (uint16_t)strtol(tokenList->token[startTokenIdx+1], nullptr, 8);
    if (addr > 0x3ffff)
        return "<addr> 18bit octal";
    return nullptr ; // ok
}

const char *RequestUnibusSize::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 0)
        return "argC != 0";
    return nullptr ; // ok
}

const char *RequestUnibusTest::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 1)
        return "argC != 1";
    seed = (uint32_t)strtol(tokenList->token[startTokenIdx], nullptr, 16);
    return nullptr ; // ok
}

const char *RequestBoot::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 1)
        return "argC != 1";
    fileNr = (uint16_t)strtol(tokenList->token[startTokenIdx], nullptr, 16);
    return nullptr ; // ok
}

#if !defined(PLATFORM_ARDUINO)
const char *RequestRegDef::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 0)
        return "argC != 0";
    return nullptr ; // ok
}


// curSyntaxInfo = "REGDEF <name:bitcount> pairs";
// on exit: vector registers contains names and sizes
const char *ResponseRegDef::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    registers.clear() ;
    // each token is name:size
    for (int i=0 ; i < argTokenCount ; i++) {
        char *s = tokenList->token[startTokenIdx+i] ;
        char *colonPtr = strchr(s, ':') ;
        if (colonPtr == nullptr)
            return "format must be name:bitcount pairs" ;
        // manipulate parsed tokens .. nobody else uses them
        *colonPtr = 0; // terminator at : pos
        char *name = s ;
        char *bitCountStr = colonPtr+1;
        int bitCount = strtol(bitCountStr, nullptr, 10);
        if (bitCount <= 0)
            return "illegal bitcount in name:bitcount pair" ;
        // add to definition list
        Variable reg(std::string(name), "", bitCount, VariableType::none);
        registers.push_back(reg) ;
    }
    return nullptr ; // ok
}

const char *RequestRegVal::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    if (argTokenCount != 0)
        return "argC != 0";
    return nullptr ; // ok
}


// curSyntaxInfo = "REGVAL list of hex values
// fill values into newly allocated registers vector.
// delivers values in the same order as previous REGDEF
// caller must merge variable values with variable definitions
const char *ResponseRegVals::initFromArgToken(TokenList *tokenList, int startTokenIdx) {
    int argTokenCount = tokenList->count - startTokenIdx ; // argument tokens
    // each token is name:size
    for (int i=0 ; i < argTokenCount ; i++) {
        Variable reg;  // add dummy entry
        reg.setValue((uint32_t)strtol(tokenList->token[startTokenIdx+i], nullptr, 16));
        registers.push_back(reg);
    }
    return nullptr ; // ok
}





#endif // !defined(PLATFORM_ARDUINO)

