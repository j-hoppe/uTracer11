/* unibus.h - UNIBUS signal capture and generation

*/
#ifndef __UNIBUS_H__
#define __UNIBUS_H__

#include "hardware.h"
#include "mcp23017.h"
#include "messages.h"
#include <Arduino.h>

class Unibus {
  public:
    // IDs for UNIBUs signals
    enum class Signal : uint8_t {
        none,
        addr, // 18 bit
        data, // 16 bit
        c0,
        c1,
        msyn,
        ssyn,
        pa,
        pb,
        intr,
        br4,
        br5,
        br6,
        br7,
        bg4,
        bg5,
        bg6,
        bg7,
        npr,
        npg,
        sack,
        bbsy,
        init,
        aclo,
        dclo
    };

  private:
    // MSYN/SSYN may stuck at 0 for current micro step,
    // MCP INT are re-armed after CAP* readout,
    // and fired again on same level.
    // INTs are filtered out until MSYN is found "inactive" again => duplicate_MSYN_INT = 0
    // or maybe 1 inactive again.
    bool duplicate_MSYN_INT = false;
    bool duplicate_SSYN_INT = false;

  public:
    void setup();

    const char *signalToText(Signal signal);
    Signal textToSignal(char *signalText);
    const char *c1c0ToText(uint8_t c1c0);

    ResponseUnibusSignals getSignalsAsResponse();

  private:
    uint32_t readADDR(uint8_t gpio00a, uint8_t gpio00b, uint8_t gpio01a);
    uint16_t readDATA(uint8_t gpio02a, uint8_t gpio02b, uint8_t gpio03a);
    bool readMSYN(uint8_t gpio00a);
    bool readSSYN(uint8_t gpio02a);
    uint8_t readC1C0(uint8_t gpio01b);

  public:
    uint32_t readADDR();
    uint16_t readDATA();
    bool readMSYN();
    bool readSSYN();
    uint8_t readC1C0();

    void writeADDR(uint32_t val);
    void writeDATA(uint16_t val);
    void writeC1(bool val);
    void writeMSYN(bool val);
    void writeSSYN(bool val);
    void writeBBSY(bool val);
    void writeSACK(bool val);
    void writeACLO(bool val);
    void writeDCLO(bool val);
    void writeINIT(bool val);

  public:
    // next capture is feed back of selef-generated DATI/DATO
    // if false, next capture is asynchronuos UNIBUs activity
    bool cycleRequestedPending = false;

    bool writeSignal(Signal signal, uint32_t val);
    uint16_t datiHead(uint32_t addr, bool *nxm);
    void datiProbeHead(uint32_t addr, bool *nxm);
    void datoHead(uint32_t addr, uint16_t data, bool *nxm);
    void datxTail();
    uint32_t sizeMem(uint32_t addr, uint32_t blockSize);
    uint32_t sizeMem();
    bool testMem(uint32_t seed, uint32_t endAddr, char *buffer, int bufferSize);
    bool getMSYNcaptured(uint32_t *address, uint8_t *c1c0);
    bool getSSYNcaptured(uint16_t *data);
};

extern Unibus theUnibus;

#endif // __UNIBUS_H__
