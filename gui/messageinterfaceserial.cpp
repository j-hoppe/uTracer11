/* hardware - Interface to uTracer11 M93X2 probe and attached KY11/KM11 probes
Also manages Messages
*/


#include "messageinterfaceserial.h"
#include "messages.h"

#include "CSerialPort/SerialPortInfo.h"
using namespace itas109; // the authors namespace

#include "application.h" // to reach pdp11 via wxGetApp().pdp11

// return name list suitable for wxComboBox->Add()
std::vector<wxString> MessageInterfaceSerial::getAvailableSerialPorts()
{
    std::vector<wxString> result;
    std::vector<SerialPortInfo> m_portsList = CSerialPortInfo::availablePortInfos();
    for (size_t i = 0; i < m_portsList.size(); i++) {
        std::string portName = m_portsList[i].portName;
        wxString wxPortName(portName.c_str(), wxConvUTF8);
        result.push_back(wxPortName);
    }
    return result;
}

bool MessageInterfaceSerial::connect(std::string _portName)
{
    rcvMessageCount = 0;
    xmtMessageCount = 0;
    if (serialPort.isOpen())
        serialPort.close();
    serialRingBuffer = ""; // discard unread data
    // constants are namespace itas109
    name = _portName;
    serialPort.init(name.c_str(), BaudRate115200, ParityNone, DataBits8, StopOne, FlowNone);
    if (serialPort.open()) {
        wxLogStatus("Port %s open", name);
        return true;
    }
    else {
        wxLogError("Port %s could not be opened", name);
        return false;
    }
}

void MessageInterfaceSerial::disconnect() {
    serialPort.close();
}

// transmit to M93X2
void MessageInterfaceSerial::serialPortSendLine(std::string line)
{
    if (!serialPort.isOpen()) {
        wxLogError("Port %s not open", name);
        return;
    }
    int byteCount;
    const char* c_line = line.c_str();
    byteCount = serialPort.writeData(c_line, strlen(c_line));
    if (byteCount < 0) {
        wxLogError("Could not write to Port %s", name);
        return;
    }
    // terminate \n
    serialPort.writeData("\n", 1);
    //wxLogInfo("TxD \"%s\"", c_line);
}

// grab pending input from M93X2probe and add to receive buffer
// then split off full messages separated with \n
// parse and forward these, then free.
// called periodically @ 10ms, serial polling
void MessageInterfaceSerial::receiveAndProcessResponses() {
    // in 10ms @115200 baud: max bytecount = 0.01 * 115200/10 = 115
    const int bufferSize = 10240;// 10x reserve
    char buffer[bufferSize + 1];
    int bytesRead = serialPort.getReadBufferUsedLen(); // waiting chars
    if (bytesRead <= 0)
        return;
    if (bytesRead >= bufferSize) {
        wxLogError("Serial receive buffer overflow");
        bytesRead = bufferSize; // receive max one buffer
    }
    // fetch chunk
    serialPort.readData(buffer, bytesRead);
    buffer[bytesRead] = 0; // terminate string

    // add chunk of text to ringBuffer and process completed Messages
    appendRcvDataAndProcessResponses(buffer);
}

// convert message to text and send via RS232 to M93X2probe
// and frees it. Return assigned tag, maybe autogenerated.
MsgTag MessageInterfaceSerial::xmtRequest(Message* msg)
{
    MsgTag tag = (msg->tag == MsgTag::next) ? getNextRequestTag() : msg->tag;
    msg->tag = tag;
    serialPortSendLine(msg->render());
    xmtMessageCount++;
    delete msg; // is consumed now
    return tag;
}


