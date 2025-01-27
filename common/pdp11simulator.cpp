/* pdp11simulator - base class for a simulator
  Implement a real simulator by deriving from class Pdp11Simulator
  Receives requests and generates responses, which are wrapped into regular methods.
  Message interface via TCP/IP.
*/
#include <iostream>
#include <cstdio>
#include <cassert>

#include "utils.h"
#include "messages.h"
#include "pdp11simulator.h"

// class var
Pdp11Simulator* Pdp11Simulator::instance;



// Just a clone of std::printf().
// Later:  perhaps ungarble output with typed input,
// use separate input/output windows, or add logging.
int Pdp11Simulator::Console::printf(const char *format, ...)
{
    va_list args;  // Declare a variable argument list
    // Initialize the va_list to retrieve additional arguments
    va_start(args, format);
    // Pass the format and va_list to vprintf
    int result = vprintf(format, args);
    fflush(stdout) ;
    // Clean up the va_list when done
    va_end(args);
    return result ;
}


// thread for non-blocking readline()
void Pdp11Simulator::Console::processInput() {
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        // std::cout << "Received input: " << input << std::endl;
        if (simulator != nullptr) {
            simulator->onConsoleInputline(input);
        } else
            fprintf(stderr, "Console: no simulator connected\n") ;
    }
}


// start recieving. must connect to a simulator later, and accept commands for it
void Pdp11Simulator::Console::start()
{
    inputThread = new std::thread(&Pdp11Simulator::Console::processInput, this);
}



/****** Route Messages to handlers *****/
// must implement process() for all defined messages
// response are generated here, not processed
void* ResponseOK::process() {
    return nullptr;
}
void* ResponseError::process() {
    return nullptr;
}
void* ResponseUnibusSignals::process() {
    return nullptr;
}
void* ResponseKY11LBSignals::process() {
    return nullptr;
}
void* ResponseKM11Signals::process() {
    return nullptr;
}
void* ResponseUnibusCycle::process() {
    return nullptr;
}
void* ResponseSwitches::process() {
    return nullptr;
}
void* ResponseVersion::process() {
    return nullptr;
}
void* ResponseMcp23017Registers::process() {
    return nullptr;
}
void* ResponseRegDef::process() {
    return nullptr;
}
void* ResponseRegVals::process() {
    return nullptr;
}

// requests are sent to target PDP11, processed on
// physical PDP11s by M93023probe or similar,
// or a simulated PDP11
void* RequestReset::process() {
    return nullptr;
}

void* RequestBoot::process() {
    return nullptr;
}

//void* RequestUnibusSignalsRead::process() { return nullptr; }
//void* RequestUnibusSignalWrite::process() { return nullptr; }
//void* RequestUnibusExam::process() { return nullptr; }
//void* RequestUnibusDeposit::process() { return nullptr; }
void* RequestUnibusSize::process() {
    return nullptr;
}
void* RequestUnibusTest::process() {
    return nullptr;
}
void* RequestMcp23017RegistersRead::process() {
    return nullptr;
}
void* RequestLedWrite::process() {
    return nullptr;
}
void* RequestSwitchesRead::process() {
    return nullptr;
}

void* RequestVersionRead::process() {
    Pdp11Simulator::instance->respond(new ResponseVersion(tag, Pdp11Simulator::instance->getVersionText()));
    return nullptr;
}

void* RequestKY11LBSignalWrite::process() {
    Pdp11Simulator::instance->onRequestKY11LBSignalWrite(this);
    return nullptr;
}

void *RequestKY11LBSignalsRead::process() {
    Pdp11Simulator::instance->onRequestKY11LBSignalsRead(this);
    return nullptr;
}

void* RequestKM11SignalsWrite::process() {
    Pdp11Simulator::instance->onRequestKM11SignalsWrite(this);
    return nullptr;
}

void* RequestKM11SignalsRead::process() {
    Pdp11Simulator::instance->onRequestKM11SignalsRead(this);
    return nullptr;
}

void *RequestUnibusDeposit::process() {
    Pdp11Simulator::instance->onRequestUnibusDeposit(this) ;
    return nullptr;
}

void *RequestUnibusExam::process() {
    Pdp11Simulator::instance->onRequestUnibusExam(this);
    return nullptr;
}

void* RequestUnibusSignalsRead::process() {
    Pdp11Simulator::instance->onRequestUnibusSignalsRead(this);
    return nullptr;
}

void *RequestUnibusSignalWrite::process() {
    Pdp11Simulator::instance->onRequestUnibusSignalWrite(this);
    return nullptr;
}

// return the definition list for internal state variables
void *RequestRegDef::process() {
    ResponseRegDef *response = new ResponseRegDef(tag) ;
    response->registers = Pdp11Simulator::instance->stateVars ; //
    Pdp11Simulator::instance->respond(response);
//	fprintf(stderr, "RequestRegDef::process()\n") ;
    // will render and delete
    return nullptr;
}

