/* bootlogic - controls power-on and boot switch fucntions
*/
#ifndef __BOOTLOGIC_H__
#define __BOOTLOGIC_H__

#include "hardware.h"

class BootLogic {
	public:
		static const int acloPulseWidthMillis = 250 ; // ACLO 1/4 sec active pulse
		PinAux acloPin ; // inverted, D8
		PinAux bootSwitchPin ; // inverted, D9

        volatile bool acloDetected ; // ISR signals
		volatile bool bootSwitchDetected ;

		uint32_t acloOffMillis; // time, when ACLO has to be set inactive
		// 0: is inactive
        
        void setup() ;
        void loop(uint32_t cur_millis) ;
};

extern BootLogic theBootLogic; // Singleton

#endif // __BOOTLOGIC_H__



