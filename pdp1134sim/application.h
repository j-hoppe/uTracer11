/* application.h - main module for pdp11/34 simulator with utracer11 interface */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "tcpmessageinterface.h"
// The Application class. This class shows a window
// containing a statusbar with the text "Hello World"
class Application {
public:
	int argc; //cmdline
	char **argv;

	void help();
	void processCmdline(int _argc, char *_argv[]);
	TcpMessageInterface messageInterface ;
	void connectToClient() ;

};

extern Application app;

#endif // __APPLICATION_H__