// return the internal state variables, with values updated
void *RequestRegVal::process() {
    auto stateVars = &Pdp11Simulator::instance->stateVars ; // alias to emulator state
    // for all variables: eval registered pointer
    for (auto it = stateVars->begin() ; it != stateVars->end() ; it++) {
        switch(it->endpointSizeof) {
        case 1: // pointer to byte var
            it->setValue(* ( (uint8_t *)it->endpoint )) ;
            break ;
        case 2: // pointer to word var
            it->setValue(* ( (uint16_t *)it->endpoint )) ;
            break ;
        case 4: // pointer to long var
            it->setValue (* ( (uint32_t *)it->endpoint)) ;
            break ;
        default:
            fprintf(stderr, "RequestRegVal::process(): var %s ill endpointsize %d",
                    it->name.c_str(), it->endpointSizeof) ;
            assert(false); // definition error
        }
    }

    ResponseRegVals *response = new ResponseRegVals(tag) ;
    response->registers = *stateVars ;
    Pdp11Simulator::instance->respond(response);
    // will render and delete
    return nullptr;
}

/****** Protoype for event methods *****/

const char* Pdp11Simulator::getVersionText()
{
    fprintf(stderr, "Abstract Pdp11Simulator::getVersionText() called\n");
    return nullptr;
}

void Pdp11Simulator::onConsoleInputline(std::string inputLine)
{
    UNREFERENCED_PARAMETER(inputLine);
    fprintf(stderr, "Abstract Pdp11Simulator::onConsoleInputline() called\n");
}


void Pdp11Simulator::onRequestKY11LBSignalWrite(RequestKY11LBSignalWrite* requestKY11LBSignalWrite) {
    UNREFERENCED_PARAMETER(requestKY11LBSignalWrite);
    fprintf(stderr, "Abstract Pdp11Simulator::onRequestKY11LBSignalWrite() called\n");
}


void Pdp11Simulator::onRequestKY11LBSignalsRead(RequestKY11LBSignalsRead* requestKY11LBSignalsRead) {
    UNREFERENCED_PARAMETER(requestKY11LBSignalsRead);
    fprintf(stderr, "Abstract Pdp11Simulator::onRequestKY11LBSignalsRead() called\n");
}

void Pdp11Simulator::onRequestKM11SignalsWrite(RequestKM11SignalsWrite* requestKM11SignalsWrite) {
    UNREFERENCED_PARAMETER(requestKM11SignalsWrite);
    fprintf(stderr, "Abstract Pdp11Simulator::onRequestKM11SignalsWrite() called\n");
}

void Pdp11Simulator::onRequestKM11SignalsRead(RequestKM11SignalsRead* requestKM11SignalsRead) {
    UNREFERENCED_PARAMETER(requestKM11SignalsRead);
    fprintf(stderr, "Abstract Pdp11Simulator::onRequestKM11SignalsRead() called\n");
}

void  Pdp11Simulator::onRequestUnibusDeposit(RequestUnibusDeposit* requestUnibusDeposit) {
    UNREFERENCED_PARAMETER(requestUnibusDeposit);
    fprintf(stderr, "abstract Pdp11Simulator::onRequestUnibusDeposit() called\n");
}

void  Pdp11Simulator::onRequestUnibusExam(RequestUnibusExam* requestUnibusExam) {
    UNREFERENCED_PARAMETER(requestUnibusExam);
    fprintf(stderr, "abstract Pdp11Simulator::onRequestUnibusExam() called\n");
}

void  Pdp11Simulator::onRequestUnibusSignalsRead(RequestUnibusSignalsRead* requestUnibusSignalsRead) {
    auto response = new ResponseUnibusSignals(requestUnibusSignalsRead->tag, unibusSignals);
    respond(response);
}

void  Pdp11Simulator::onRequestUnibusSignalWrite(RequestUnibusSignalWrite* requestUnibusSignalWrite) {
    UNREFERENCED_PARAMETER(requestUnibusSignalWrite);
    fprintf(stderr, "abstract Pdp11Simulator::onRequestUnibusSignalWrite() called\n");
}


void Pdp11Simulator::setup() {
    // clear pending responses?
}

void Pdp11Simulator::service() {
}


// CPU executed a memory access by micro code, respond this to GUI.
// Always asynchronical, never a response to a previous request and no tag
void Pdp11Simulator::onCpuUnibusCycle(uint8_t c1c0, uint32_t addr, uint16_t data, bool nxm) {
    // When micro machine is running at own speed,
    // do not flood socket with Gigabytes of messages, which are disposed in the GUI anyhow.
    if (!microClockEnabled) {
        respond(new ResponseUnibusCycle(0, c1c0, addr, data, nxm, /*expected*/false));
    }
}

void Pdp11Simulator::setMicroClockEnable(bool state) {
    UNREFERENCED_PARAMETER(state);
    fprintf(stderr, "Abstract Pdp11Simulator::setRunState() called\n");
}
void Pdp11Simulator::microStep() {
    fprintf(stderr, "Abstract Pdp11Simulator::microStep() called\n");
}
