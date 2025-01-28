/* messageinterface - abstract connector to a target Pdp11, via message streams
Derived classes:
    Communication to physical M93X2probe vie "messageinterfaceserial"
     with simulators over messageinterface TcpIp
*/
#ifndef __MESSAGEINTERFACE_H__
#define __MESSAGEINTERFACE_H__

#include <string>
#include "messages.h"

class MessageInterface {
private:
    std::string receiveRingBuffer;
protected:
    void appendRcvDataAndProcessResponses(std::string buffer);
public:
    virtual ~MessageInterface() = default;
    std::string name; // for serial: COM3, ttyUSB0

    unsigned long rcvMessageCount = 0;
    unsigned long xmtMessageCount = 0;

    // produce cyclic sequence for send request message tags
    MsgTag latestRequestTag = NOTAG;
    MsgTag getNextRequestTag();
    // hold tag of latest received response message
    MsgTag latestResponseTag = NOTAG;

    // endpoint = abstract endpoint address to probe or simualtor
    // like  "COM1", "/dev/ttyS0" or "localhost:49152"
    virtual bool connect(std::string endpoint); // abstract warning
    virtual void disconnect(); // abstract warning


    // gui calls this to process() all accumulated responses and delete them
    virtual void receiveAndProcessResponses(); // abstract warning

    // send a single request to target PDP11
    // and frees it. Return assigned tag.
    virtual MsgTag xmtRequest(Message* msg); // abstract warning
};

#endif // __MESSAGEINTERFACE_H__

