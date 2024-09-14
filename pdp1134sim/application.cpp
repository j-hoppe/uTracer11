/* application.cpp - ain module for pdp11/34 simulator with utracer11 interface

 The 11/34 simulator is TCP/IP server, which receives
 "Request"-Messages and answers with "Responses"

 It is a Linux console application.

 The actual simulator runs in an main thread.
 Messages to Simulator are created by receiver,
 delivered in a thread-save fifo
 and deleted by caller Simulator
 Messages from simulator

 */
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

#include "tcpmessageinterface.h"
#include "application.h"
#include "pdp11simulator34.h"

// singleton
Application app;

void Application::help() {
	console.printf("pdp1134sim - PDP11/34 simulator with utracer11 interface\n");
	console.printf("   Implements utracer11 message interface as TCP/IP server.\n");
	console.printf("call:\n");
	console.printf("./pdp1134sim <port>\n");
	console.printf("<port> = decimal TCP/IP port number to listen\n");
	console.printf("         Recommended <port> is 65392, the decimal UART base 177560.\n");
	exit(1);
}

void Application::processCmdline(int _argc, char *_argv[]) {
	argc = _argc;
	argv = _argv;

	if (argc != 2)
		help();
	messageInterface.tcpPort = strtol(argv[1], nullptr, 10);
}

int main(int argc, char *argv[]) {

	app.console.start() ;

	// instantiate a simulator, 11/34 or 11/40 or whatever
	
	Pdp11Simulator34 simulator(&app.console, &app.messageInterface);

	app.processCmdline(argc, argv);


	app.messageInterface.connectToClient();
	app.messageInterface.start(); // start receiver/transmitter threads
	// messages now waiting in queues

	// start simulator in main thread
	simulator.setup() ;
    
	simulator.loop() ;
}
