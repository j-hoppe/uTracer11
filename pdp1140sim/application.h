/* application.h - main module for pdp11/40 simulator with utracer11 interface */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "pdp11simulator.h"
#include "tcpmessageinterface.h"
// The Application class. This class shows a window
// containing a statusbar with the text "Hello World"
class Application {
public:
    int argc; //cmdline
    char **argv;

    void help();
    void processCmdline(int _argc, char *_argv[]);
    Pdp11Simulator::Console console ;
    TcpMessageInterface messageInterface ;
};

extern Application app;

#endif // __APPLICATION_H__
