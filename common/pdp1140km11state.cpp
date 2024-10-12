
/* Encode/decode KM11 signals

Signal layout on DEC KM11A (CPU)
    KD11-A Maintenance Console Overlay (A-SS-5509081-0-12)

pupp6   pupp3   pupp0   bupp6   bupp3   bupp0   c
                                                        ----    mstop
pupp7   pupp4   pupp1   bupp7   bupp4   bupp1   v
                                                        mclk    mclk_enab
pupp8   pupp5   pupp2   bupp8   bupp5   bupp2   z

 --      --     trap    ssyn    msyn    t       n


Signal layout on DEC KM11B (MMU / FPU)
    KT11-D, KE11-E,F Maintenance Console Overlay (A-SS-5509081-0-13)

pba15   pba12   pba09   pba06   b15     dr00    epsc
                                                        ----    ----
pba16   pba13   pba10   pba07   ecn00   dr09    epsv
                                                        ----    ----
pba17   pba14   pba11   pba08   expunfl msr00   epsz

roma    romb    romc    romd    expovfl msr01   epsn


Signals indices [col,row] on M93X2probe KM11 PCB:
    IN (LEDs)                                           Out (switches)
0,0     1,0     2,0     3,0     4,0     5,0     6,0
0,1     1,1     2,1     3,1     4,1     5,1     6,1     0,0  1,0
0,2     1,2     2,2     3,2     4,2     5,2     6,2     0,1  1,1
0,3     1,3     2,3     3,3     4,3     5,3     6,3


Encoding KM11-Signals - gpio bits on M93X2probe PCB
    mcp[3][0].a = gpio0a = KM11A_IN00..12, OUT00
    mcp[3][0].b = gpio0b = KM11A_IN13..31, OUT01
    mcp[3][1].a = gpio1a = KM11A_IN32..50, OUT10
    mcp[3][1].b = gpio1b = KM11A_IN51..63, OUT11

    mcp[3][2], mcp[3][3] same for KM11B

*/

#include "utils.h"
#include "pdp1140km11state.h"


// get a KM11 signal bit from the matrix andd shift it
#define getbit(val,shift) (!!( (val) & (1 << (shift) ) ) )  // extract a bit as bool
#define setbit(bitmask,shift) ( (bitmask) ? 1 << (shift):0 )

// GUI evaluates raw KM11 signals from PDP11/40 or simulator
void Pdp1140KM11State::inputsFromKM11AResponse(ResponseKM11Signals* respKm11A)
{
    if (respKm11A->channel != 'A')
        LOGERROR("Pdp1140KM11Signals::inputsFromKM11A: channel A expected, is %c", respKm11A->channel);
    // for bit-encoding see PCB of M93X2probe


    // get bits from GPIO, same for all KM11s
    //TODO: in** into base class KM11State
    // new methods:  gpios2bits(), bits2gpios()

    auto in00 = getbit(respKm11A->gpio0a, 0);
    auto in01 = getbit(respKm11A->gpio0a, 1);
    auto in02 = getbit(respKm11A->gpio0a, 2);
    //auto in03 = respKm11A->gpio0a & 8; NC
    auto in10 = getbit(respKm11A->gpio0a, 4);
    auto in11 = getbit(respKm11A->gpio0a, 5);
    auto in12 = getbit(respKm11A->gpio0a, 6);

    //auto in13 = respKm11A->gpio0b & 1; NC
    auto in20 = getbit(respKm11A->gpio0b, 1);
    auto in21 = getbit(respKm11A->gpio0b, 2);
    auto in22 = getbit(respKm11A->gpio0b, 3);
    auto in23 = getbit(respKm11A->gpio0b, 4);
    auto in30 = getbit(respKm11A->gpio0b, 5);
    auto in31 = getbit(respKm11A->gpio0b, 6);

    auto in32 = getbit(respKm11A->gpio1a, 0);
    auto in33 = getbit(respKm11A->gpio1a, 1);
    auto in40 = getbit(respKm11A->gpio1a, 2);
    auto in41 = getbit(respKm11A->gpio1a, 3);
    auto in42 = getbit(respKm11A->gpio1a, 4);
    auto in43 = getbit(respKm11A->gpio1a, 5);
    auto in50 = getbit(respKm11A->gpio1a, 6);

    auto in51 = getbit(respKm11A->gpio1b, 0);
    auto in52 = getbit(respKm11A->gpio1b, 1);
    auto in53 = getbit(respKm11A->gpio1b, 2);
    auto in60 = getbit(respKm11A->gpio1b, 3);
    auto in61 = getbit(respKm11A->gpio1b, 4);
    auto in62 = getbit(respKm11A->gpio1b, 5);
    auto in63 = getbit(respKm11A->gpio1b, 6);

    // compose state vars fom bits, different for all KM11s
    pupp = setbit(in00, 6) | setbit(in01, 7) | setbit(in02, 8)
           | setbit(in10, 3) | setbit(in11, 4) | setbit(in12, 5)
           | setbit(in20, 0) | setbit(in21, 1) | setbit(in22, 2);
    bupp = setbit(in30, 6) | setbit(in31, 7) | setbit(in32, 8)
           | setbit(in40, 3) | setbit(in41, 4) | setbit(in42, 5)
           | setbit(in50, 0) | setbit(in51, 1) | setbit(in52, 2);
    trap = setbit(in23, 0);
    ssyn = setbit(in33, 0);
    msyn = setbit(in43, 0);
    t = setbit(in53, 0);
    c = setbit(in60, 0);
    v = setbit(in61, 0);
    z = setbit(in62, 0);
    n = setbit(in63, 0);
}


