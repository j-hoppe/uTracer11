/*  pdp11adapter40 - base interface to physical and simulated PDP-11/40
*/

#include "pdp11adapter40.h"

#include "application.h" // to reach pdp11 via wxGetApp().pdp11
#include "messages.h" // to reach pdp11 via wxGetApp().pdp11


void Pdp11Adapter40::setupGui(wxFileName _resourceDir) {
    auto app = &wxGetApp();

    // generic panels before and optical left of specific ones
    _resourceDir.AppendDir("pdp1140");
    Pdp11Adapter::setupGui(_resourceDir);

    uFlowPanel = new Pdp11uFlowPanel(app->mainFrame->documentsNotebookFB);
    app->mainFrame->documentsNotebookFB->AddPage(uFlowPanel, "11/40 Micro program flow", false);

    //dataPathPanel = new Pdp1134DataPathPanel(app->mainFrame->documentsNotebookFB);
    //app->mainFrame->documentsNotebookFB->AddPage(dataPathPanel, "11/34 Data path", false);

    //pdp1134uWordPanel = new Pdp1134uWordPanel(app->mainFrame->documentsNotebookFxWB);
    //mainFrame->documentsNotebookFxWB->AddPage(pdp1134uWordPanel, "Current micro word", false);

}

void Pdp11Adapter40::onInit() {
    Pdp11Adapter::onInit(); // stuff same for all models

    // generate messages to init gui, until first status updates comes in
    // clear CPU state
    auto cpuSignalsA = ResponseKM11Signals('A', 0, 0, 0, 0);
    cpuSignalsA.process(); // calls virtual onRcvMessageFromPdp11() and updates GUI
    auto cpuSignalsB = ResponseKM11Signals('B', 0, 0, 0, 0);
    cpuSignalsB.process(); // calls virtual onRcvMessageFromPdp11() and updates GUI

    uflowPageAnnotations.loadXml(resourceDir, "KD11-A_Processor_Flow_Diagrams", "KD11-A_Processor_Flow_Diagrams.xml");

    // Check xml: key is 3 digit octal value
    for (auto itpa = uflowPageAnnotations.pageAnnotations.begin(); itpa != uflowPageAnnotations.pageAnnotations.end(); itpa++) {
        DocumentPageAnnotation* pa = &(itpa->second);
        size_t pos;
        (void)std::stoi(pa->key, &pos, 8); // try convert to octal, disable [[nodiscard]]
        if (pos != 3)
            wxLogError("Illegal octal key in 11/40 uflow.xml");
    }


#ifdef TODO
    // Check xml: key is 3 digit octal value
    for (auto itpa = uflowPageAnnotations.pageAnnotations.begin(); itpa != uflowPageAnnotations.pageAnnotations.end(); itpa++) {
        DocumentPageAnnotation* pa = &(itpa->second);
        size_t pos;
        std::stoi(pa->key, &pos, 8); // try convert to octal
        if (pos != 3)
            wxLogError("Illegal octal key in 11/40 uflow.xml");
    }

    datapathPageAnnotations.loadXml(resourceDir, "mp00082-datapath", "mp00082-datapath.xml");

    // set manclock to initial state of ToggleButton
    auto _manClkEnable = wxGetApp().mainFrame->manClockEnableButton->GetValue();
    setManClkEnable(_manClkEnable);

    // search in current uflow MPC data for keys of datapath objects
    // like "d:\retrocmp\dec\pdp11\uTracer11\resources\pdp1134\"
    uflowTodataPathKeyPatterns.loadXml(resourceDir, ".", "uflow2datapath.xml");
    auto controlKeyList = datapathPageAnnotations.getKeyList();
    uflowTodataPathKeyPatterns.assertKeysAgainst(controlKeyList, "uflow2datapath.xml must reference mp00082-datapath.xml");

    // verify matching of regexes to datafields
    // list all annotations datafields[2] with
    uflowPageAnnotations.dumpKeywordsForDatafields(uflowTodataPathKeyPatterns);
#endif
}

