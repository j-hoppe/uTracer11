/*  pdp11adapter34 - base interface to physical and simulated PDP-11/34
*/

#include "pdp11adapter34.h"

#include "binary.h"

#include "application.h" // to reach pdp11 via wxGetApp().pdp11
#include "messages.h" // to reach pdp11 via wxGetApp().pdp11

void Pdp11Adapter34::setupGui(wxFileName _resourceDir) {
    auto app = &wxGetApp();

    _resourceDir.AppendDir("pdp1134");
    Pdp11Adapter::setupGui(_resourceDir);

    uFlowPanel = new Pdp11uFlowPanel(app->mainFrame->documentsNotebookFB);
    app->mainFrame->documentsNotebookFB->AddPage(uFlowPanel, "11/34 Micro program flow", false);

    controlwordPanel = new Pdp1134ControlwordPanel(app->mainFrame->documentsNotebookFB);
    app->mainFrame->documentsNotebookFB->AddPage(controlwordPanel, "Current micro word", false);

    dataPathPanel = new Pdp1134DataPathPanel(app->mainFrame->documentsNotebookFB);
    app->mainFrame->documentsNotebookFB->AddPage(dataPathPanel, "11/34 Data path", false);

}

// Set State of control, visibility and functions
void Pdp11Adapter34::updateGui(State state) {
    switch (state) {
    case State::init:
        return;
        break;
    case State::uMachineRunning:
        uFlowPanel->Disable();
        controlwordPanel->Disable();
        dataPathPanel->Disable();
        break;
    case State::uMachineManualStepping:
        uFlowPanel->Enable();
        controlwordPanel->Enable();
        dataPathPanel->Enable();
        break;
    case State::uMachineAutoStepping:
        uFlowPanel->Enable();
        controlwordPanel->Enable();
        dataPathPanel->Enable();
        break;
    }
    //   uFlowPanel->GetParent()->Layout() ;
   //    dataPathPanel->GetParent()->Layout() ;
    Pdp11Adapter::updateGui(state); // base
}


void Pdp11Adapter34::onInit() {
    Pdp11Adapter::onInit(); // stuff same for all models

    // generate messages to init gui, until first status updates comes in
    auto cpuSignals = ResponseKY11LBSignals(0, 0, 0, 0, 0);
    cpuSignals.process(); // calls virtual onRcvMessageFromPdp11() and updates GUI

    // setup bit fields. fieldnames like markers in XML! 
    controlWordFields = {
    ControlWordField(0,8,"0","NEXT_MPC_ADDRESS"),
    ControlWordField(9,11,"0","MISCELLANEOUS_CONTROL"),
    ControlWordField(12,12,"0","DATA_TRAN"),
    ControlWordField(13,14,"0","BUS_CONTROL"),
    ControlWordField(15,15,"0","ENAB_MAINT"),
    ControlWordField(16,16,"0","LOAD_BAR"),
    ControlWordField(17,17,"1","LONG_CYCLE_L"),
    ControlWordField(18,18,"0","AUX_CONTROL"),
    ControlWordField(19,23,"00101","ALU,BLEG_CONTROL"),
    ControlWordField(24,27,"0000","B,BX,OVX,DBE_CONTROL"),
    ControlWordField(28,29,"00","SSMUX_CONTROL"),
    ControlWordField(30,31,"01","AMUX_CONTROL"),
    ControlWordField(32,35,"0000","BUT_BITS"),
    ControlWordField(36,37,"00","SPA_SRC_SEL"),
    ControlWordField(38,39,"11","SPA_DST_SEL"),
    ControlWordField(40,40,"1","FORCE_RS_V_1_L"),
    ControlWordField(41,41,"1","PREVIOUS_MODE_L"),
    ControlWordField(42,42,"0","BUT_SERVICE"),
    ControlWordField(43,43,"0","FORCE_KERNEL"),
    ControlWordField(44,47,"0000","ROM_SPA")
    };

    loadControlStore(resourceDir, ".", "m8266-controlstore.xml");

    uflowPageAnnotations.loadXml(resourceDir, "mp00082-uflow", "mp00082-uflow.xml");
    // Check xml: key is 3 digit octal value
    for (auto itpa = uflowPageAnnotations.pageAnnotations.begin(); itpa != uflowPageAnnotations.pageAnnotations.end(); itpa++) {
        DocumentPageAnnotation* pa = &(itpa->second);
        size_t pos;
        (void)std::stoi(pa->key, &pos, 8); // try convert to octal, disable [[nodiscard]]
        if (pos != 3)
            wxLogError("Illegal octal key in 11/34 uflow.xml");
    }

    controlwordPageAnnotations.loadXml(resourceDir, "mp00082-controlword", "mp00082-controlword.xml");

    datapathPageAnnotations.loadXml(resourceDir, "mp00082-datapath", "mp00082-datapath.xml");

    // set manclock to initial state of "ManClock" ToggleButton. false  = not pressed
    auto _manClkEnable = wxGetApp().mainFrame->manClockEnableButton->GetValue();
    setManClkEnable(_manClkEnable); // state change

    // search in current uflow MPC data for keys of datapath objects
    // like "d:\retrocmp\dec\pdp11\uTracer11\resources\pdp1134\"
    uflowTodataPathKeyPatterns.loadXml(resourceDir, ".", "uflow2datapath.xml");
    auto controlKeyList = datapathPageAnnotations.getKeyList();
    uflowTodataPathKeyPatterns.assertKeysAgainst(controlKeyList, "uflow2datapath.xml must reference mp00082-datapath.xml");

    // verify matching of regexes to datafields
    // list all annotations datafields[2] with
    uflowPageAnnotations.dumpKeywordsForDatafields(uflowTodataPathKeyPatterns);
}