void Pdp1140KM11State::inputsToKM11AResponse(ResponseKM11Signals* respKm11A) {
    //TODO: in** into ResponseKM11Signals
    // get bits from state vars, different for all KM11s
    uint8_t in00 = getbit(pupp, 6);
    uint8_t in01 = getbit(pupp, 7);
    uint8_t in02 = getbit(pupp, 8);
    uint8_t in03 = 0;
    uint8_t in10 = getbit(pupp, 3);
    uint8_t in11 = getbit(pupp, 4);
    uint8_t in12 = getbit(pupp, 5);
    uint8_t in13 = 0;
    uint8_t in20 = getbit(pupp, 0);
    uint8_t in21 = getbit(pupp, 1);
    uint8_t in22 = getbit(pupp, 2);
    uint8_t in30 = getbit(bupp, 6);
    uint8_t in31 = getbit(bupp, 7);
    uint8_t in32 = getbit(bupp, 8);
    uint8_t in40 = getbit(bupp, 3);
    uint8_t in41 = getbit(bupp, 4);
    uint8_t in42 = getbit(bupp, 5);
    uint8_t in50 = getbit(bupp, 0);
    uint8_t in51 = getbit(bupp, 1);
    uint8_t in52 = getbit(bupp, 2);
    uint8_t in23 = trap ;
    uint8_t in33 = ssyn;
    uint8_t in43 = msyn;
    uint8_t in53 = t;
    uint8_t in60 = c;
    uint8_t in61 = v;
    uint8_t in62 = z;
    uint8_t in63 = n;

    // compose GPIOs from bits, same for all KM11s
    respKm11A->channel = 'A' ;
    respKm11A->gpio0a = setbit(in00, 0) | setbit(in01, 1) | setbit(in02, 2) | setbit(in03, 3)
                        | setbit(in10, 4) | setbit(in11, 5) | setbit(in12, 6) ;
    respKm11A->gpio0b = setbit(in13, 0) | setbit(in20, 1) | setbit(in21, 2) | setbit(in22, 3)
                        | setbit(in23, 4) | setbit(in30, 5) | setbit(in31, 6) ;
    respKm11A->gpio1a = setbit(in32, 0) | setbit(in33, 1) | setbit(in40, 2) | setbit(in41, 3)
                        | setbit(in42, 4) | setbit(in43, 5)	| setbit(in50, 6) ;
    respKm11A->gpio1b = setbit(in51, 0) | setbit(in52, 1) | setbit(in53, 2) | setbit(in60, 3)
                        | setbit(in61, 4) | setbit(in62, 5)	| setbit(in63, 6) ;
}


