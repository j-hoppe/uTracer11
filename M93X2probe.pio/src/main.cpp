/* main.cpp - Setup and main loop

  uTracer11 Probe is a board plugged into the UNIBUS terminator slot
  - senses UNIBUS signals
  - senses complete DATI/DATO cycles
  - executed EXAM and DEPOSIT on UNIBUS
  - senses uProgram Counter and diagnostics signals
     on the M8266 header for PDP-11/34 CPU
  - generates µClock on the M8266 header
  - senses uProgram Counter / 2x28 diagnostics signals
    on two KM11 diagnostic boards KM11A/KM11B
  - generates 2x4 signals on KM11
    Use of KM11 depends on PDP-11 model under test

  Serial protocol see "controller.cpp"
*/
#include <Arduino.h>
#include <avr/wdt.h>

#include "bootlogic.h"
#include "console.h"
#include "controller.h"
#include "hardware.h"
#include "ky11lb.h"
#include "messages.h"
#include "unibus.h"

void setup() {
    // put your setup code here, to run once:
    // NO Watchdog ... we have long running loops
    wdt_disable();

    theLogger.setup();
    theHardware.setup();
    theKY11LB.setup();
    theUnibus.setup();

    LOG("\n\M93X2probe compiled %s %s\n", __DATE__, __TIME__);

    theConsole.setup(&Serial1, 115200);
    //theConsole.printStrF("\nHello world!\n");
    //while(1) ;

    theBootLogic.setup();

    // output response to "VERSION" command, is also answer to RESET
    theConsole.printStrF("\n%s\n", theController.versionAsResponse(0).render());

	/*
    // test print out, via controller callback
    thePdp11Data.setIterationCallback(&theController, &Pdp11DataUser::onPdp11DataIteration);
    auto recList = thePdp11Data.getRecordListByNumber(1);
    thePdp11Data.iterate(recList, 1);
    */
}

void loop() {
    uint32_t cur_millis = millis();

    // put your main code here, to run repeatedly:
    theHardware.loop(cur_millis);
    theConsole.loop();

    theBootLogic.loop(cur_millis);
    // BOOT on ACLO or boot switch

    theController.loop();

    if (theConsole.stringReceived) {
        // one line received, buffer valid until next .service()
        char *line = theConsole.getAndClearRcvString();
        char *response = theController.doHostCommand(line);
        if (response != nullptr)
            theConsole.xmtLine(response); // start output
    }
    /*
        // something to sent? Wait until buffer clear
          if (strlen(event_buffer) && theConsole.stringTransmitted) {
            theConsole.xmtLine("!"+event_message) ; // start output
            */
}
