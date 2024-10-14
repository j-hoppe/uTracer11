/* autostepcontroller - state machine to handle stop conditions for auto single stepping

 Issues MPC clock pulses, receives asynchronically 
 MPC values and CPU UniBus Cycles,
 checks these against stop conditions.

 current logic:
 run until a certain MPC
 or
  a certain opcode fetch address is hit.
*/
#include <wx/wx.h>
#include "pdp11adapter.h"
#include "autostepcontroller.h"


void AutoStepController::changeState(State newState) {
	if (state == newState)
		return ;
	//wxLogInfo("AutoStepController state change from %d to %d", state, newState);
    //pdp11Adapter->doLogEvent("State change from %d to %d", state, newState);
	state = newState ;
}


// eval logic terms here
// currently MPC or OPcodeAddress,
// each of these may be invalid (not set by user) and is ignored then.
bool AutoStepController::stopConditionHit() {
    bool result = false;
    if (stopMpc != Pdp11Adapter::InvalidMpc && curMpc == stopMpc) {
        stopConditionText = wxString::Format("Micro PC match at %0.3o", stopMpc);
    result = true;
}
    if (stopOpcodeAddress != Pdp11Adapter::InvalidUnibusAddress
        && lastFetchUnibusCycle.c1c0 == 0 /*DATI*/
        && lastFetchUnibusCycle.addr == stopOpcodeAddress) {
        stopConditionText = wxString::Format("Opcode fetch match at %0.6o", stopOpcodeAddress);
        result = true;
    }
    return result ;
}


// setup
void AutoStepController::init(Pdp11Adapter *_pdp11Adapter, unsigned _stopMpc, uint32_t _stopOpcodeAddress) {
    pdp11Adapter = _pdp11Adapter ;
    stopMpc = _stopMpc ;
    stopOpcodeAddress = _stopOpcodeAddress ;
    stopConditionText = "Stepping ...";
    nextUnibusCycleIsFetch = false;
    changeState(State::stepMpc) ; // loop() start condition
}


// Interface to message stream:
// eval a received microprogram counter
void AutoStepController::evalUStep(unsigned mpc) {
    if (state == State::waitForMpc) {
        curMpc = mpc;
        if (mpc == pdp11Adapter->getMpcFetch()) {
            // prepare to extract opcode fetch from UNIBUS data traffic
            nextUnibusCycleIsFetch = true;
        } 
        if (stopConditionHit())
            changeState(State::conditionMatch) ; // single hit. todo: repeat count
        else changeState(State::stepMpc) ; // next mpc pulse
    }
}


// Interface to message stream:
// eval a received DATI or DATO cycle
void AutoStepController::evalUnibusCycle(ResponseUnibusCycle *cycle) {
    if (nextUnibusCycleIsFetch) {
		lastFetchUnibusCycle = *cycle;
        nextUnibusCycleIsFetch = false; // got it
        if (stopConditionHit())
            changeState(State::conditionMatch) ; // single hit. todo: repeat count
    }
}


// advance state machine
// main entry: perform a single step, wait for MPC and UNIBUIS cycle to receive
// difficult part do not issue next mpc step until all events from
// physical/simulated CPU arrived
// for example: after a fetch wait for the generated DATI cycle,
// which may cause an stop condition
void AutoStepController::service() {
    switch (state) {
    case State::stepMpc: // ready to pulse next MAN CLK
        // arm detection logic for next micro step
        pdp11Adapter->uStep() ; // generate the KY11/KM11 pulse
        curMpc = Pdp11Adapter::InvalidMpc ;
        lastFetchUnibusCycle.addr = Pdp11Adapter::InvalidUnibusAddress ;
        changeState(State::waitForMpc) ;
        break ;
    case State::waitForMpc:
        // mpc pulse issued, for MPC and perhaps DATI/DATO to receive
        // exit only by evalUStep()
		wxMilliSleep(1); // save CPU time
        break ;
    case State::conditionMatch:
        // caller should have detected via hasStopped()?
		wxMilliSleep(1); // save CPU time
        break ;

    }
}


// stop due to condition hit.
// also emergency timeout?
bool AutoStepController::hasStopped() {
    return state == State::conditionMatch ;
}
