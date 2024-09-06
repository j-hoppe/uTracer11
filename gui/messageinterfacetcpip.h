/* messageinterfaceserial - receive and transmit message over anTCP/IP socket

*/
#ifndef __MESSAGEINTERFACETCPIP_H__
#define __MESSAGEINTERFACETCPIP_H__


#include "wx/wx.h"
#include "wx/socket.h"
#include "messages.h"
#include "messageinterface.h"

class MessageInterfaceTcpIp : public MessageInterface {
private:
    //void onSocketEvent(wxSocketEvent& event);

public:

    // socketAddr like "localhost:49152" or "192.168.1.133:8080"
    bool connect(std::string hostAndPortStr) override;
    void disconnect() override;

    void receiveAndProcessResponses() override;
    void xmtRequest(Message* msg) override;

private:
    wxSocketClient  socketClient;
};

#endif // __MESSAGEINTERFACETCPIP_H__
