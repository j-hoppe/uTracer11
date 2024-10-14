/* tcpmessageinterface.h - receive/transmit uTracer11 messages over TCP/IP

 ASCII data stream is read/written over TCP/IP socket in own threads.
 Stream of messages connected to application via thread-safe MessageQueue
 */

#ifndef __TCPMESSAGEINTERFACE_H__
#define __TCPMESSAGEINTERFACE_H__

#include <thread>
#include <string>
#include "messagequeue.h"

class TcpMessageInterface {
private:

	std::string receiveRingBuffer ;


	void receiveRequests() ; // receiver thread
	void transmitResponses() ; // transmitter thread
	
public:
	std::thread *receiverThread = nullptr;
	std::thread *transmitterThread= nullptr;
	bool receiverThreadRunning ;
	bool transmitterThreadRunning ;

	int tcpPort ; // TCP/IP port number
	int tcpSocket; // file handle for TCP/IP socket
	TcpMessageInterface() ;
	~TcpMessageInterface() ;

	MessageQueue 	receiveQueue ;
	MessageQueue 	transmitQueue ;

	void connectToClient() ;
	void start() ;
};



#endif /* __MESSAGEINTERFACE_H__ */
