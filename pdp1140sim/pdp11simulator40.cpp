/* simulator1140
- is connected to gui via ProbeInterface methods
- has full memory
- cloned from fake 1134 emu

From M9312 Field Maintenance Print Set (Oct 1978, MP00617).pdf

     269                                	; character RX routine
     270                                	;
     271                                	; R2 = new 7bit character returned in R2<7:0>, R2<15:08> n/c
     272                                	; R3 = return address
     273
     274 165524 105737  177560          rxchar:	tstb	@#dlrcsr		; wait for RX ready
     275 165530 100375                  	bpl	rxchar			; no character yet, loop
     276 165532 105002                  	clrb	r2			; clear low byte only
     277 165534 153702  177562          	bisb	@#dlrbuf,r2		; insert character in low byte

     upc 016
     ba=165524
     udata=
*/

#include "utils.h"
#include "pdp11simulator40.h"



const char* Pdp11Simulator40::getVersion()
{
    return "PDP11/40 simple test fake";
}

void Pdp11Simulator40::onRequestKY11LBSignalWrite(RequestKY11LBSignalWrite* requestKY11LBSignalWrite) {
    if (!strcasecmp(requestKY11LBSignalWrite->signalName, "MCE")) {
        // manual clock enable - machine stopped
        setMicroRunState(!requestKY11LBSignalWrite->val);
    }
    else if (!strcasecmp(requestKY11LBSignalWrite->signalName, "MC")) {
        // GUI wants micro step via KY11 header, raising sigal edge or pulse
        if (requestKY11LBSignalWrite->val > 0)
            microStep();
    }

    respond(new ResponseKY11LBSignals(mpc, 0, 0, 0, 0));
}

void Pdp11Simulator40::onRequestKY11LBSignalsRead(RequestKY11LBSignalsRead* requestKY11LBSignalsRead) {
    UNREFERENCED_PARAMETER(requestKY11LBSignalsRead);
    // GUI wants to see the current upc
    respond(new ResponseKY11LBSignals(mpc, 0, 0, 0, 0));
}

void  Pdp11Simulator40::onRequestUnibusDeposit(RequestUnibusDeposit* requestUnibusDeposit) {
    // GUI wants to change memory via DATO
    uint32_t addr = requestUnibusDeposit->addr;
    uint16_t data = requestUnibusDeposit->data;
    if (addr >= 0x1f000) // not existing memory
        // respond with captured cycle
        respond(new ResponseUnibusCycle(/*DATO*/2, addr, 0, /*NXM*/true, /*requested*/true));
    else {
        memory[requestUnibusDeposit->addr] = data;
        // respond with captured state
        respond(new ResponseUnibusCycle(/*DATO*/2, addr, data, /*NXM*/false, /*requested*/true));
    }
}

void  Pdp11Simulator40::onRequestUnibusExam(RequestUnibusExam* requestUnibusExam) {
    // GUI wants to see memory via DATI
    uint32_t addr = requestUnibusExam->addr;
    if (addr >= 0x1f000) // not existing memory
        // respond with captured cycle
        respond(new ResponseUnibusCycle(/*DATI*/0, addr, 0, /*NXM*/true, /*requested*/true));
    else {
        uint16_t data = memory[requestUnibusExam->addr];
        // respond with captured state
        respond(new ResponseUnibusCycle(/*DATI*/0, addr, data, /*NXM*/false, /*requested*/true));
    }
}


// the simulator should do ACLO/DCLO or other signals, which causes CPU traps and restart
void Pdp11Simulator40::onRequestUnibusSignalWrite(RequestUnibusSignalWrite* requestUnibusSignalWrite)
{
    char* signalName = requestUnibusSignalWrite->signalName;
    uint32_t val = requestUnibusSignalWrite->val; // aliases

    // update simulated UNIBUS state
    unibusSignals.setSignal(signalName, val);

    // let the simulator process changes signals lines
    if (!strcasecmp(signalName, "DCLO")) {
        if (val) {
            // if DCLO goes active: trap, stop CPU
        }
        else {
            // if DCLO goes inactive: power up sequence
            setup(); // full reset, todo: emulated power-on
        }
    }

    // echo UNIBUS state back
    // make a copy, as it is deleted after sending

    auto response = new ResponseUnibusSignals(unibusSignals);
    respond(response);
}




/*
upc  000       service trap

upc 016 fetch next instr

dati 765524 -> 105737

015    incr pc
bracnh to TSTB =Single operand SOP (B) NON MODE
PC indirect addr = DM3 auto inc defreeded via pc

upc = 163
pc now 165526

dati 765526 -> 177560      addr of DL11 console recvr

upc 307 pc= pc+2  , now 165530

upc 311 fetch dest addr
dati 777560 -  0

-> NON MOD AUX
265 load cc

---------
upc  000       service trap

upc 016 fetch next instr

dati 765530 -> 100375

015    incr pc

status bit 7 od LD11 is 0 - BPL jump to  BRANCH CHANGE

upc 200

upc 522

upc 523
*/

