/* messageinterfaceserial - receive and transmit message over RS232
 Interface to uTracer11  M93X2 adapter and attached KY11/KM11 probes
*/
#ifndef __MESSAGEINTERFACESERIAL_H__
#define __MESSAGEINTERFACESERIAL_H__

// include path = CSerialPort/include
#include "wx/wx.h"
#include "CSerialPort/SerialPort.h"
#include "messages.h"
#include "messageinterface.h"

class MessageInterfaceSerial: public MessageInterface {
public:
    // COM port to uTracer11 PCB
    // Examples: Windows : COM1 Linux : /dev/ttyS0
    //void OnIdle();
    itas109::CSerialPort serialPort;
    std::vector<wxString> getAvailableSerialPorts();

	// portName like  "COM1", "/dev/ttyS0"
    bool connect(std::string portName) override;
    void disconnect() override;

    void receiveAndProcessResponses() override;
    MsgTag xmtRequest(Message* msg) override;

private:
    std::string serialRingBuffer;
    void serialPortSendLine(std::string line);
};

#endif // __MESSAGEINTERFACESERIAL_H__

