/*  pdp11adapter40 - base interface to physical and simulated PDP-11/40
*/

#include "pdp11adapter40.h"

#include "application.h" // to reach pdp11 via wxGetApp().pdp11
#include "messages.h" // to reach pdp11 via wxGetApp().pdp11


void Pdp11Adapter40::setupGui(wxFileName _resourceDir) {
    // generic panels before and optical left of specific ones
    _resourceDir.AppendDir("pdp1140");
    Pdp11Adapter::setupGui(_resourceDir);

    uFlowPanel = new Pdp11uFlowPanel(app->mainFrame->documentsNotebookFB);
    app->mainFrame->documentsNotebookFB->AddPage(uFlowPanel, "11/40 Micro program flow", false);

    //dataPathPanel = new Pdp1134DataPathPanel(app->mainFrame->documentsNotebookFB);
    //app->mainFrame->documentsNotebookFB->AddPage(dataPathPanel, "11/34 Data path", false);

    //pdp1134uWordPanel = new Pdp1134ControlwordPanel(app->mainFrame->documentsNotebookFxWB);
    //mainFrame->documentsNotebookFxWB->AddPage(pdp1134uWordPanel, "Current micro word", false);
}

// Set State of control, visibility and functions
void Pdp11Adapter40::updateGui(State state) {
    switch(state) {
    case State::init:
        return;
        break ;
    case State::uMachineRunning:
        uFlowPanel->Disable() ;
        break ;
    case State::uMachineManualStepping:
        uFlowPanel->Enable();
        break ;
    case State::scriptRunning:
        uFlowPanel->Enable() ;
        break ;
    }
    //uFlowPanel->GetParent()->Layout() ;
    Pdp11Adapter::updateGui(state); // base
}