// called periodically
void Pdp11Adapter40::onTimer(unsigned periodMs) {
    // update ever 500ms new UNIBUS line status and mpc
    //
    // request update every 0.5secs
    timerUnprocessedMs += periodMs;
    if (timerUnprocessedMs > 500) {
        timerUnprocessedMs -= 500;
        // request KM11 & unibussignals
        // response from M93X2probe is ResponseKM11Signals,ResponseUnibusSignals
        auto reqKm11A = new RequestKM11SignalsRead('A');
        wxGetApp().messageInterface->xmtRequest(reqKm11A); // send+delete
        auto reqKm11B = new RequestKM11SignalsRead('B');
        wxGetApp().messageInterface->xmtRequest(reqKm11B); // send+delete
        auto reqUnibus = new RequestUnibusSignalsRead();
        wxGetApp().messageInterface->xmtRequest(reqUnibus); // send+delete
    }
}


// called on microProgramCounter change or by mainframe on resize, notebook page flip
// paint all of the uflow, uword or datapath images
void Pdp11Adapter40::paintDocumentAnnotations() {
    // paints only on visible panels

    // key in the uFlow XML are 3-digit octal values
    std::string key = wxString::Format("%03o", microProgramCounter).ToStdString();
    uflowPageAnnotations.paintScaled(key, uFlowPanel);
#ifdef TODO
    // paint datapath image. Several objects marked
    // wich are mentioned in current uflow annotaion datafield
    // get datafields for current upc
    auto dpa = uflowPageAnnotations.find(key);
    if (dpa == nullptr)
        return; // "file not found" display, no data for upc
    // get objects to mark
    //std::string upcTextDescription = dpa->dataFields[uflowMicroInstructionDataFieldIndex]; // 3rd field on upc box drawing
    std::vector<std::string> dataPathKeyList;
    //uflowTodataPathKeyPatterns.getKeyListByMatch(upcTextDescription, dataPathKeyList);
    uflowTodataPathKeyPatterns.getKeyListByMatch(dpa->dataFields, dataPathKeyList);

    datapathPageAnnotations.paintScaled(dataPathKeyList, datapathPanel);
#endif
}

void Pdp11Adapter40::setManClkEnable(bool _manClkEnable)
{
    km11State.mclk_enab = _manClkEnable;

    auto msg = new RequestKM11SignalsWrite('A', manClkEnable);
    km11State.outputsToKM11A(msg); // encode
    wxGetApp().messageInterface->xmtRequest(msg); // send+delete
    // answer from M93X2probe is ResponseKM11Signals
    Pdp11Adapter::setManClkEnable(_manClkEnable); // actions same for all pdp11s
}

void Pdp11Adapter40::uStep()
{
    // send falling edge 
    km11State.mclk = 0;
    auto msg = new RequestKM11SignalsWrite();
    km11State.outputsToKM11A(msg);  // encode
    wxGetApp().messageInterface->xmtRequest(msg); // send+delete

    // send raising edge 
    km11State.mclk = 1;
    msg = new RequestKM11SignalsWrite();
    km11State.outputsToKM11A(msg);
    wxGetApp().messageInterface->xmtRequest(msg); // send+delete

    // answer from M93X2probe is ResponseKY11LBSignals
    Pdp11Adapter::uStepStart(); // actions same for all pdp11s
}

// test: inject fake mpc, clock must be stopped
void Pdp11Adapter40::uStepUntilStop(uint32_t stopUpc, int stopUnibusCycle, uint32_t stopUnibusAddress, int stopRepeatCount) {
    UNREFERENCED_PARAMETER(stopUpc);
    UNREFERENCED_PARAMETER(stopUnibusCycle);
    UNREFERENCED_PARAMETER(stopUnibusAddress);
    UNREFERENCED_PARAMETER(stopRepeatCount);

    // construct a response indicating the given upc
   // PUPP bits on KM11 in gpio0a: <2><1><0><5><4><3><8><7><6>
    // gpio0a = 0 <5> <4> <3> 0 < 8 > <7> <6>
    // gpio0b = 00000 <2> <1> <0> 0

    unsigned bits876 = (stopUpc >> 6) & 7;
    unsigned bits543 = (stopUpc >> 3) & 7;
    unsigned bits210 = stopUpc & 7;
    uint8_t gpio0a = bits876 | (bits543 << 4);
    uint8_t gpio0b = bits210 << 1;

    auto msg = new ResponseKM11Signals('A', gpio0a, gpio0b, 0, 0);
    ignoreKM11 = false;
    onResponseKM11Signals(msg);
    ignoreKM11 = true;
    // hardware is still sending the real mpc, block evaluation
    delete msg;
}


