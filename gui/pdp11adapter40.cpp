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

    // set manclock to initial state of "ManClock" ToggleButton. false  = not pressed
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

    auto msg = new RequestKM11SignalsWrite('A', 0); // empty template
    km11State.outputsToKM11AWriteRequest(msg); // encode outputs to KM11 pins
    wxGetApp().messageInterface->xmtRequest(msg); // send+delete
    // answer from M93X2probe is ResponseKM11Signals
    Pdp11Adapter::setManClkEnable(_manClkEnable); // actions same for all pdp11s
}

void Pdp11Adapter40::uStep()
{
    // send falling edge 
    km11State.mclk = 0;
    auto msg = new RequestKM11SignalsWrite();
    km11State.outputsToKM11AWriteRequest(msg);  // encode
    wxGetApp().messageInterface->xmtRequest(msg); // send+delete

    // send raising edge 
    km11State.mclk = 1;
    msg = new RequestKM11SignalsWrite();
    km11State.outputsToKM11AWriteRequest(msg);
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
        km11State.inputsFromKM11AResponse(km11Signals);

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
        km11State.inputsFromKM11BResponse(km11Signals);
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


