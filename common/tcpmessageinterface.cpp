/* tcpmessageinterface.cpp - receive/transmit uTracer11 messages over TCP/IP

 ASCII data stream is read/written over TCP/IP socket in own threads.
 Stream of messages connected to application via thread-safe MessageQueue
 */

#include <cstdlib>
#include <cstdio>
#include <cstring> // memset
#include <ctime>
#include <thread>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "tcpmessageinterface.h"

TcpMessageInterface::TcpMessageInterface() {
    tcpPort = 0; // TCP/IP port number
    tcpSocket = 0; // file handle for TCP/IP socket
}

TcpMessageInterface::~TcpMessageInterface() {
    if (tcpSocket > 0)
        close(tcpSocket);
}


void TcpMessageInterface::connectToClient() {

    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

#ifdef __CYGWIN__
#define SO_REUSEPORT 0 // long discussions on the net
#endif
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt() failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(tcpPort);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Server is listening on port %d for client to connect ...\n", tcpPort);

    if ((tcpSocket = accept(server_fd, (struct sockaddr*) &address,
                            (socklen_t*) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Client connected\n");
}


// poll char stream over socket,
// split into text lines, parse and queue
// terminates only on connection loss
void TcpMessageInterface::receiveRequests() {
    char buffer[1024] = { 0 };
	receiverThreadRunning = true ;
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(tcpSocket, buffer, 1024);
        if (bytesRead <= 0) {
            fprintf(stderr, "Read() error, client disconnected or error occurred\n");
            close(tcpSocket);
			receiverThreadRunning = false ; // signal
			return ;
        }
        receiveRingBuffer.append(buffer);
        const std::string separators = "\n\r;" ; // cr, lf or ;
        // extract leading lines at \n and \r and ;
        size_t separatorPos = receiveRingBuffer.find_first_of(separators);
        while (separatorPos != std::string::npos) {
            // extract and process all accumulated messages
            std::string line = receiveRingBuffer.substr(0, separatorPos); // strip \n
            receiveRingBuffer.erase(0, separatorPos + 1); // remove from buffer, discard \n
            separatorPos = receiveRingBuffer.find_first_of(separators); // find next
            if (line != "") {
                // add message to queue
                Message* msg = Message::parse(line.c_str());
                if (msg == nullptr) {
					// parse error
					msg = new ResponseError(Message::errorBuffer) ;
					fprintf(stderr, "Non-message text from client: \"%s\"\n", msg->render());
					transmitQueue.push(msg) ; // direct response, only valid msg go to simulator
                } else {
					// do not flood console with traffic
                    //fprintf(stderr, "Message from client: %s\n", msg->render());
                    receiveQueue.push(msg) ; // do be deleted by pop()-caller
                }
            }
        }
    }
}

// wait for queue to fill, then render and send over socket
// independed threads
// terminates only on connection loss
void TcpMessageInterface::transmitResponses() {
    struct timespec waitTimespec10ms;
    waitTimespec10ms.tv_sec = 0; 		  // 0 seconds
    waitTimespec10ms.tv_nsec = 10 * 1000000; // 10 milliseconds in nanoseconds

	transmitterThreadRunning = true ;
    // unprocessed message to send?
    while (true) {
        Message *msg = transmitQueue.pop(true) ; // wait blocking until filled
        std::string msgTxt(msg->render());
        delete msg ;
        msgTxt.append("\r\n") ; // separator, CR LF to work under Windows telnet client
        // send and clear, thread unsafe
        int bytesSend = send(tcpSocket, msgTxt.c_str(), msgTxt.length(), 0);
        if (bytesSend <= 0) {
            fprintf(stderr, "Send() error, client disconnected or error occurred\n");
            close(tcpSocket);
			transmitterThreadRunning = false ; // signal
			return ;
        }
		// do not flood console
		// fprintf(stderr, "Message to client: %s\n", msgTxt.c_str());
        nanosleep(&waitTimespec10ms, nullptr) ;
    }
}

// already connected, setup threads and wait for completion
void TcpMessageInterface::start() {
    receiveQueue.clear() ;
    transmitQueue.clear() ;

    receiveRingBuffer = "";

    receiverThread = new std::thread(&TcpMessageInterface::receiveRequests, this);
    transmitterThread = new std::thread(&TcpMessageInterface::transmitResponses,
                                        this);
}