// visualize new MPC and other signals via KM11 A+B diag header
void Pdp11Adapter40::onResponseKM11Signals(ResponseKM11Signals* km11Signals) {
    // update display in Notebook pages
    if (ignoreKM11)
        return;

    wxString s;

    if (km11Signals->channel == 'A') {
        // parse
        km11State.inputsFromKM11A(km11Signals);

        // process MPC events only for change
        if (microProgramCounter != km11State.pupp) {
            // MPC changed => new value, => event + display update
            microProgramCounter = km11State.pupp;
            doLogEvent("mpc = %0.3o", microProgramCounter);
            // repaint document pages only on change
            paintDocumentAnnotations();
            uStepComplete(microProgramCounter);
        }


        if (km11StatusPanel) { // not for simulation ?
            // display
            s = wxString::Format("%0.3o", km11State.pupp);
            km11StatusPanel->pdp1140Km11aPuppText->SetLabel(s);
            s = wxString::Format("%0.3o", km11State.bupp);
            km11StatusPanel->pdp1140Km11aBuppText->SetLabel(s);
            s = wxString::Format("%d", km11State.trap);
            km11StatusPanel->pdp1140Km11aTrapText->SetLabel(s);
            s = wxString::Format("%d", km11State.ssyn);
            km11StatusPanel->pdp1140Km11aSsynText->SetLabel(s);
            s = wxString::Format("%d", km11State.msyn);
            km11StatusPanel->pdp1140Km11aMsynText->SetLabel(s);
            s = wxString::Format("%d", km11State.c);
            km11StatusPanel->pdp1140Km11aCText->SetLabel(s);
            s = wxString::Format("%d", km11State.v);
            km11StatusPanel->pdp1140Km11aVText->SetLabel(s);
            s = wxString::Format("%d", km11State.z);
            km11StatusPanel->pdp1140Km11aZText->SetLabel(s);
            s = wxString::Format("%d", km11State.n);
            km11StatusPanel->pdp1140Km11aNText->SetLabel(s);
            s = wxString::Format("%d", km11State.t);
            km11StatusPanel->pdp1140Km11aTText->SetLabel(s);
        }
    }
    else if (km11Signals->channel == 'B') {
        // parse
        km11State.inputsFromKM11B(km11Signals);
        if (km11StatusPanel) { // not for simulation ?
            // display
            s = wxString::Format("%0.6o", km11State.pba);
            km11StatusPanel->pdp1140Km11bPbaText->SetLabel(s);
            s = wxString::Format("%d%d", km11State.rom_a, km11State.rom_b);
            km11StatusPanel->pdp1140Km11bRomAbText->SetLabel(s);
            s = wxString::Format("%d", km11State.rom_c);
            km11StatusPanel->pdp1140Km11bRomCText->SetLabel(s);
            s = wxString::Format("%d", km11State.rom_d);
            km11StatusPanel->pdp1140Km11bRomDText->SetLabel(s);
            s = wxString::Format("%d", km11State.b_15);
            km11StatusPanel->pdp1140Km11bB15Text->SetLabel(s);
            s = wxString::Format("%d", km11State.ecin_00);
            km11StatusPanel->pdp1140Km11bEcin00Text->SetLabel(s);
            s = wxString::Format("%d", km11State.exp_unfl);
            km11StatusPanel->pdp1140Km11bExpUnflText->SetLabel(s);
            s = wxString::Format("%d", km11State.exp_ovfl);
            km11StatusPanel->pdp1140Km11bExpOvflText->SetLabel(s);
            s = wxString::Format("%d", km11State.dr00);
            km11StatusPanel->pdp1140Km11bDr00Text->SetLabel(s);
            s = wxString::Format("%d", km11State.dr09);
            km11StatusPanel->pdp1140Km11bDr09Text->SetLabel(s);
            s = wxString::Format("%d", km11State.msr00);
            km11StatusPanel->pdp1140Km11bMsr00Text->SetLabel(s);
            s = wxString::Format("%d", km11State.msr01);
            km11StatusPanel->pdp1140Km11bMsr01Text->SetLabel(s);
            s = wxString::Format("%d", km11State.eps_c);
            km11StatusPanel->pdp1140Km11bEpsCText->SetLabel(s);
            s = wxString::Format("%d", km11State.eps_v);
            km11StatusPanel->pdp1140Km11bEpsVText->SetLabel(s);
            s = wxString::Format("%d", km11State.eps_z);
            km11StatusPanel->pdp1140Km11bEpsZText->SetLabel(s);
            s = wxString::Format("%d", km11State.eps_n);
            km11StatusPanel->pdp1140Km11bEpsNText->SetLabel(s);
        }
    }
    else wxLogFatalError("Pdp11Adapter40::onResponseKM11Signals(): unknown channel %c", km11Signals->channel);

#ifdef TODO
    // process MPC events only for change
    auto eventPanel = wxGetApp().eventPanel; // obsolete, all pdp11s use same event log
    ky11Signals->mpc &= 0x1ff; // strip off bit 10, to 0..511
    if (microProgramCounter != ky11Signals->mpc) {
        // MPC changed => new value, => event + display update
        microProgramCounter = ky11Signals->mpc;
        doLogEvent(eventPanel, "mpc = %0.3o", microProgramCounter);
        // repaint document pages only on change
        paintDocumentAnnotations();

        doUStepComplete(ky11Signals->mpc);
        }
#endif
    }

