/* pdp11simulator - base class for a simulator
  Implement a real simulator by deriving from class Pdp11Simulator
  Receives requests and generates responses, which are wrapped into regular methods.
  Message interface via TCP/IP.
*/
#include <cstdio>
#include <cassert>

#include "utils.h"
#include "messages.h"
#include "pdp11simulator.h"

// class var
Pdp11Simulator* Pdp11Simulator::instance;


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
void* ResponseStateDef::process() {
    return nullptr;
}
void* ResponseStateVals::process() {
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
    Pdp11Simulator::instance->respond(new ResponseVersion(Pdp11Simulator::instance->getVersion()));
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
    Pdp11Simulator::instance->onRequestUnibusSignalsRead();
    return nullptr;
}

void *RequestUnibusSignalWrite::process() {
    Pdp11Simulator::instance->onRequestUnibusSignalWrite(this);
    return nullptr;
}

// return the defintion list for internal state variables
void *RequestStateDef::process() {
	ResponseStateDef *response = new ResponseStateDef() ;
	response->stateVars = Pdp11Simulator::instance->stateVars ; //
    Pdp11Simulator::instance->respond(response);
    // will render and delete
    return nullptr;
}

// return the internal state variables, with values updated
void *RequestStateVal::process() {
	auto stateVars = &Pdp11Simulator::instance->stateVars ; // alias to emulator state
	// for all variables: eval registered pointer
	for (auto it = stateVars->begin() ; it != stateVars->end() ; it++) {
		switch(it->objectSizeof) {
		case 1: // pointer to byte var
			it->value = * ( (uint8_t *)it->object ) ;
			break ;
		case 2: // pointer to word var
			it->value = * ( (uint16_t *)it->object ) ;
			break ;
		case 4: // pointer to long var
			it->value = * ( (uint32_t *)it->object ) ;
			break ;
		default:
			assert(false); // definition error
		}
	}

	ResponseStateVals *response = new ResponseStateVals() ;
	response->stateVars = *stateVars ;
    Pdp11Simulator::instance->respond(response);
    // will render and delete
    return nullptr;
}

/****** Protoype for event methods *****/

const char* Pdp11Simulator::getVersion()
{
    fprintf(stderr, "Abstract Pdp11Simulator::getVersion() called\n");
    return nullptr;
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

void  Pdp11Simulator::onRequestUnibusSignalsRead() {
    auto response = new ResponseUnibusSignals(unibusSignals);
    respond(response);
}

void  Pdp11Simulator::onRequestUnibusSignalWrite(RequestUnibusSignalWrite* requestUnibusSignalWrite) {
    UNREFERENCED_PARAMETER(requestUnibusSignalWrite);
    fprintf(stderr, "abstract Pdp11Simulator::onRequestUnibusSignalWrite() called\n");
}


void Pdp11Simulator::setup() {
    // clear pending responses?
}

void Pdp11Simulator::loop() {
}


// CPU executed a memory access by micro code, respond this to GUI.
// Always asynchronical, never a response to a previous request
void Pdp11Simulator::onCpuUnibusCycle(uint8_t c1c0, uint32_t addr, uint16_t data, bool nxm) {
    respond(new ResponseUnibusCycle(c1c0, addr, data, nxm, /*expected*/false));
}

void Pdp11Simulator::setMicroRunState(bool state) {
    UNREFERENCED_PARAMETER(state);
    fprintf(stderr, "Abstract Pdp11Simulator::setRunState() called\n");
}
void Pdp11Simulator::microStep() {
    fprintf(stderr, "Abstract Pdp11Simulator::microStep() called\n");
}