// called periodically
void Pdp11Adapter34::onTimer(unsigned periodMs) {
    timerUnprocessedMs += periodMs;
    bool pollSlow = (timerUnprocessedMs > 500);
    if (pollSlow)
        timerUnprocessedMs -= 500;

    // do not pollSlow data when pdp11 is running at own speed
    if (state == State::init || state == State::uMachineRunning)
        pollSlow = false;

    if (pollSlow) {
        // update ever 500ms new UNIBUS line status and mpc
        //
        // TODO: which signals are suppressed if not in single ustep mode?
        // which do you want to see "dancing" while CPU is running in real time?
        // request KY11 & unibussignals
        // response from M93X2probe is ResponseKY11LBSignals,ResponseUnibusSingals
        auto reqKy11 = new RequestKY11LBSignalsRead();
        wxGetApp().messageInterface->xmtRequest(reqKy11); // send+delete
        auto reqUnibus = new RequestUnibusSignalsRead();
        wxGetApp().messageInterface->xmtRequest(reqUnibus); // send+delete

        bool updateCpuStateVars = (state == State::uMachineManualStepping) || (state == State::uMachineAutoStepping);
        if (updateCpuStateVars && cpuStateVars.size() > 0) {
            // singlestepping: the pdp11 publishes its internal state, update it
            auto reqState = new RequestStateVal();
            wxGetApp().messageInterface->xmtRequest(reqState); // send+delete
        }
    }
}


// annotate the control word page. 
// key = 3 digit mpc
void Pdp11Adapter34::paintMicroStoreDocumentAnnotations(std::string key) {
    // skip before startup and if not visible
    if (controlwordPanel == nullptr || !controlwordPanel->IsShownOnScreen())
        return;

    // uword: for each mpc a list of 0s and 1s (48 bit field)
    // 1. paint "0" or "1" onto certain positions
    // 2. extract certain bit subfields (eg: 28..31 = SSMUX control)
    // 		this is the key for a geometry (box around SSMUX table entry)
    //controlwordPageAnnotations.paintScaled(key, uwordPanel);

    // for the moment static image, no geometries

    std::vector<std::string> keyList =
    { "BIT.00", "BIT.24", "BIT.47", "FIELDLABEL.NEXT_MPC_ADDRESS", "FIELDVALUE.ROM_SPA.0000" };

    controlwordPageAnnotations.paintScaled(keyList, controlwordPanel);
}

// paint datapath image. Several objects marked
// which are mentioned in current uflow annotation datafield
// get datafields for current upc
// key = 3 digit mpc
void Pdp11Adapter34::paintDatapathDocumentAnnotations(std::string key) {
    // skip before startup and if not visible
    if (dataPathPanel == nullptr || !dataPathPanel->IsShownOnScreen())
        return;

    auto dpa = uflowPageAnnotations.find(key);
    if (dpa == nullptr)
        return; // "file not found" display, no data for upc
    // get objects to mark
    //std::string upcTextDescription = dpa->dataFields[uflowMicroInstructionDataFieldIndex]; // 3rd field on upc box drawing
    std::vector<std::string> keyList;
    //uflowTodataPathKeyPatterns.getKeyListByMatch(upcTextDescription, keyList);
    uflowTodataPathKeyPatterns.getKeyListByMatch(dpa->dataFields, keyList);

    datapathPageAnnotations.paintScaled(keyList, dataPathPanel);
}

