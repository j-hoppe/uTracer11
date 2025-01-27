/* pdp11simulator - base class for an simulator
  runs in own thread,
  receives requests and generates responses
  via a thread save message-Queue.
  Message interface via TCP/IP.

*/
#ifndef __PDP11SIMULATOR_H__
#define __PDP11SIMULATOR_H__



#include <vector>
#include "variables.h"
#include "messages.h"
#include "messagequeue.h"
#include "tcpmessageinterface.h"

class Pdp11Simulator {

public:
// stdin, stdout for user commands
    class Console {
    private:
        std::thread *inputThread = nullptr;
        void processInput() ;

    public:
        Pdp11Simulator *simulator = nullptr;
        Console() {} ;
        ~Console() {} ;


        // better use these functions for output, perhaps later re-routing
        int printf(const char *format, ...) ;
        void start() ;
    };


    Console *console ;
private:
    TcpMessageInterface *messageInterface ;

public:
    // pointer to Singleton instance as message context
    static Pdp11Simulator* instance;

    //  must link to console and frame work queues
    Pdp11Simulator(Console *_console, TcpMessageInterface * _messageInterface):
        console(_console), messageInterface(_messageInterface) {
        instance = this ;
        console->simulator = this ; // link to onConsoleInputLine()
    }

	// main state: running, stopped
    bool microClockEnabled = false; // true = micro machine running, start stopped

    // state of simulated UNIBUS
    UnibusSignals unibusSignals;

    // emulator tells the message interface about its visibile internal state
    // state to be published by STEDEF/VAL messages
    std::vector<Variable> stateVars;

    void stateVarRegisterClear() {
        stateVars.clear() ;
    }

    // name, width, and internal location of a emulator internal variable
    void stateVarRegister(const char *name, void *varPointer, int varSize, int bitCount) {
        Variable stateVar;
        stateVar.name = name ;
        stateVar.bitCount = bitCount ;
        stateVar.endpoint = varPointer ;
        stateVar.endpointSizeof = varSize;
        stateVar.value = 0 ; // not used here
        stateVars.push_back(stateVar);
    }




    // pop all pending message from the queue and call the individual message handlers
    void processPendingRequests() {
        Message *msg ;
        // not waiting for next message
        while ((msg = messageInterface->receiveQueue.pop(false))) {
            msg->process() ;
            delete msg ;
        }
    }

    void respond(Message *msg) {
        messageInterface->transmitQueue.push(msg) ;
    }

    // eval user commands, called in parallel thread!
    virtual void onConsoleInputline(std::string inputLine) ; // abstract


    virtual void setup(); // initalization, once
    virtual void service(); // called repeatedly for non-preemptive multitasking

    virtual const char* getVersionText() ;

    // is simualted micro machine running on own speed+
    // false = single micro step, true = running
    virtual void setMicroClockEnable(bool state) ; // abstract

    // GUI executes one micro step
    virtual void microStep(); // abstract

    // a simulator must repond to these requests, sent by GUI
    // KY11-LB only on 34,04, KM11 on the others.
    virtual void onRequestKY11LBSignalWrite(RequestKY11LBSignalWrite* requestKY11LBSignalWrite); // abstract
    virtual void onRequestKY11LBSignalsRead(RequestKY11LBSignalsRead* requestKY11LBSignalsRead); // abstract
    virtual void onRequestKM11SignalsWrite(RequestKM11SignalsWrite* requestKM11SignalsWrite); // abstract
    virtual void onRequestKM11SignalsRead(RequestKM11SignalsRead* requestKM11SignalsRead); // abstract
    virtual void onRequestUnibusDeposit(RequestUnibusDeposit* requestUnibusDeposit); // abstract
    virtual void onRequestUnibusExam(RequestUnibusExam* requestUnibusExam); // abstract
    virtual void onRequestUnibusSignalsRead(RequestUnibusSignalsRead* requestUnibusSignalsRead); // read in base class, not abstract
    virtual void onRequestUnibusSignalWrite(RequestUnibusSignalWrite* requestUnibusSignalWrite); // abstract

    // called if simulator executes DATI or DATO
    virtual void onCpuUnibusCycle(uint8_t c1c0, uint32_t addr, uint16_t data, bool nxm) ;
};

#endif // __PDP11SIMULATOR_H__
