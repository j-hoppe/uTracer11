/* controller.h - application logic, dos user commands


 */

#if !defined(__CONTROLLER_H__)
#define __CONTROLLER_H__

#include "messages.h"
#include "pdp11data.h"

class Controller : public Pdp11DataUser {
  public:
    void setup(); // initialization
    void loop();  // background operation
                  //    bool unibusEventAsResponse(); // check for asynchronous MSYN/SSYN
    ResponseUnibusCycle *unibusEventAsResponse();

    ResponseVersion versionAsResponse(MsgTag _tag);

    // command string, result print over RS2323
    char *doHostCommand(char *line);

    // called by Pd11Data on iterate()
    bool onPdp11DataIteration(int functionCode, uint16_t addr, uint16_t data) override;

    void bootProgram(uint16_t fileNr);
};

// Singleton
extern Controller theController;

#endif // __CONTROLLER_H__
