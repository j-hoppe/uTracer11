/* autostepcontroller - state machine to handle stop conditions for auto single stepping

Issues MPC clock pulses, receives asynchronically
MPC values and CPU UniBus Cycles,
checks these against stop conditions.

 current logic:
 run until a certain MPC
 or
  a certain opcode fetch address is hit.
*/
#ifndef __AUTOSTEPCONTROLLER_H__
#define __AUTOSTEPCONTROLLER_H__

#include "messages.h"


class Pdp11Adapter; // forward

class AutoStepController {
public:
    enum class State {
        // ready to pulse next MAN CLK
        stepMpc,

        // mpc pulse issued, wait for MPC response message and perhaps DATI/DATO to receive
        waitForMpc,

        // opcode fetch MPC detected, wait for Unibus access message response
        waitForFetchUnibusCycle,

        //	break conditions hit
        conditionMatch

        //  error stop by timeout???
    };

    State state;
	void changeState(State newState) ;

    unsigned curMpc;
    ResponseUnibusCycle lastFetchUnibusCycle;

private:
    Pdp11Adapter* pdp11Adapter;

    // cur state

    // stop conditions
    unsigned stopMpc; // disable by impossible value
    uint32_t stopOpcodeAddress; // dsisable by impoissible (odd) address


    // eval logic terms here
    // currently MPC or OPcodeAddress
    bool breakConditionHit();

public:
    // setup
    void init(Pdp11Adapter* _pdp11Adapter, unsigned _stopMpc, uint32_t _stopOpcodeAddress);

    // Interface to message stream:
    // eval a received microprogram counter
    void evalUStep(unsigned mpc);

    // Interface to message stream:
    // eval a received DATI or DATO cycle
    void evalUnibusCycle(ResponseUnibusCycle *cycle);

    void loop();

    // stop due to condition hit.
    // also emergency timeout?
    bool hasStopped();
};

#endif // __AUTOSTEPCONTROLLER_H__

