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
    uint16_t mpc; // current micro program counter
    uint16_t	memory[0x20000]; // 18 bit addresses = 128kwords
    uint32_t	opcodeCount ; // processed opcodes
    uint32_t	mcycleCount ; // processed microsteps

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
    void onCpuUnibusCycle(uint8_t c1c0, uint32_t addr, uint16_t data, bool nxm) override;

    void setMicroClockEnable(bool state) override; // 0 = single micro step, 1 = running
    void microStep() override;
public:
    const char* getVersionText() override;
    const char* getStateText() ;


    Pdp11Simulator34(Console *_console, TcpMessageInterface * messageInterface):
        Pdp11Simulator(_console, messageInterface) {}

    void setup() override;
    void service() override;


};


#endif // __PDP11SIMULATOR34_H__

