/* probeinterface - abstract connector to a target Pdp11, via message streams
Derived classes:
    Communication to physical M93X2probe vie "messageinterfaceserial"
    Simulator must implement this too
*/

#include "logger.h"
#include "messageinterface.h"
#include "pdp11adapter.h"

bool MessageInterface::connect(std::string endpoint) {
    UNREFERENCED_PARAMETER(endpoint);
    wxLogFatalError("Abstract MessageInterface::connect() called");
    return false;
}

void MessageInterface::disconnect() {
    wxLogFatalError("Abstract MessageInterface::disconnect() called");

}

// add chunk of text to ringBuffer and process completed Messages
void MessageInterface::appendDataAndProcessResponses(std::string buffer) {
    // wxLogDebug("RxD #%d	\"%s\"", byteCount, buffer);
    // add chunk to ring buffer
    receiveRingBuffer.append(buffer);
    // extract leading lines at \n and \r and ;
    size_t separatorPos = receiveRingBuffer.find_first_of(Message::separatorChars);
    while (separatorPos != std::string::npos) {
        // extract and process all accumolated messages
        std::string line = receiveRingBuffer.substr(0, separatorPos); // strip \n
        receiveRingBuffer.erase(0, separatorPos + 1); // remove from buffer, discard \n
        separatorPos = receiveRingBuffer.find_first_of(Message::separatorChars); // find next
        if (line != "") {
            // process in pdp11adapter, delete there
            Message* msg = Message::parse(line.c_str());
            if (msg != nullptr) {
                //			 wxLogInfo("RxD \"%s\"", msg->render());
                rcvMessageCount++;
                msg->process();
                delete msg; // is consumed now
            }
        }
    }
}

void MessageInterface::receiveAndProcessResponses()
{
    wxLogFatalError("Abstract MessageInterface::receiveAndProcessResponses() called");
}

void MessageInterface::xmtRequest(Message* msg) {
    UNREFERENCED_PARAMETER(msg);
    wxLogFatalError("Abstract MessageInterface::xmtRequest() called");
}


// implement generic process() forward for each message type


// forward messages on receive. not used
void* ResponseOK::process() { return nullptr; }
void* ResponseError::process() { return nullptr; }

// show in caption
void* ResponseVersion::process() { 
    wxGetApp().pdp11Adapter->onResponseVersion(this);
	return nullptr; 
}


void* ResponseUnibusSignals::process() {
    // forward to pdp11adapter for display
    wxGetApp().pdp11Adapter->onResponseUnibusSignals(this);
    return nullptr;

}

void* ResponseKY11LBSignals::process() {
    // forward to pdp11adapter for display
    wxGetApp().pdp11Adapter->onResponseKY11LBSignals(this);
    return nullptr;
}
void* ResponseKM11Signals::process() {
    // forward to pdp11adapter for display
    wxGetApp().pdp11Adapter->onResponseKM11Signals(this);
    return nullptr;
}

void* ResponseUnibusCycle::process() {
    // forward to pdp11adapter for display
    wxGetApp().pdp11Adapter->evalUnibusCycle(this);
    return nullptr;
}

void* ResponseRegDef::process() { 
    wxGetApp().pdp11Adapter->evalResponseRegisterDefinition(this);
    return nullptr; 
    // todo: enable panel with "internal state", paint list of variables
}


void* ResponseRegVals::process() { 
		wxGetApp().pdp11Adapter->evalResponseRegisterValues(this);
		return nullptr; 
		return nullptr; 
    // todo: update panel with "internal state"
}

// not requested, no responses
void* ResponseSwitches::process() { return nullptr; }
void* ResponseMcp23017Registers::process() { return nullptr; }

// requests are sent to target PDP11, processed on
// physical PDP11s by M93X2probe or similar,
// or a simulated PDP11
void* RequestReset::process() { return nullptr; }
void* RequestUnibusSignalsRead::process() { return nullptr; }
void* RequestUnibusSignalWrite::process() { return nullptr; }
void* RequestUnibusExam::process() { return nullptr; }
void* RequestUnibusDeposit::process() { return nullptr; }
void* RequestUnibusSize::process() { return nullptr; }
void* RequestUnibusTest::process() { return nullptr; }
void* RequestKY11LBSignalsRead::process() { return nullptr; }
void* RequestKY11LBSignalWrite::process() { return nullptr; }
void* RequestKM11SignalsRead::process() { return nullptr; }
void* RequestKM11SignalsWrite::process() { return nullptr; }
void* RequestMcp23017RegistersRead::process() { return nullptr; }
void* RequestLedWrite::process() { return nullptr; }
void* RequestSwitchesRead::process() { return nullptr; }
void* RequestVersionRead::process() { return nullptr; }
void* RequestBoot::process() { return nullptr; }
void* RequestRegDef::process() { return nullptr; }
void* RequestRegVal::process() { return nullptr; }