// called on microProgramCounter change or by mainframe on resize, notebook page flip
// paint all of the uflow, uword or datapath images
void Pdp11Adapter34::paintDocumentAnnotations() {
    // paints only on visible panels

    // key in the uFlow XML are 3-digit octal values
    std::string key = wxString::Format("%03o", microProgramCounter).ToStdString();

    // uflow: a single geometry for the mpc
    if (uFlowPanel != nullptr) // startup passed?
        uflowPageAnnotations.paintScaled(key, uFlowPanel);

    paintMicroStoreDocumentAnnotations(key);

    paintDatapathDocumentAnnotations(key);
}

void Pdp11Adapter34::setManClkEnable(bool _manClkEnable)
{
    auto msg = new RequestKY11LBSignalWrite("MCE", _manClkEnable);
    wxGetApp().messageInterface->xmtRequest(msg); // send+delete
    // answer from M93X2probe is ResponseKY11LBSignals
    Pdp11Adapter::setManClkEnable(_manClkEnable); // actions same for all pdp11s
}

void Pdp11Adapter34::uStep()
{
    auto msg = new RequestKY11LBSignalWrite("MC", 2/*Pulse*/);
    wxGetApp().messageInterface->xmtRequest(msg); // send+delete
    // answer from M93X2probe is ResponseKY11LBSignals
    Pdp11Adapter::uStepStart(); // actions same for all pdp11s
}

/*
// test: inject fake mpc, clock must be stopped
void Pdp11Adapter34::uStepAutoUntilStop(uint32_t stopUpc, int stopUnibusCycle, uint32_t stopUnibusAddress, int stopRepeatCount) {
    UNREFERENCED_PARAMETER(stopUpc);
    UNREFERENCED_PARAMETER(stopUnibusCycle);
    UNREFERENCED_PARAMETER(stopUnibusAddress);
    UNREFERENCED_PARAMETER(stopRepeatCount);
    auto msg = new ResponseKY11LBSignals(stopUpc, 0, 0, 0, 0);
    ignoreKY11 = false;
    onResponseKY11LBSignals(msg);
    ignoreKY11 = true;
    // hardware is still sending the real mpc, block evaluation
    delete msg;
}

*/

// 11/34 has no KM11 diag header, so this should never be called
void Pdp11Adapter34::onResponseKM11Signals(ResponseKM11Signals* km11Signals) {
    UNREFERENCED_PARAMETER(km11Signals);
    wxLogError("KM11 signals received for PDP11/34 ?");
}


// process KY11 signals, for mPC and other signals
// internally used for all 1134s, but not shown on simulation?
void Pdp11Adapter34::onResponseKY11LBSignals(ResponseKY11LBSignals* ky11Signals) {
    // update display in Notebook pages

    if (ignoreKY11)
        return;
    if (ky11StatusPanel) {
        wxString s;
        s = wxString::Format("%0.3o", ky11Signals->mpc);
        ky11StatusPanel->pdp1134Ky11MpcText->SetLabel(s);
        s = wxString::Format("%d", ky11Signals->fp11);
        ky11StatusPanel->pdp1134Ky11FpText->SetLabel(s);
        s = wxString::Format("%d", ky11Signals->sbf);
        ky11StatusPanel->pdp1134Ky11SbfText->SetLabel(s);
        s = wxString::Format("%d", ky11Signals->lir);
        ky11StatusPanel->pdp1134Ky11LirText->SetLabel(s);
        s = wxString::Format("%d", ky11Signals->pbp);
        ky11StatusPanel->pdp1134Ky11PbpText->SetLabel(s);
    }
    // process MPC events only for change
    ky11Signals->mpc &= 0x1ff; // strip off bit 10, to 0..511
    if (microProgramCounter != ky11Signals->mpc) {
        // MPC changed => new value, => event + display update
        doEvalMpc(ky11Signals->mpc);
    }
}

// Unibus-Signal panel same for all Pdp11s
// route message to Pdp11-model specific panel
void Pdp11Adapter34::onResponseUnibusSignals(ResponseUnibusSignals* unibusSignals) {
    doEvalUnibusSignals(unibusSignals); // same for all pdp11's
}

// Unibus-Signal panel same for all Pdp11s
// route message to Pdp11-model specific panel
void Pdp11Adapter34::evalUnibusCycle(ResponseUnibusCycle* unibusCycle)
{
    doEvalUnibusCycle(unibusCycle); // same for all pdp11's
}