void Pdp11Adapter40::onInit() {
    Pdp11Adapter::onInit(); // stuff same for all models

    // add KM11A/B signals to stateVars
    // mpc already registered, but tune length
    stateVars.get("MPC")->bitCount = 9 ; // 0..511
    stateVars.add(Variable("PUPP", "Prev MPC", 9, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("BUPP", "Basic MPC", 9, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("TRAP", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("SSYN", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("MSYN", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("T", "PSW Trace", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("C", "PSW Carry", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("V", "PSW Overflow", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("Z", "PSW Zero", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("N", "PSW Negative", 1, VariableType::signal | VariableType::trace)) ;

    // do not trace the KM11B stuff until needed
    stateVars.add(Variable("PBA", "MMU Base Addr", 18, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("ROM_ABCD", "", 4, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("ECIN00", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("EXP_UNFL", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("EXP_OVFL", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("DR00", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("DR09", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("MSR00", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("MSR01", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("EPS_C", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("EPS_V", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("EPS_Z", "", 1, VariableType::signal | VariableType::trace)) ;
    stateVars.add(Variable("EPS_N", "", 1, VariableType::signal | VariableType::trace)) ;
	displayStateVarsDefinition() ;

    // generate messages to init gui, until first status updates comes in
    // clear CPU state
    auto cpuSignalsA = ResponseKM11Signals(MsgTag::next, 'A', 0, 0, 0, 0);
    cpuSignalsA.process(); // calls virtual onRcvMessageFromPdp11() and updates GUI
    auto cpuSignalsB = ResponseKM11Signals(MsgTag::next, 'B', 0, 0, 0, 0);
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
    datapathPageAnnotations.loadXml(resourceDir, "mp00082-datapath", "mp00082-datapath.xml");

    // search in current uflow MPC data for keys of datapath objects
    // like "d:\retrocmp\dec\pdp11\uTracer11\resources\pdp1134\"
    uflowTodataPathKeyPatterns.loadXml(resourceDir, ".", "uflow2datapath.xml");
    auto controlKeyList = datapathPageAnnotations.getKeyList();
    uflowTodataPathKeyPatterns.assertKeysAgainst(controlKeyList, "uflow2datapath.xml must reference mp00082-datapath.xml");

    // verify matching of regexes to datafields
    // list all annotations datafields[2] with
    uflowPageAnnotations.dumpKeywordsForDatafields(uflowTodataPathKeyPatterns);
#endif

    // set manclock to initial state of "ManClock" ToggleButton. false  = not pressed
    auto _manClkEnable = app->mainFrame->manClockEnableButton->GetValue();
    setManClkEnable(_manClkEnable); // state change
}

// called periodically
void Pdp11Adapter40::onTimer(unsigned periodMs) {
    // update ever 500ms new UNIBUS line status and mpc
    //
    // request update every 0.5secs
    timerUnprocessedMs += periodMs;
    bool pollSlow = (timerUnprocessedMs > 500) ;
    if (pollSlow)
        timerUnprocessedMs -= 500;

    // do not pollSlow data when pdp11 is running at own speed
    if (state == State::init || state == State::uMachineRunning)
        pollSlow = false ;

    if (pollSlow) {
        // request KM11 & unibussignals
        // response from M93X2probe is ResponseKM11Signals,ResponseUnibusSignals
        auto reqKm11A = new RequestKM11SignalsRead(MsgTag::next, 'A');
        app->messageInterface->xmtRequest(reqKm11A); // send+delete
        auto reqKm11B = new RequestKM11SignalsRead(MsgTag::next, 'B');
        app->messageInterface->xmtRequest(reqKm11B); // send+delete
        auto reqUnibus = new RequestUnibusSignalsRead(MsgTag::next);
        app->messageInterface->xmtRequest(reqUnibus); // send+delete
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
    km11AState.mclk_enab = _manClkEnable;

    auto msg = new RequestKM11SignalsWrite(MsgTag::next, 'A', 0); // empty template
    km11AState.outputsToKM11AWriteRequest(msg); // encode outputs to KM11 pins
    app->messageInterface->xmtRequest(msg); // send+delete
    // answer from M93X2probe is ResponseKM11Signals
    Pdp11Adapter::setManClkEnable(_manClkEnable); // actions same for all pdp11s
}

void Pdp11Adapter40::requestUStep()
{
    // Momentary switch S4 on KM11 produces 1-0-1 on MCLK_L
    // -> 0-1-0 on positive km11State.mclk
    km11AState.mclk = 1; // assume 0 already set
    auto msg1 = new RequestKM11SignalsWrite(MsgTag::next);
    km11AState.outputsToKM11AWriteRequest(msg1);  // encode
    app->messageInterface->xmtRequest(msg1); // send+delete

    // send falling edge
    km11AState.mclk = 0;
    msg1 = new RequestKM11SignalsWrite(MsgTag::next);
    km11AState.outputsToKM11AWriteRequest(msg1);
    app->messageInterface->xmtRequest(msg1); // send+delete

    // answer from M93X2probe is ResponseKY11LBSignals

    auto msg2 = new RequestRegVal(MsgTag::next);
    app->messageInterface->xmtRequest(msg2); // send+delete
}

/*
// test: inject fake mpc, clock must be stopped
void Pdp11Adapter40::uStepAutoUntilStop(uint32_t stopUpc, int stopUnibusCycle, uint32_t stopUnibusAddress, int stopRepeatCount) {
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
*/

// visualize new MPC and other signals via KM11 A+B diag header
void Pdp11Adapter40::onResponseKM11Signals(ResponseKM11Signals* km11Signals) {
    // update display in Notebook pages
    if (ignoreKM11)
        return;

    wxString s;

    if (km11Signals->channel == 'A') {
        // parse
        km11AState.inputsFromKM11AResponse(km11Signals);

        // to state vars
        stateVars.get("PUPP")->setValue( km11AState.pupp) ;
        stateVars.get("BUPP")->setValue( km11AState.bupp) ;
        stateVars.get("TRAP")->setValue( km11AState.trap) ;
        stateVars.get("SSYN")->setValue( km11AState.ssyn) ;
        stateVars.get("MSYN")->setValue( km11AState.msyn) ;
        stateVars.get("T")->setValue( km11AState.t) ;
        stateVars.get("C")->setValue( km11AState.c) ;
        stateVars.get("V")->setValue( km11AState.v) ;
        stateVars.get("Z")->setValue( km11AState.z) ;
        stateVars.get("N")->setValue( km11AState.n) ;

        // process MPC events only for change
        if (microProgramCounter != km11AState.pupp) {
            // MPC changed => new value, => event + display update
            onResponseMpc(km11AState.pupp) ;
        }

        if (km11StatusPanel) { // not for simulation ?
            // to panel
            s = stateVars.get("BUPP")->valueText();
            km11StatusPanel->pdp1140Km11aBuppText->SetLabel(s);
            s = stateVars.get("PUPP")->valueText();
            km11StatusPanel->pdp1140Km11aPuppText->SetLabel(s);
            s = stateVars.get("TRAP")->valueText();
            km11StatusPanel->pdp1140Km11aTrapText->SetLabel(s);
            s = stateVars.get("SSYN")->valueText();
            km11StatusPanel->pdp1140Km11aSsynText->SetLabel(s);
            s = stateVars.get("MSYN")->valueText();
            km11StatusPanel->pdp1140Km11aMsynText->SetLabel(s);
            s = stateVars.get("C")->valueText();
            km11StatusPanel->pdp1140Km11aCText->SetLabel(s);
            s = stateVars.get("V")->valueText();
            km11StatusPanel->pdp1140Km11aVText->SetLabel(s);
            s = stateVars.get("Z")->valueText();
            km11StatusPanel->pdp1140Km11aZText->SetLabel(s);
            s = stateVars.get("N")->valueText();
            km11StatusPanel->pdp1140Km11aNText->SetLabel(s);
            s = stateVars.get("T")->valueText();
            km11StatusPanel->pdp1140Km11aTText->SetLabel(s);
        }
    }
    else if (km11Signals->channel == 'B') {
        // parse
        km11BState.inputsFromKM11BResponse(km11Signals);

        // to state vars
        stateVars.get("PBA")->setValue(km11BState.pba) ;
        int rom_abcd = 0 ; // bits->digits
        if (km11BState.rom_a)
            rom_abcd |= 010 ;
        if (km11BState.rom_b)
            rom_abcd |= 04 ;
        if (km11BState.rom_c)
            rom_abcd |= 02 ;
        if (km11BState.rom_d)
            rom_abcd |= 01 ;
        stateVars.get("ROM_ABCD")->setValue(rom_abcd);
        stateVars.get("ECIN00")->setValue( km11BState.ecin_00) ;
        stateVars.get("EXP_UNFL")->setValue( km11BState.exp_unfl) ;
        stateVars.get("EXP_OVFL")->setValue( km11BState.exp_ovfl) ;
        stateVars.get("DR00")->setValue( km11BState.dr00) ;
        stateVars.get("DR09")->setValue( km11BState.dr09) ;
        stateVars.get("MSR00")->setValue( km11BState.msr00) ;
        stateVars.get("MSR01")->setValue( km11BState.msr01) ;
        stateVars.get("EPS_C")->setValue( km11BState.eps_c) ;
        stateVars.get("EPS_V")->setValue( km11BState.eps_c) ;
        stateVars.get("EPS_Z")->setValue( km11BState.eps_c) ;
        stateVars.get("EPS_N")->setValue( km11BState.eps_c) ;
		displayStateVarsValues() ;

        if (km11StatusPanel) { // not for simulation ?
            // display
            s = stateVars.get("PBA")->valueText() ;
            km11StatusPanel->pdp1140Km11bPbaText->SetLabel(s);
            // ROM A-D!!!
            s = wxString::Format("%d%d", km11BState.rom_a, km11BState.rom_b);
            km11StatusPanel->pdp1140Km11bRomAbText->SetLabel(s);
            s = wxString::Format("%d", km11BState.rom_c);
            km11StatusPanel->pdp1140Km11bRomCText->SetLabel(s);
            s = wxString::Format("%d", km11BState.rom_d);
            km11StatusPanel->pdp1140Km11bRomDText->SetLabel(s);
            s = stateVars.get("B15")->valueText() ;
            km11StatusPanel->pdp1140Km11bB15Text->SetLabel(s);
            s = stateVars.get("ECIN00")->valueText() ;
            km11StatusPanel->pdp1140Km11bEcin00Text->SetLabel(s);
            s = stateVars.get("EXP_UNFL")->valueText() ;
            km11StatusPanel->pdp1140Km11bExpUnflText->SetLabel(s);
            s = stateVars.get("EXP_OVFL")->valueText() ;
            km11StatusPanel->pdp1140Km11bExpOvflText->SetLabel(s);
            s = stateVars.get("DR00")->valueText() ;
            km11StatusPanel->pdp1140Km11bDr00Text->SetLabel(s);
            s = stateVars.get("DR09")->valueText() ;
            km11StatusPanel->pdp1140Km11bDr09Text->SetLabel(s);
            s = stateVars.get("MSR00")->valueText() ;
            s = wxString::Format("%d", km11BState.msr00);
            km11StatusPanel->pdp1140Km11bMsr00Text->SetLabel(s);
            s = stateVars.get("MSR01")->valueText() ;
            km11StatusPanel->pdp1140Km11bMsr01Text->SetLabel(s);
            s = stateVars.get("EPS_C")->valueText() ;
            km11StatusPanel->pdp1140Km11bEpsCText->SetLabel(s);
            s = stateVars.get("EPS_V")->valueText() ;
            km11StatusPanel->pdp1140Km11bEpsVText->SetLabel(s);
            s = stateVars.get("EPS_Z")->valueText() ;
            km11StatusPanel->pdp1140Km11bEpsZText->SetLabel(s);
            s = stateVars.get("EPS_N")->valueText() ;
            km11StatusPanel->pdp1140Km11bEpsNText->SetLabel(s);
        }
    }
    else wxLogFatalError("Pdp11Adapter40::onResponseKM11Signals(): unknown channel %c", km11Signals->channel);

#ifdef TODO
    // process MPC events only for change
    auto eventPanel = app->eventPanel; // obsolete, all pdp11s use same event log
    ky11Signals->mpc &= 0x1ff; // strip off bit 10, to 0..511
    if (microProgramCounter != ky11Signals->mpc) {
        // MPC changed => new value, => event + display update
        microProgramCounter = ky11Signals->mpc;
        logEvent(eventPanel, "mpc = %0.3o", microProgramCounter);
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
    Pdp11Adapter::onResponseUnibusSignals(unibusSignals); // same for all pdp11's
}

// Unibus-Signal panel same for all Pdp11s
// route message to Pdp11-model specific panel
void Pdp11Adapter40::onResponseUnibusCycle(ResponseUnibusCycle* unibusCycle)
{
        Pdp11Adapter::onResponseUnibusCycle(unibusCycle); // same for all pdp11's
}


