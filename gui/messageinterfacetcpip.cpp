/* messageinterfaceserial - receive and transmit message over anTCP/IP socket
Also manages Messages
*/


#include "messages.h"
#include "messageinterfacetcpip.h"

#include "application.h" // to reach pdp11 via wxGetApp().pdp11

#ifdef USED
// not used
void MessageInterfaceTcpIp::onSocketEvent(wxSocketEvent& event)
{
    switch (event.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
        wxLogInfo("Connected to the server.\n");
        break;

    case wxSOCKET_INPUT:
    {
        char buffer[256];
        socketClient.Read(buffer, sizeof(buffer));
        buffer[socketClient.LastCount()] = '\0'; // Ensure null-termination
        wxLogInfo("Received: \"%s\"\n", buffer);
    }
    break;

    case wxSOCKET_LOST:
        wxLogInfo("Connection lost.\n");
        break;
    }
}
#endif

// socketAddr like "localhost:49152" or "192.168.1.133:8081"
bool MessageInterfaceTcpIp::connect(std::string hostAndPortStr)
{
    rcvMessageCount = 0;
    xmtMessageCount = 0;

	name = "<no port>" ; // error until connection
	
    // split socketAddr into host and socket nr
    wxArrayString tokens = wxSplit(hostAndPortStr, ':', 0);
    if (tokens.GetCount() != 2) {
        wxLogFatalError("Socket address must have form <hostname>:<portnumber>, is %s", hostAndPortStr);
    }
    wxString hostAddress = tokens[0];
    long portNumber;
    if (!tokens[1].ToLong(&portNumber) || portNumber <= 0 || portNumber > 65535) {
        wxLogFatalError("Invalid socket port number, must have form <hostname>:<portnumber>, is %s", hostAndPortStr);
    }
    wxIPV4address ipAddr;
    if (!ipAddr.Hostname(hostAddress)) {
        wxLogFatalError("Invalid or unresolvable host address, is %s", hostAddress);
    }
    if (!ipAddr.Service(static_cast<wxUint16>(portNumber))) {
        wxLogFatalError("Invalid port number, is %d", portNumber);
    }

    wxLogInfo("Attempting to connect to %s = %s on port %d", ipAddr.Hostname(), ipAddr.IPAddress(), ipAddr.Service());

    if (!socketClient.Connect(ipAddr, true)) { // wait
        wxLogFatalError("Failed to connect to %s = %s on port %d", ipAddr.Hostname(), ipAddr.IPAddress(), ipAddr.Service());
    }

    //this->Connect(socketClient.GetId(), wxEVT_SOCKET, wxSocketEventHandler(this::onSocketEvent));

    socketClient.SetFlags(wxSOCKET_NOWAIT); // Read() not blocking, no events

	name = hostAndPortStr ; // success

    return true;
}


void MessageInterfaceTcpIp::disconnect() {
    socketClient.Close();
}


// grab pending input from socket and add to receive buffer
// then split off full messages separated with \n or ;
// parse and forward these.
// called periodically @ 10ms, polling
void MessageInterfaceTcpIp::receiveAndProcessResponses() {
    // poll 
    if (!socketClient.IsData())
        return;
    if (!socketClient.IsOk())
        wxLogFatalError("Socket connection broke");

    const int bufferSize = 10240;// 10x reserve
    char buffer[bufferSize + 1];
    socketClient.Read(buffer, bufferSize); // waiting chars
    int bytesRead = socketClient.LastReadCount();
    if (bytesRead <= 0)
        return;
    if (bytesRead >= bufferSize) {
        wxLogError("Socket receive buffer overflow");
        bytesRead = bufferSize; // receive max one buffer
    }
    // fetch chunk of data
    buffer[bytesRead] = 0; // terminate string

    // add chunk of text to ringBuffer and process completed Messages
    appendDataAndProcessResponses(buffer);
}

// convert message to text and send via RS232 to M93X2probe
// and frees it!
void MessageInterfaceTcpIp::xmtRequest(Message* msg)
{
    if (!socketClient.IsOk())
        wxLogFatalError("Socket connection broke");

    const int bufferSize = 10240;// 10x reserve
    char buffer[bufferSize + 1];

    strcpy_s(buffer, bufferSize, msg->render());
    strcat_s(buffer, bufferSize, "\n");
    int bytesToWrite = strlen(buffer);
    socketClient.Write(buffer, bytesToWrite);
    int bytesWritten = socketClient.LastWriteCount();
    if (bytesWritten != bytesToWrite)
        wxLogFatalError("Socket write(): write %s = %d bytes, only %d written", buffer, bytesToWrite, bytesWritten);

    xmtMessageCount++;
    delete msg; // is consumed now

}

