
/* Encode/decode KM11 signals for the PDP-11/40

Remember: its a long way to transmit a  CPU simulator variable 
 over the message interface to the GUI.
1. CPU simulator var "mpc"			CPU
2. logical KM11A signals  ("PUPP")	Km11 overlay
3. KM11 flip chip signals (K2,N1,V1,P1,....)
4. compact "gpio" hex values
 

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

FlipChip signals on slot F1:
K2      J1      P1      R1      S1      N2      R2
N1      C1      L1      K1      M2      D2      E2       A1     B2
V1      T2      F1      S2      F2      H2      H1       U1     V2
P2      U2      D1      M1      E1      J2      L2


Signals indices [col,row] on M93X2probe KM11 PCB: see messages.txt

*/

#include "utils.h"
#include "pdp1140km11state.h"


// GUI evaluates raw KM11 signals from PDP11/40 or simulator
void Pdp1140KM11AState::inputsFromKM11AResponse(ResponseKM11Signals* respKm11A)
{
    if (respKm11A->channel != 'A')
        LOGERROR("Pdp1140KM11Signals::inputsFromKM11A: channel A expected, is %c", respKm11A->channel);
    // for bit-encoding see PCB of M93X2probe
    // get FlipChip Signal bits from GPIO, same for all KM11s
    respKm11A->getFlipchipSignalsFromGpioVals();

    // compose state vars fom bits, different for all KM11s
    pupp = setbit(respKm11A->K2, 6) | setbit(respKm11A->N1, 7) | setbit(respKm11A->V1, 8)
           | setbit(respKm11A->J1, 3) | setbit(respKm11A->C1, 4) | setbit(respKm11A->T2, 5)
           | setbit(respKm11A->P1, 0) | setbit(respKm11A->L1, 1) | setbit(respKm11A->F1, 2);
    bupp = setbit(respKm11A->R1, 6) | setbit(respKm11A->K1, 7) | setbit(respKm11A->S2, 8)
           | setbit(respKm11A->S1, 3) | setbit(respKm11A->M2, 4) | setbit(respKm11A->F2, 5)
           | setbit(respKm11A->N2, 0) | setbit(respKm11A->D2, 1) | setbit(respKm11A->H2, 2);
    trap = setbit(respKm11A->D1, 0);
    ssyn = setbit(respKm11A->M1, 0);
    msyn = setbit(respKm11A->E1, 0);
    t = setbit(respKm11A->J2, 0);
    c = setbit(respKm11A->R2, 0);
    v = setbit(respKm11A->E2, 0);
    z = setbit(respKm11A->H1, 0);
    n = setbit(respKm11A->L2, 0);
}


void Pdp1140KM11AState::inputsToKM11AResponse(ResponseKM11Signals* respKm11A) {
    // get bits from state vars, different for all KM11s
    respKm11A->K2 = getbit(pupp, 6);
    respKm11A->N1 = getbit(pupp, 7);
    respKm11A->V1 = getbit(pupp, 8);
    respKm11A->P2 = 0;
    respKm11A->J1 = getbit(pupp, 3);
    respKm11A->C1 = getbit(pupp, 4);
    respKm11A->T2 = getbit(pupp, 5);
    respKm11A->U2 = 0;
    respKm11A->P1 = getbit(pupp, 0);
    respKm11A->L1 = getbit(pupp, 1);
    respKm11A->F1 = getbit(pupp, 2);
    respKm11A->R1 = getbit(bupp, 6);
    respKm11A->K1 = getbit(bupp, 7);
    respKm11A->S2 = getbit(bupp, 8);
    respKm11A->S1 = getbit(bupp, 3);
    respKm11A->M2 = getbit(bupp, 4);
    respKm11A->F2 = getbit(bupp, 5);
    respKm11A->N2 = getbit(bupp, 0);
    respKm11A->D2 = getbit(bupp, 1);
    respKm11A->H2 = getbit(bupp, 2);
    respKm11A->D1 = trap ;
    respKm11A->M1 = ssyn;
    respKm11A->E1 = msyn;
    respKm11A->J2 = t;
    respKm11A->R2 = c;
    respKm11A->E2 = v;
    respKm11A->H1 = z;
    respKm11A->L2 = n;

    // compose GPIOs from bits, same for all KM11s
    respKm11A->channel = 'A' ;
    respKm11A->setGpioValsFromFlipchipSignals();
}


void Pdp1140KM11BState::inputsFromKM11BResponse(ResponseKM11Signals* respKm11B)
{
    if (respKm11B->channel != 'B')
        LOGERROR("Pdp1140KM11Signals::inputsFromKM11B: channel B expected, is %c", respKm11B->channel);
    // let the compiler do all the demuxing
    respKm11B->getFlipchipSignalsFromGpioVals();

    pba = setbit(respKm11B->K2, 15) | setbit(respKm11B->N1, 16) | setbit(respKm11B->V1, 17)
          | setbit(respKm11B->J1, 12) | setbit(respKm11B->C1, 13) | setbit(respKm11B->T2, 14)
          | setbit(respKm11B->P1, 9) | setbit(respKm11B->L1, 10) | setbit(respKm11B->F1, 11)
          | setbit(respKm11B->R1, 6) | setbit(respKm11B->K1, 7) | setbit(respKm11B->S2, 8);
    rom_a = setbit(respKm11B->P2, 0);
    rom_b = setbit(respKm11B->U2, 0);
    rom_c = setbit(respKm11B->D1, 0);
    rom_d = setbit(respKm11B->M1, 0);
    b_15 = setbit(respKm11B->S1, 0);
    ecin_00 = setbit(respKm11B->M2, 0);
    exp_unfl = setbit(respKm11B->F2, 0);
    exp_ovfl = setbit(respKm11B->E1, 0);
    dr00 = setbit(respKm11B->N2, 0);
    dr09 = setbit(respKm11B->D2, 0);
    msr00 = setbit(respKm11B->H2, 0);
    msr01 = setbit(respKm11B->J2, 0);
    eps_c = setbit(respKm11B->R2, 0);
    eps_v = setbit(respKm11B->E2, 0);
    eps_z = setbit(respKm11B->H1, 0);
    eps_n = setbit(respKm11B->L2, 0);
}

void Pdp1140KM11BState::inputsToKM11BResponse(ResponseKM11Signals* respKm11B) {
	UNREFERENCED_PARAMETER(respKm11B) ;
    LOGERROR("Pdp1140KM11AState::inputsToKM11BResponse NOT YET IMPLEMENTED %s!", "");
}


void Pdp1140KM11AState::outputsToKM11AWriteRequest(RequestKM11SignalsWrite* reqKm11A)
{
    reqKm11A->channel = 'A';
    // Compare 11/40 KM11 overlay paper with "OUT" signals  in messages.txt
    // out00 = ---    out01 = MSTOP
    // out10 = MCLK   out11 = MCLK ENAB
    reqKm11A->A1 = 0;
    reqKm11A->B2 = mstop;
    reqKm11A->U1 = mclk; // "mclk_l" in fpms
    reqKm11A->V2 = mclk_enab; // "mclk_enab_l" in fpms
    reqKm11A->setVal03FromFlipchipSignals();
}

void Pdp1140KM11AState::outputsFromKM11AWriteRequest(RequestKM11SignalsWrite* reqKm11A) {
    reqKm11A->getFlipchipSignalsFromVal03();
    mstop = reqKm11A->B2 ;
    mclk = reqKm11A->U1 ; // "mclk_l" in fpms
    mclk_enab = reqKm11A->V2; // "mclk_enab_l" in fpms
}