// 11/40 has no KY11 diag header, so this should never be called
void Pdp11Adapter40::onResponseKY11LBSignals(ResponseKY11LBSignals* ky11Signals) {
    UNREFERENCED_PARAMETER(ky11Signals);
    wxLogError("KY11-LB signals received for PDP11/40 ?");
}

// Unibus-Signal panel same for all Pdp11s
// route message to Pdp11-model specific panel
void Pdp11Adapter40::onResponseUnibusSignals(ResponseUnibusSignals* unibusSignals) {
    doEvalUnibusSignals(unibusSignals);
}

// Unibus-Signal panel same for all Pdp11s
// route message to Pdp11-model specific panel
void Pdp11Adapter40::evalUnibusCycle(ResponseUnibusCycle* unibusCycle)
{
    doEvalUnibusCycle(unibusCycle); // same for all pdp11's
}

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

// get a KM11 signal bit from the matrix andd shift it
#define bit(bitmask,shift) ( bitmask ? (1 << shift):0 )

void Pdp1140KM11State::inputsFromKM11A(ResponseKM11Signals* respKm11A)
{
    if (respKm11A->channel != 'A')
        wxLogFatalError("Pdp1140KM11Signals::inputsFromKM11A: channel A expected, is %c", respKm11A->channel);
    // for bit-encoding see PCB of M93X2probe

    // let the compiler do all the demuxing
    auto in00 = respKm11A->gpio0a & 1;
    auto in01 = respKm11A->gpio0a & 2;
    auto in02 = respKm11A->gpio0a & 4;
    //auto in03 = respKm11A->gpio0a & 8; NC
    auto in10 = respKm11A->gpio0a & 0x10;
    auto in11 = respKm11A->gpio0a & 0x20;
    auto in12 = respKm11A->gpio0a & 0x40;

    //auto in13 = respKm11A->gpio0b & 1; NC
    auto in20 = respKm11A->gpio0b & 2;
    auto in21 = respKm11A->gpio0b & 4;
    auto in22 = respKm11A->gpio0b & 8;
    auto in23 = respKm11A->gpio0b & 0x10;
    auto in30 = respKm11A->gpio0b & 0x20;
    auto in31 = respKm11A->gpio0b & 0x40;

    auto in32 = respKm11A->gpio1a & 1;
    auto in33 = respKm11A->gpio1a & 2;
    auto in40 = respKm11A->gpio1a & 4;
    auto in41 = respKm11A->gpio1a & 8;
    auto in42 = respKm11A->gpio1a & 0x10;
    auto in43 = respKm11A->gpio1a & 0x20;
    auto in50 = respKm11A->gpio1a & 0x40;

    auto in51 = respKm11A->gpio1b & 1;
    auto in52 = respKm11A->gpio1b & 2;
    auto in53 = respKm11A->gpio1b & 4;
    auto in60 = respKm11A->gpio1b & 8;
    auto in61 = respKm11A->gpio1b & 0x10;
    auto in62 = respKm11A->gpio1b & 0x20;
    auto in63 = respKm11A->gpio1b & 0x40;

    pupp = bit(in00, 6) | bit(in01, 7) | bit(in02, 8)
        | bit(in10, 3) | bit(in11, 4) | bit(in12, 5)
        | bit(in20, 0) | bit(in21, 1) | bit(in22, 2);
    bupp = bit(in30, 6) | bit(in31, 7) | bit(in32, 8)
        | bit(in40, 3) | bit(in41, 4) | bit(in42, 5)
        | bit(in50, 0) | bit(in51, 1) | bit(in52, 2);
    trap = bit(in23, 0);
    ssyn = bit(in33, 0);
    msyn = bit(in43, 0);
    t = bit(in53, 0);
    c = bit(in60, 0);
    v = bit(in61, 0);
    z = bit(in62, 0);
    n = bit(in63, 0);
}

