
#include "messages.h"

// Signals exposed o the PDP-11/40 KM11 A and B diagnostic headers
class Pdp1140KM11State {
public:
    // member variables are exact the labels on DEC 11/40 KM11 overlay sheets
    // inputs from CPU to KM11A
    uint16_t pupp = 0;
    uint16_t bupp = 0;
    bool trap = 0;
    bool ssyn = 0;
    bool msyn = 0;
    bool t = 0;
    bool c = 0;
    bool v = 0;
    bool z = 0;
    bool n = 0;
    // inputs MMU/FPU to KM11B
    uint32_t pba = 0; //17..08
    bool rom_a = 0;
    bool rom_b = 0;
    bool rom_c = 0;
    bool rom_d = 0;
    bool b_15 = 0;
    bool ecin_00 = 0;
    bool exp_unfl = 0;
    bool exp_ovfl = 0;
    bool dr00 = 0;
    bool dr09 = 0;
    bool msr00 = 0;
    bool msr01 = 0;
    bool eps_c = 0;
    bool eps_v = 0;
    bool eps_z = 0;
    bool eps_n = 0;

    // outputs from KM11A to CPU
    bool mstop = 0;
    bool mclk = 0;
    bool mclk_enab = 0;

    // lowlevel encode/decode


    // GUI evaluates raw KM11 signals from PDP11/40 or simulator
    void inputsFromKM11AResponse(ResponseKM11Signals* respKm11A);
    void inputsFromKM11BResponse(ResponseKM11Signals* respKm11B);
    // GUI sends controll signals to PDP11/40 or simulator
    void outputsToKM11AWriteRequest(RequestKM11SignalsWrite* reqKm11A);
    // no outputs over KM11B

    // simulator produces raw KM11 signals
    void inputsToKM11AResponse(ResponseKM11Signals* respKm11A);
    void inputsToKM11BResponse(ResponseKM11Signals* respKm11B);
    // simulator receives control signals
    void outputsFromKM11AWriteRequest(RequestKM11SignalsWrite* reqKm11A);
}
;