void Pdp1140KM11State::inputsFromKM11BResponse(ResponseKM11Signals* respKm11B)
{
    if (respKm11B->channel != 'B')
        LOGERROR("Pdp1140KM11Signals::inputsFromKM11B: channel B expected, is %c", respKm11B->channel);
    // let the compiler do all the demuxing
    auto in00 = getbit(respKm11B->gpio0a, 0);
    auto in01 = getbit(respKm11B->gpio0a, 1);
    auto in02 = getbit(respKm11B->gpio0a, 2);
    auto in03 = getbit(respKm11B->gpio0a, 3);
    auto in10 = getbit(respKm11B->gpio0a, 4);
    auto in11 = getbit(respKm11B->gpio0a, 5);
    auto in12 = getbit(respKm11B->gpio0a, 6);

    auto in13 = getbit(respKm11B->gpio0b, 0);
    auto in20 = getbit(respKm11B->gpio0b, 1);
    auto in21 = getbit(respKm11B->gpio0b, 2);
    auto in22 = getbit(respKm11B->gpio0b, 3);
    auto in23 = getbit(respKm11B->gpio0b, 4);
    auto in30 = getbit(respKm11B->gpio0b, 5);
    auto in31 = getbit(respKm11B->gpio0b, 6);

    auto in32 = getbit(respKm11B->gpio1a, 0);
    auto in33 = getbit(respKm11B->gpio1a, 1);
    auto in40 = getbit(respKm11B->gpio1a, 2);
    auto in41 = getbit(respKm11B->gpio1a, 3);
    auto in42 = getbit(respKm11B->gpio1a, 4);
    auto in43 = getbit(respKm11B->gpio1a, 5);
    auto in50 = getbit(respKm11B->gpio1a, 6);

    auto in51 = getbit(respKm11B->gpio1b, 0);
    auto in52 = getbit(respKm11B->gpio1b, 1);
    auto in53 = getbit(respKm11B->gpio1b, 2);
    auto in60 = getbit(respKm11B->gpio1b, 3);
    auto in61 = getbit(respKm11B->gpio1b, 4);
    auto in62 = getbit(respKm11B->gpio1b, 5);
    auto in63 = getbit(respKm11B->gpio1b, 6);

    pba = setbit(in00, 15) | setbit(in01, 16) | setbit(in02, 17)
          | setbit(in10, 12) | setbit(in11, 13) | setbit(in12, 14)
          | setbit(in20, 9) | setbit(in21, 10) | setbit(in22, 11)
          | setbit(in30, 6) | setbit(in31, 7) | setbit(in32, 8);
    rom_a = setbit(in03, 0);
    rom_b = setbit(in13, 0);
    rom_c = setbit(in23, 0);
    rom_d = setbit(in33, 0);
    b_15 = setbit(in40, 0);
    ecin_00 = setbit(in41, 0);
    exp_unfl = setbit(in42, 0);
    exp_ovfl = setbit(in43, 0);
    dr00 = setbit(in50, 0);
    dr09 = setbit(in51, 0);
    msr00 = setbit(in52, 0);
    msr01 = setbit(in53, 0);
    eps_c = setbit(in60, 0);
    eps_v = setbit(in61, 0);
    eps_z = setbit(in62, 0);
    eps_n = setbit(in63, 0);
}

void Pdp1140KM11State::inputsToKM11BResponse(ResponseKM11Signals* respKm11B) {
	UNREFERENCED_PARAMETER(respKm11B) ;
    LOGERROR("Pdp1140KM11State::inputsToKM11BResponse NOT YET IMPLEMENTED %s!", "");
}


void Pdp1140KM11State::outputsToKM11AWriteRequest(RequestKM11SignalsWrite* reqKm11A)
{
    reqKm11A->channel = 'A';
    // Compare 11/40 KM11 overlay paper with "OUT" signals  in messages.txt
    // out00 = ---    out01 = MSTOP
    // out10 = MCLK   out11 = MCLK ENAB
    auto out00 = 0;
    auto out01 = mstop;
    auto out10 = !mclk; // "mclk_l" in fpms
    auto out11 = !mclk_enab; // "mclk_enab_l" in fpms
    reqKm11A->val03 = setbit(out00, 0) | setbit(out01, 1) | setbit(out10, 2) | setbit(out11, 3);
}

void Pdp1140KM11State::outputsFromKM11AWriteRequest(RequestKM11SignalsWrite* reqKm11A) {
    // auto out00 = getbit(reqKm11A->val03, 0);
    auto out01 = getbit(reqKm11A->val03, 1);
    auto out10 = getbit(reqKm11A->val03, 2);
    auto out11 = getbit(reqKm11A->val03, 3);
    mstop = out01 ;
    mclk = !out10 ; // "mclk_l" in fpms
    mclk_enab = !out11; // "mclk_enab_l" in fpms
}



