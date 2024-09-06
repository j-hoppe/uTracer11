/* bootlogic - controls power-on and boot switch functions

- On power on, or when some other compoents requieres a reboot,
  ACLO is pulsed low.
  Capture this via interrupt
- If a User initiates manual BOOT over front panel switch (KY11LB) or Programmerconsole),
  the BOOTSWITCH FASTON tab must generate a ACLO pulse,
  like on M9301, M9312.

  Interrupt vs polling
  ACLO can be pulsed very short, example by KY11LB panels
  -> so ACLO edge must be caught by interrupt
  BOOT SWITCH: if operated manually, polling is enough
  But on KY11LB, the 8008 issues a 50usec Low inpulse
  -> interrupt

*/

#include "bootlogic.h"
// works on other singletons
#include "console.h"
#include "controller.h"
#include "unibus.h"


BootLogic theBootLogic; // Singleton


// Register 1->0 edge on ACLO_L pin Arduino.D8: "power good"
void acloHandleInterrupt() {
    theBootLogic.acloDetected = true ;
}

void bootSwitchHandleInterrupt() {
    theBootLogic.bootSwitchDetected = true ;
}

void BootLogic::setup() {
    acloPin.setupInput(8) ;
    bootSwitchPin.setupInput(9) ;
    acloDetected = false ;
    acloOffMillis = 0 ;

    // trigger ISR on "power good" ACLO 1->0 , ACLO_L RISING
    attachInterrupt(digitalPinToInterrupt(acloPin.gpio), acloHandleInterrupt, RISING);
    // trigger ISR on BOOTSWITCH down: 1->0
    attachInterrupt(digitalPinToInterrupt(bootSwitchPin.gpio), bootSwitchHandleInterrupt, FALLING);
}


void BootLogic::loop(uint32_t cur_millis)    {
#ifdef XXX // yes, the interrups are working
    if (acloDetected) { // asnychronuous ACLO
        theConsole.printStrF("ACLO!\n");
        delayMicroseconds(1000000) ; // display pause
        acloDetected = false ;
    }
    if (bootSwitchDetected) {
        theConsole.printStrF("BOOT SWITCH!\n");
        delayMicroseconds(1000000) ; // display pause
        bootSwitchDetected = false ;
    }
#endif
    if (acloDetected) { // asnychronuous ACLO
        auto fileNr = theHardware.optionSwitch.getVal() ;
        //theConsole.printStrF("BOOT %d\n", (int)fileNr);

        if (fileNr != 0)
            theController.bootProgram(fileNr) ;
        acloDetected = false ; // event processed
    } 
    if (bootSwitchDetected) {
        // bootswitch active: set ACLO
        if (acloOffMillis == 0) // aclo not yet set: do it
            theUnibus.writeACLO(true);
        acloOffMillis = cur_millis + acloPulseWidthMillis;
        bootSwitchDetected = false ; // event processed
    }
     // ACLO off when period complete, to cancel switch bouncing
     if ((acloOffMillis > 0) && (cur_millis > acloOffMillis)) {
            // switch off
            theUnibus.writeACLO(false);
            acloOffMillis = 0;
            // falling ACLO edge triggers ISR now
    }
}
