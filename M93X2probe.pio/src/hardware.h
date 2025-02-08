/* hardware.h - low level functions for onboard IO, LEDs and switches


 */

#if !defined(__HARDWARE_H__)
#define __HARDWARE_H__

#include "mcp23017.h"
#include "eeprom.h"
#include <Arduino.h>


// AUX0..5
class PinAux {
  public:
    int gpio; // which pin
    void setupInput(int _gpio);
    void setupOutput(int _gpio);
    void setVal(bool val);
};

class LED {
  private:
    int gpio; // which pin
    uint32_t next_on_millis;
    uint32_t next_off_millis;

  public:
    bool on;
    uint32_t on_period_millis;  // 0 = permanent off
    uint32_t off_period_millis; // 0 = permanent on
    void setup(int _gpio);
    void loop(uint32_t cur_millis);
};

class OptionSwitch {
  public:
    void setup();
    uint8_t getVal();
};

class Hardware {
  public:
    static const int max_mcp_group = 3;
    static const int max_mcp_addr = 3;
    OptionSwitch optionSwitch;
    LED leds[2];

    // ACLO,BOOTSWITCH in bootlogic.h
    PinAux pinaux[4]; // AUX1..3 is MOSI;MISO;SCK
    void setup();     // initialization
    void loop(uint32_t cur_millis);      // background operation

    ///// MCP23017 array
    // globally suppress redundant register writes.
    // disable for setup sequences when unknown register state!
    bool mcpWriteOptimization = false;

    // names encoded as mcp_<group>_<addr>, see schematic
    Mcp23017 mcp[max_mcp_group + 1][max_mcp_addr + 1];

    Mcp23017 *getMcpByAddr(uint8_t group, uint8_t addr);

    uint8_t activeMcpGroup; // current selectect group
    void pulseMcpReset();
    void setMcpGroup(uint8_t group);

	Eeprom eeprom ;

  private:
    void setupMcpKY11LBRegisters();
    void setupMcpKM11Registers();
	void setupMcpUnibusRegisters() ;
};

extern Hardware theHardware;

#endif // __HARDWARE_H__
