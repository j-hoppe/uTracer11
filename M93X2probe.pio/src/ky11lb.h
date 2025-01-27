/* ky11lb.h - function to handle 11/34,04 KY11LB-LB interface

*/
#ifndef __KY11LB_H__
#define __KY11LB_H__
#include "messages.h"
#include "utils.h"
#include <Arduino.h>

class KY11LB {
  public:
    // register offsets
    enum class Signal : uint8_t {
        none,
        MPC,          // micro program counter
        MAN_CLK_ENAB, // writable
        MAN_CLK,      // writable
        FP11,         // '34 FP11_ATTACHED_L
        SBF,          // '34 SERVICE_BREAK_FAIL_L
        LIR,          // '34 LOAD_IR_L
        PBP           // '34 PFAIL_BR_PEND_H
    };

    void setup();
    const char *signalToText(Signal signal);
    Signal KY11LB::textToSignal(char *signalText);

    uint16_t readMPC(uint8_t gpio20a, uint8_t gpio20b);
    uint16_t readMPC();
    void writeMCE(bool val);
    void writeMC(bool val);
    bool writeSignal(Signal signal, bool val);
    ResponseKY11LBSignals getSignalsAsResponse(MsgTag _tag);
};

// Singleton
extern KY11LB theKY11LB;

#endif // __KY11LB_H__
