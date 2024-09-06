/* pdp11simulator - base class for an simulator
  runs in own thread,
  receives requests and generates responses
  via a thread save message-Queue.
  Message interface via TCP/IP.

*/
#include <vector>
#include "messages.h"
#include "messagequeue.h"
#include "tcpmessageinterface.h"

class Pdp11Simulator {
private:
    TcpMessageInterface *messageInterface ;

public:
    // pointer to single instance as message context
    static Pdp11Simulator* instance;

    //  must link to frame work queues
    Pdp11Simulator(TcpMessageInterface * _messageInterface):
        messageInterface(_messageInterface) {
        instance = this ;
    }

	// state of simulated UNIBUS
    UnibusSignals unibusSignals;

    // emulator tells the message interface about its visibile internal state
    // state to be published by STEDEF/VAL messages
    std::vector<MessagesStateVar> stateVars;

    void stateVarRegisterClear() {
    	stateVars.clear() ;
    }

    // name, width, and internal location of a emulator internal variable
    void stateVarRegister(const char *name, void *varPointer, int varSize, int bitCount) {
    	MessagesStateVar stateVar;
    	strncpy(stateVar.name, name, stateVar.nameSize-1) ;
    	stateVar.name[stateVar.nameSize] = 0 ;
    	stateVar.bitCount = bitCount ;
    	stateVar.object = varPointer ;
	stateVar.objectSizeof = varSize;
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

    virtual void setup(); // initalization, once
    virtual void loop(); // contains the while(true) main loop

    virtual const char* getVersion() ;

    // is simualted micro machine running on own speed+
    // false = single micro step, true = running
    virtual void setMicroRunState(bool state) ; // abstract

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
    virtual void onRequestUnibusSignalsRead(); // read in base class, not abstract
    virtual void onRequestUnibusSignalWrite(RequestUnibusSignalWrite* requestUnibusSignalWrite); // abstract

    // called if simulator executes DATI or DATO
    virtual void onCpuUnibusCycle(uint8_t c1c0, uint32_t addr, uint16_t data, bool nxm) ;

    // a simulator repsonds
};