void Pdp11Simulator40::setMicroRunState(bool state)
{
    microRunState = state;
}

void Pdp11Simulator40::microStep() {
    unsigned nextMpc = 0;
	mcyclecount++ ;

    switch (state) {
    case 0: // mpc 000 service trap
        nextMpc = 0016;
        state++;
        break;
        // execute tstb	@#dl11.rcsr
    case 1: // mpc = 016 fetch next instr
        unibusSignals.addr = ba = 0765524 ;
        onCpuUnibusCycle(/*DATI*/0, 0765524, 0105737, false);
        nextMpc = 0015;
        state++;
		opcodecount++ ; // execution starts
        break;
    case 2: // mpc = 015    incr pc
        unibusSignals.addr = ba = 0 ;
		// branch to TSTB = Single operand SOP(B) NON MODE
        // PC indirect addr = DM3 auto inc deferred via pc
        nextMpc = 0163;
        r[7] = 0165526 ; // PC
        state++;
        break;
    case 3: //  upc = 163,  pc now 165526
        //  fetch addr of DL11 console recvr
        // Single operand SOP(B) NON MODE
		unibusSignals.addr = ba = 0765526 ;
        onCpuUnibusCycle(/*DATI*/0, 0765526, 0177560, false);
        nextMpc = 0307;
        state++;
        break;
    case 4: //  upc 307 pc = pc + 2,
        unibusSignals.addr = ba = 0 ; // bus idle
        r[7] = 0165530 ; // PC inc'ed
        nextMpc = 0311;
        state++;
        break;
    case 5: // mpc = 311, mem of fetched DL11 register
        //  no char received, no flags in DL11 receiver status
        r[7] = 0165532 ; // PC inc'ed
        unibusSignals.addr = ba = 0777560 ;
        onCpuUnibusCycle(/*DATI*/0, 0777560, 000000, false); //
        nextMpc = 0265; // step NON MOD AUX
        state++;
        break;
    case 6: // mpc = 265 NON MOD AUX, load CC
        unibusSignals.addr = ba = 0 ; // bus idle
        nextMpc = 0000;
        //
        state++;
        break;
    case 7: // mpc 0000 service trap
        nextMpc = 0016;
        state++;
        // execute  bpl	rxchar
        break;
    case 8: // mpc = 016  fetch next instr
    	unibusSignals.addr = ba = 0765530 ;
        onCpuUnibusCycle(/*DATI*/0, 0765530, 0100375, false);
        nextMpc = 0015;
        state++;
        break;
    case 9: // mpc 015 incr pc,
        unibusSignals.addr = ba = 0 ; // bus idle
        r[7] = 0165534 ; // PC inc'ed
        nextMpc = 0200;
        // status bit 7 of DL11 is 0 - BPL jumps to  BRANCH CHANGE
        state++;
        break;
    case 10: // mpc = 200
        nextMpc = 0522;
        state++;
        break;
    case 11: // mpc = 522
        nextMpc = 0523;
        state++;
        break;
    case 12: // mpc = 523
        // add branch displacement to pc
        r[7] = 0165524 ; // PC set to "loop" jump label
        nextMpc = 0000;
        state = 0; // loop
        break;
    }
    mpc = nextMpc;
}




void Pdp11Simulator40::setup() {
    Pdp11Simulator::setup();
    mpc = 0;
	opcodecount = 0;
	mcyclecount = 0 ;
    state = 0;
    unibusSignals = UnibusSignals(); // re-init with all 0s
    r[0] = r[1] = r[2]= r[3]= r[4]= r[5]= r[6]= r[7] = 0 ;
    ba = 0;

	// prepare for publishing
	stateVarRegisterClear() ;
	stateVarRegister("MPC", &mpc, sizeof(mpc), 16 );
	stateVarRegister("R0", &(r[0]), sizeof(r[0]), 16 );
	stateVarRegister("R1", &(r[1]), sizeof(r[1]), 16 );
	stateVarRegister("R2", &(r[2]), sizeof(r[2]), 16 );
	stateVarRegister("R3", &(r[3]), sizeof(r[3]), 16 );
	stateVarRegister("R4", &(r[4]), sizeof(r[4]), 16 );
	stateVarRegister("R5", &(r[5]), sizeof(r[5]), 16 );
	stateVarRegister("SP", &(r[6]), sizeof(r[6]), 16 );
	stateVarRegister("PC", &(r[7]), sizeof(r[7]), 16 );
	stateVarRegister("BA", &ba, sizeof(ba), 18 );
	stateVarRegister("OPCODES", &opcodecount, sizeof(opcodecount), 32);
	stateVarRegister("CYCLES", &mcyclecount, sizeof(mcyclecount), 32);
}

void Pdp11Simulator40::loop() {
	while(true) {
		processPendingRequests() ;
	}
}






