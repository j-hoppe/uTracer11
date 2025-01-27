
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
    pupp = SETBIT(respKm11A->K2, 6) | SETBIT(respKm11A->N1, 7) | SETBIT(respKm11A->V1, 8)
           | SETBIT(respKm11A->J1, 3) | SETBIT(respKm11A->C1, 4) | SETBIT(respKm11A->T2, 5)
           | SETBIT(respKm11A->P1, 0) | SETBIT(respKm11A->L1, 1) | SETBIT(respKm11A->F1, 2);
    bupp = SETBIT(respKm11A->R1, 6) | SETBIT(respKm11A->K1, 7) | SETBIT(respKm11A->S2, 8)
           | SETBIT(respKm11A->S1, 3) | SETBIT(respKm11A->M2, 4) | SETBIT(respKm11A->F2, 5)
           | SETBIT(respKm11A->N2, 0) | SETBIT(respKm11A->D2, 1) | SETBIT(respKm11A->H2, 2);
    trap = SETBIT(respKm11A->D1, 0);
    ssyn = SETBIT(respKm11A->M1, 0);
    msyn = SETBIT(respKm11A->E1, 0);
    t = SETBIT(respKm11A->J2, 0);
    c = SETBIT(respKm11A->R2, 0);
    v = SETBIT(respKm11A->E2, 0);
    z = SETBIT(respKm11A->H1, 0);
    n = SETBIT(respKm11A->L2, 0);
}


void Pdp1140KM11AState::inputsToKM11AResponse(ResponseKM11Signals* respKm11A) {
    // get bits from state vars, different for all KM11s
    respKm11A->K2 = GETBIT(pupp, 6);
    respKm11A->N1 = GETBIT(pupp, 7);
    respKm11A->V1 = GETBIT(pupp, 8);
    respKm11A->P2 = 0;
    respKm11A->J1 = GETBIT(pupp, 3);
    respKm11A->C1 = GETBIT(pupp, 4);
    respKm11A->T2 = GETBIT(pupp, 5);
    respKm11A->U2 = 0;
    respKm11A->P1 = GETBIT(pupp, 0);
    respKm11A->L1 = GETBIT(pupp, 1);
    respKm11A->F1 = GETBIT(pupp, 2);
    respKm11A->R1 = GETBIT(bupp, 6);
    respKm11A->K1 = GETBIT(bupp, 7);
    respKm11A->S2 = GETBIT(bupp, 8);
    respKm11A->S1 = GETBIT(bupp, 3);
    respKm11A->M2 = GETBIT(bupp, 4);
    respKm11A->F2 = GETBIT(bupp, 5);
    respKm11A->N2 = GETBIT(bupp, 0);
    respKm11A->D2 = GETBIT(bupp, 1);
    respKm11A->H2 = GETBIT(bupp, 2);
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

    pba = SETBIT(respKm11B->K2, 15) | SETBIT(respKm11B->N1, 16) | SETBIT(respKm11B->V1, 17)
          | SETBIT(respKm11B->J1, 12) | SETBIT(respKm11B->C1, 13) | SETBIT(respKm11B->T2, 14)
          | SETBIT(respKm11B->P1, 9) | SETBIT(respKm11B->L1, 10) | SETBIT(respKm11B->F1, 11)
          | SETBIT(respKm11B->R1, 6) | SETBIT(respKm11B->K1, 7) | SETBIT(respKm11B->S2, 8);
    rom_a = SETBIT(respKm11B->P2, 0);
    rom_b = SETBIT(respKm11B->U2, 0);
    rom_c = SETBIT(respKm11B->D1, 0);
    rom_d = SETBIT(respKm11B->M1, 0);
    b_15 = SETBIT(respKm11B->S1, 0);
    ecin_00 = SETBIT(respKm11B->M2, 0);
    exp_unfl = SETBIT(respKm11B->F2, 0);
    exp_ovfl = SETBIT(respKm11B->E1, 0);
    dr00 = SETBIT(respKm11B->N2, 0);
    dr09 = SETBIT(respKm11B->D2, 0);
    msr00 = SETBIT(respKm11B->H2, 0);
    msr01 = SETBIT(respKm11B->J2, 0);
    eps_c = SETBIT(respKm11B->R2, 0);
    eps_v = SETBIT(respKm11B->E2, 0);
    eps_z = SETBIT(respKm11B->H1, 0);
    eps_n = SETBIT(respKm11B->L2, 0);
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



