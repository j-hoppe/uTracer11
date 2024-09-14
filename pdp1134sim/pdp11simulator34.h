/* pdp11simulator34 - simple 1134 fake
- is connected to gui via ProbeInterface methods
- has full memory
- fix ucode/UNIBUS loop
*/

#ifndef __PDP11SIMULATOR34_H__
#define __PDP11SIMULATOR34_H__


#include "pdp11simulator.h"

class Pdp11Simulator34 : public Pdp11Simulator {
private:
    // emulated CPU
    unsigned state; // position in predefined faked activity sequence
    bool microClockEnabled = false; // true = micro machine running, start stopped
    uint16_t mpc; // current micro program counter
    uint16_t	memory[0x20000]; // 18 bit addresses = 128kwords
    uint32_t	opcodecount ; // processed opcodes
    uint32_t	mcyclecount ; // processed microsteps

    // same internal registers, to be published via messages
    uint16_t	r[16] ;
    uint32_t	ba ; // busaddress, 18 bit
    bool		halt ; // true = macro CPU halted, executes 000,015,000 cycle only then

    // eval user commands
    void consolePrompt(bool printMenu) ;
    void onConsoleStart() ;
    void onConsoleInputline(std::string inputLine) override ;
    void onRequestKY11LBSignalWrite(RequestKY11LBSignalWrite* requestKY11LBSignalWrite) override;
    void onRequestKY11LBSignalsRead(RequestKY11LBSignalsRead* requestKY11LBSignalsRead) override;
    // move UNIBUS access to base class Pdp11Simulator ?
    void onRequestUnibusDeposit(RequestUnibusDeposit* requestUnibusDeposit) override;
    void onRequestUnibusExam(RequestUnibusExam* requestUnibusExam) override;
    void onRequestUnibusSignalWrite(RequestUnibusSignalWrite* requestUnibusSignalWrite) override;

    void setMicroClockEnable(bool state) override; // 0 = single micro step, 1 = running
    void microStep() override;
public:
    const char* getVersion() override;

    Pdp11Simulator34(Console *_console, TcpMessageInterface * messageInterface):
        Pdp11Simulator(_console, messageInterface) {}

    void setup() override;
    void loop() override;


};


#endif // __PDP11SIMULATOR34_H__