void Pdp1140KM11State::inputsFromKM11B(ResponseKM11Signals* respKm11B)
{
    if (respKm11B->channel != 'B')
        wxLogFatalError("Pdp1140KM11Signals::inputsFromKM11B: channel B expected, is %c", respKm11B->channel);
    // let the compiler do all the demuxing
    auto in00 = respKm11B->gpio0a & 1;
    auto in01 = respKm11B->gpio0a & 2;
    auto in02 = respKm11B->gpio0a & 4;
    auto in03 = respKm11B->gpio0a & 8;
    auto in10 = respKm11B->gpio0a & 0x10;
    auto in11 = respKm11B->gpio0a & 0x20;
    auto in12 = respKm11B->gpio0a & 0x40;

    auto in13 = respKm11B->gpio0b & 1;
    auto in20 = respKm11B->gpio0b & 2;
    auto in21 = respKm11B->gpio0b & 4;
    auto in22 = respKm11B->gpio0b & 8;
    auto in23 = respKm11B->gpio0b & 0x10;
    auto in30 = respKm11B->gpio0b & 0x20;
    auto in31 = respKm11B->gpio0b & 0x40;

    auto in32 = respKm11B->gpio1a & 1;
    auto in33 = respKm11B->gpio1a & 2;
    auto in40 = respKm11B->gpio1a & 4;
    auto in41 = respKm11B->gpio1a & 8;
    auto in42 = respKm11B->gpio1a & 0x10;
    auto in43 = respKm11B->gpio1a & 0x20;
    auto in50 = respKm11B->gpio1a & 0x40;

    auto in51 = respKm11B->gpio1b & 1;
    auto in52 = respKm11B->gpio1b & 2;
    auto in53 = respKm11B->gpio1b & 4;
    auto in60 = respKm11B->gpio1b & 8;
    auto in61 = respKm11B->gpio1b & 0x10;
    auto in62 = respKm11B->gpio1b & 0x20;
    auto in63 = respKm11B->gpio1b & 0x40;

    pba = bit(in00, 15) | bit(in01, 16) | bit(in02, 17)
        | bit(in10, 12) | bit(in11, 13) | bit(in12, 14)
        | bit(in20, 9) | bit(in21, 10) | bit(in22, 11)
        | bit(in30, 6) | bit(in31, 7) | bit(in32, 8);
    rom_a = bit(in03, 0);
    rom_b = bit(in13, 0);
    rom_c = bit(in23, 0);
    rom_d = bit(in33, 0);
    b_15 = bit(in40, 0);
    ecin_00 = bit(in41, 0);
    exp_unfl = bit(in42, 0);
    exp_ovfl = bit(in43, 0);
    dr00 = bit(in50, 0);
    dr09 = bit(in51, 0);
    msr00 = bit(in52, 0);
    msr01 = bit(in53, 0);
    eps_c = bit(in60, 0);
    eps_v = bit(in61, 0);
    eps_z = bit(in62, 0);
    eps_n = bit(in63, 0);
}

void Pdp1140KM11State::outputsToKM11A(RequestKM11SignalsWrite* reqKm11A)
{
    reqKm11A->channel = 'A';
	// COmapte 11/40 KM11 overlay paper with "OUT" signals  in messages.txt
	// out00 = ---    out01 = MSTOP
	// out10 = MCLK   out11 = MCLK ENAB 
	auto out00 = 0;
    auto out01 = mstop;
    auto out10 = mclk;
    auto out11 = mclk_enab;
    reqKm11A->val03 = bit(out00, 0) | bit(out01, 1) | bit(out10, 2) | bit(out11, 3);
}
