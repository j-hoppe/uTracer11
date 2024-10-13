/* pdp11Adapter - logic between a target PDP11 and wxWidgets GUI
 In the model-view-controller pattern:
 model = target PDP11 physical (via M93X2probe) or simulator
 view = wxWidget GUI
 controller - pdp11adapter

Abstract base class
 Sub classes needed for
 11/40 with KM11,
 11/40 simulation (angelo)
 11/34 with KY11
 and others

*/


#include "pdp11adapter.h"
#include "application.h"
#include <map> // std headers after wx.h, else tons of C4996 strcpy()
#include <vector>

#include "pdp11adapter34phys.h"
#include "pdp11adapter40phys.h"
#include "pdp11adapter40sim.h"



// class factory, generate Pdp11 instance by class name via type enum
static std::map<std::string, enum Pdp11Adapter::Type> typeNames{
{"none", Pdp11Adapter::Type::none},
{"pdp1134phys", Pdp11Adapter::Type::pdp1134phys},
{"pdp1134sim", Pdp11Adapter::Type::pdp1134sim},
{"pdp1140phys", Pdp11Adapter::Type::pdp1140phys},
{"pdp1140sim", Pdp11Adapter::Type::pdp1140sim}
};

// string->enum
enum Pdp11Adapter::Type Pdp11Adapter::parseTypeName(std::string typeName) {
    auto it = typeNames.find(typeName);
    if (it != typeNames.end())
        return it->second;
    else return Pdp11Adapter::Type::none;
}

// list of all PDP11 names
std::vector<std::string> Pdp11Adapter::getTypeNames() {
    std::vector<std::string> result;
    for (auto it = typeNames.begin(); it != typeNames.end(); it++)
        result.push_back(it->first);
    return result;
}


Pdp11Adapter::Pdp11Adapter()
{
}

void Pdp11Adapter::setupGui(wxFileName _resourceDir)
{
    Application* app = &wxGetApp();
    // generic panels before and optical left of specific ones
    resourceDir = _resourceDir;

    //// Main Notebook
    // UNIBUS memory leftmost
    memoryPanel = new MemoryPanel(app->mainFrame->documentsNotebookFB); // good place?
    app->mainFrame->documentsNotebookFB->AddPage(memoryPanel, "Memory", false);

    // 0 to 256kb-8kb)
    memoryGridController.init(memoryPanel->memoryGridFB, &memoryimage, 0, MEMORY_IOPAGE_ADDRESS);
    // upper 8kb
    ioPageGridController.init(memoryPanel->ioPageGridFB, &memoryimage, MEMORY_IOPAGE_ADDRESS, MEMORY_END_ADDRESS);

    tracePanel = new TracePanel(app->mainFrame->documentsNotebookFB); // good place?
    app->mainFrame->documentsNotebookFB->AddPage(tracePanel, "Trace", false);
    traceController.init(tracePanel->traceGridFB, this, app->cmdLineResourceSymbolFilePath);

    //// Status panel
    unibusSignalsPanel = new UnibusSignalsPanelFB(app->mainFrame->pdp11StatusPanelFB);
    app->mainFrame->pdp11StatusSizerFB->Add(unibusSignalsPanel, 1, wxALL, 5);

    /// Internal PDP11 simulator variables
    internalStatePanel = new InternalStatePanelFB(app->mainFrame->pdp11StatusPanelFB);
    app->mainFrame->pdp11StatusSizerFB->Add(internalStatePanel, 1, wxALL, 5);
    // set "Visible" if request/responseStateDef indicates same state
    internalStatePanel->Show(false);
    app->mainFrame->pdp11StatusSizerFB->Layout();
}

// Set State of common controls, visibility and functions
void Pdp11Adapter::updateGui(State newState) {
    doLogEvent("State change from %d to %d", state, newState); // same for all pdp11's
    state = newState;
    Application* app = &wxGetApp();
    auto infoLabel = app->mainFrame->uMachineStateText; // fat state
    auto infoLabel2 = app->mainFrame->uMachineStateText2; //additional info
    switch (state) {
    case State::init:
        // other controls not yet valid
        infoLabel->SetLabel("Initializing");
        infoLabel2->SetLabel("");
        infoLabel->GetParent()->Layout();
        return;
        break;
    case State::uMachineRunning:
        // hide all CPU state panels, data update meaningless
        infoLabel->SetLabel("uMachine FULL SPEED");
        infoLabel2->SetLabel("No MPC/signal update");
        app->mainFrame->microStepButton->Disable();
        app->mainFrame->microRunUntilButton->Disable();
        app->mainFrame->microRunUntilButton->SetLabel("Start Auto Stepping");
        tracePanel->Disable();
        memoryPanel->Disable();
        unibusSignalsPanel->Disable();
        internalStatePanel->Disable();
        break;
    case State::uMachineManualStepping:
        infoLabel->SetLabel("uMachine MAN CLOCK");
        infoLabel2->SetLabel("Single step via button");
        app->mainFrame->microStepButton->Enable();
        app->mainFrame->microRunUntilButton->Enable();
        app->mainFrame->microRunUntilButton->SetLabel("Start Auto Stepping");
        tracePanel->Enable();
        memoryPanel->Enable();
        unibusSignalsPanel->Enable();
        internalStatePanel->Enable();
        break;
    case State::uMachineAutoStepping:
        infoLabel->SetLabel("uMachine AUTO CLOCK");
        infoLabel2->SetLabel("Steps until condition");
        app->mainFrame->microStepButton->Disable();
        // when auto stepping, the "Auto Step" button is used to stop
        app->mainFrame->microRunUntilButton->SetLabel("Stop Auto Stepping");
        app->mainFrame->microRunUntilButton->Enable();
        tracePanel->Enable();
        memoryPanel->Enable();
        unibusSignalsPanel->Enable();
        internalStatePanel->Enable();
        break;
    }
    wxGetApp().mainFrame->GetSizer()->Layout();
    //infoLabel->GetParent()->Layout() ;
            //tracePanel->GetParent()->Layout() ;
//			memoryPanel->GetParent()->Layout() ;
//	unibusSignalsPanel->GetParent()->Layout() ;
}


// all Pdp11 models must init their GUI
void Pdp11Adapter::onInit() {
    updateGui(State::init); // first state change

    timerUnprocessedMs = 99999; // force immediate call to onTimer()
    // generate messages to init gui, until first status updates comes in
    auto unibusSignals = ResponseUnibusSignals();
    unibusSignals.process(); // calls virtual onRcvMessageFromPdp11() and updates GUI

    receivedUnibusCycleAfterUstep = false; // wait for UNIBUS capture after ustep

    // show empty memory content
    memoryimage.init();
    memoryGridController.rebuild();
    ioPageGridController.rebuild();

}

// display version of conncted PDP11
void Pdp11Adapter::onResponseVersion(ResponseVersion* responseVersion) {
    //wxString title = wxString::Format("uTracer11 - %s, connected to %s", pdp11Adapter->getTypeLabel(), messageInterface->name);
    wxString title = wxString::Format("uTracer11 - %s, connected to \"%s\" via %s", getTypeLabel(),
        responseVersion->version,
        wxGetApp().messageInterface->name);
    wxGetApp().mainFrame->SetLabel(title);
}



// enable panel with "internal state",
// arragne controls for "variable = value" display
// make panel visible
// stateVars[].object is the text control displaying the value
// !! Expected to be called only ONCE in lifetime !!
void Pdp11Adapter::evalInternalStateDefinition(ResponseStateDef* responseStateDef) {
    //    wxGetApp().pdp11Adapter->evalInternalStateDefinition(this);
    cpuStateVars = responseStateDef->stateVars; // save new variable list
    if (cpuStateVars.size() == 0)
        return; // empty answer?

    // preallocated hierachry like Pdp1134CpuKY11LBStatusPanelFB
    // internalStatePanel
    //   -> statixBoxSizer      (title and frame)
    //      -> panel
    //          -> internalStateFlexGridSizerFB (2 columns), growing vertically
    // for each variable:
    // add a wxStaticText with var name
    // add a wxStaticText for variable value ... because of 2 columns var an value appear in one line
    // the
    auto parentPanel = internalStatePanel->internalStateFlexGridSizerPanelFB;
    auto parentSizer = internalStatePanel->internalStateFlexGridSizerFB;
    for (auto it = cpuStateVars.begin(); it != cpuStateVars.end(); it++) {
        // do not save the allocated wxStaticText for the variable names.
        wxStaticText* tmpVarNameStaticText = new wxStaticText(parentPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0);
        wxString varLabel = wxString::Format("%s:", it->name);
        tmpVarNameStaticText->SetLabel(varLabel);
        tmpVarNameStaticText->Wrap(-1);
        parentSizer->Add(tmpVarNameStaticText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);

        // register do not save the allocated wxStaticText for the variable val with the stateDefintion
        wxStaticText* tmpVarValStaticText = new wxStaticText(parentPanel, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, 0);
        tmpVarValStaticText->Wrap(-1);
        // tmpVarValStaticText->SetFont(wxFont(14, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier New")));
        parentSizer->Add(tmpVarValStaticText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 5);
        it->object = tmpVarValStaticText;
    }
    // make the panel large enough to hold all added elements
    // if it has less space, scrollbars appear
    parentPanel->SetMinSize(parentSizer->CalcMin());

    // make visible
    internalStatePanel->Show(true);
    wxGetApp().mainFrame->pdp11StatusSizerFB->Layout();
}


// display list of values
void Pdp11Adapter::evalInternalStateValues(ResponseStateVals* responseStateVals) {
    // value list must fit previously received variable list
    assert(cpuStateVars.size() == responseStateVals->stateVars.size());
    for (unsigned i = 0; i < cpuStateVars.size(); i++) {
        auto stateVar = &cpuStateVars[i];
        auto value = responseStateVals->stateVars[i].value;
        stateVar->value = value;
        // make display dependend on # of bits
        int digitCount = (stateVar->bitCount + 2) / 3; // num of octal digits
        wxString valText = wxString::Format("%0*o", digitCount, value);
        wxStaticText* tmpVarValStaticText = static_cast<wxStaticText*>(stateVar->object);
        tmpVarValStaticText->SetLabel(valText);
    }
    // display
}


/*
Power ON
        -time->
ACLO  1  0  0   ...
DCLO  1  1  0   ...

CPU starts at DCLO 1->0, ACLO must be 0 then

Power OFF
ACLO  0  1 (2000 cpu cycles) 0   ...
DCLO  0  0                   0   ...

*/

void Pdp11Adapter::powerUp() {
    auto msg1 = new RequestUnibusSignalWrite("ACLO", 0);
    wxGetApp().messageInterface->xmtRequest(msg1); // send+delete
    auto msg2 = new RequestUnibusSignalWrite("DCLO", 0);
    wxGetApp().messageInterface->xmtRequest(msg2); // send+delete

    uStepStart();
}

void Pdp11Adapter::powerDown() { // actions same for all pdp11s
    auto msg1 = new RequestUnibusSignalWrite("ACLO", 1);
    wxGetApp().messageInterface->xmtRequest(msg1); // send+delete
    // UNIBUS specs 2-3,s delay -> approx 1000 CPU instructions delay here.
    // impossible do achive  as CPU is under manual ustep constrol
    auto msg2 = new RequestUnibusSignalWrite("DCLO", 1);
    wxGetApp().messageInterface->xmtRequest(msg2); // send+delete
}


// operation on change of manclkenable, same for all PDP11's
void Pdp11Adapter::setManClkEnable(bool _manClkEnable) {
    manClkEnable = _manClkEnable;
    if (_manClkEnable) {
        doLogEvent("Manual Micro Clock enabled"); // same for all pdp11's
        updateGui(State::uMachineManualStepping);
    }
    else {
        doLogEvent("Manual Micro Clock disabled");
        updateGui(State::uMachineRunning);
    }
    // inhibit cycle disassembly whenf micro engine is free running,
    // or single stepping  did not yet passed a "opcode fetch"
    traceController.syncronizedWithMicroMachine = false;

}


// operation after initation of single ustep, same for all PDP11's
void Pdp11Adapter::uStepStart() {
    receivedUnibusCycleAfterUstep = false;
}

// CPU completed a ustep, feed to sub modules
void Pdp11Adapter::uStepComplete(unsigned mpc) {
    traceController.evalUStep(mpc);
	autoStepController.evalUStep(mpc) ;
}


// this a blocking loop, which keeps the Gui alive 
// take care of reentrancy for message events!
void Pdp11Adapter::uStepAutoUntilStop(uint32_t stopUpc, int stopUnibusCycle, uint32_t stopUnibusAddress, int stopRepeatCount) {
    UNREFERENCED_PARAMETER(stopUpc);
    UNREFERENCED_PARAMETER(stopUnibusCycle);
    UNREFERENCED_PARAMETER(stopUnibusAddress);
    UNREFERENCED_PARAMETER(stopRepeatCount);
    //	wxLogFatalError("Abstract Pdp11Adapter::uStepAutoUntilStop() called");
    abortAutoStepping = false;

    // stopUnibusAddress is opcode fetch address
	autoStepController.init(this, stopUpc, stopUnibusAddress) ;
	// todo: repeat count?

    updateGui(State::uMachineAutoStepping);

    // check for "ABORT" button press
    while (!abortAutoStepping && !autoStepController.hasStopped()) {
        //wxMilliSleep(100);  // Simulate some work
        wxYield(); // process pending GUI messages
        // or wxApp::ProcessPendingEvents() ?
        // also calls onTimer()

		autoStepController.loop() ; // 
    }
    updateGui(State::uMachineManualStepping);

	// refresh displays which may have been disabled in autostepMode)
	doEvalMpc(microProgramCounter) ;
	doEvalUnibusCycle(lastUnibusCycle);

}


// called when a new Mpc is received from pdp11
// MPC changed => new value, => event + display update
void Pdp11Adapter::doEvalMpc(uint16_t newMpc) {
        microProgramCounter = newMpc ;
        doLogEvent("mpc = %0.3o", microProgramCounter);
        // repaint document pages only on change
        paintDocumentAnnotations();
        uStepComplete(microProgramCounter);
}		


// Fill in fields on the Unibus Signal form, which is identical for all PDP-11 models.
// msg deleted by framework
void Pdp11Adapter::doEvalUnibusSignals(ResponseUnibusSignals* unibusSignals)
{
    auto panel = unibusSignalsPanel;
    auto s = wxString::Format("%06o", unibusSignals->signals.addr);
    panel->unibusSignalAddrText->SetLabel(s);
    s = wxString::Format("%06o", unibusSignals->signals.data);
    panel->unibusSignalDataText->SetLabel(s);
    // c1c0 to "DATI", .. "DATO" strings
    s = ResponseUnibusCycle::cycleText[unibusSignals->signals.c1c0 & 3];
    panel->unibusSignalControlText->SetLabel(s);
    s = wxString::Format("%d,%d", unibusSignals->signals.msyn, unibusSignals->signals.ssyn);
    panel->unibusSignalMsynSsynText->SetLabel(s);
    s = wxString::Format("%d,%d", unibusSignals->signals.pbpa & 1, (unibusSignals->signals.pbpa >> 1) & 1); //PA,PB
    panel->unibusSignalPaPbText->SetLabel(s);
    s = wxString::Format("%d", unibusSignals->signals.intr);
    panel->unibusSignalIntrText->SetLabel(s);
    s = wxString::Format("%d,%d,%d,%d", unibusSignals->signals.br74 & 1,
        (unibusSignals->signals.br74 >> 1) & 1,
        (unibusSignals->signals.br74 >> 2) & 1,
        (unibusSignals->signals.br74 >> 3) & 1);  //4,5,6,7
    panel->unibusSignalBr4567Text->SetLabel(s);
    s = wxString::Format("%d,%d,%d,%d", unibusSignals->signals.bg74 & 1,
        (unibusSignals->signals.bg74 >> 1) & 1,
        (unibusSignals->signals.bg74 >> 2) & 1,
        (unibusSignals->signals.bg74 >> 3) & 1);  //4,5,6,7
    panel->unibusSignalBg4567Text->SetLabel(s);
    s = wxString::Format("%d", unibusSignals->signals.npr);
    panel->unibusSignalNprText->SetLabel(s);
    s = wxString::Format("%d", unibusSignals->signals.npg);
    panel->unibusSignalNpgText->SetLabel(s);
    s = wxString::Format("%d", unibusSignals->signals.sack);
    panel->unibusSignalSackText->SetLabel(s);
    s = wxString::Format("%d", unibusSignals->signals.bbsy);
    panel->unibusSignalBbsyText->SetLabel(s);
    s = wxString::Format("%d", unibusSignals->signals.init);
    panel->unibusSignalInitText->SetLabel(s);
    s = wxString::Format("%d", unibusSignals->signals.aclo);
    panel->unibusSignalAcloText->SetLabel(s);
    s = wxString::Format("%d", unibusSignals->signals.dclo);
    panel->unibusSignalDcloText->SetLabel(s);
}

// log text to global event panel
void Pdp11Adapter::doLogEvent(const char* format, ...) {
    const int bufferLen = 80;
    char buffer[bufferLen + 1];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buffer, bufferLen, format, argptr);
    buffer[bufferLen - 1] = 0; // terminate on trunc

    auto app = &wxGetApp();
    app->mainFrame->eventsTextCtrl->AppendText(buffer);
    app->mainFrame->eventsTextCtrl->AppendText("\n");
}

// log captured DATI/DATO fill to the several form, which are identically for all PDP-11 models.
// - event log
// -  memory image and memory display grids
// - ustep activity trace
// msg deleted by framework
void Pdp11Adapter::doEvalUnibusCycle(ResponseUnibusCycle* unibusCycle)
{
	// save for disaply regresh
	lastUnibusCycle = unibusCycle ;

    // discard highspeed stream of captured bus cycles to prevent overflow of grids
    if (state == State::init || state == State::uMachineRunning)
        return;

    // log the cycle
    wxString s;
    if (unibusCycle->nxm)
        s = wxString::Format("%s addr=%0.6o NXM", ResponseUnibusCycle::cycleText[unibusCycle->c1c0], unibusCycle->addr);
    else
        s = wxString::Format("%s addr=%0.6o data=%0.6o", ResponseUnibusCycle::cycleText[unibusCycle->c1c0], unibusCycle->addr, unibusCycle->data);
    doLogEvent(s.ToStdString().c_str());


    // to memory windows
    uint32_t wordAddr = unibusCycle->addr & ~1; // dati of odd address is also word acccess
    // update memory state
    if (unibusCycle->nxm)
        memoryimage.put_nxm(wordAddr);
    else if (unibusCycle->c1c0 == 3) // DATOB byte addr
        memoryimage.put_byte(unibusCycle->addr, unibusCycle->data);
    else
        memoryimage.put_word(wordAddr, unibusCycle->data);
    // feed to correct data grid
    MemoryGridController* mgc;
    if (wordAddr < MEMORY_IOPAGE_ADDRESS)
        mgc = &memoryGridController;
    else
        mgc = &ioPageGridController;
    // now memory image updated, show in grid
    // make address even, grid manages word addresses
    mgc->updateSingleAddress(wordAddr);
    mgc->setMarkedAddress(wordAddr);
    // refresh?

    // to tracer
    // Show only UNIBUS cycles which are generated by micro step of CPU under test,
    // these are "asynchronical".
    // Exclude DATI/DATO generated by user memory EXAM/DEPOSIT ("requested")
    // Only if manual clock enabled, only, if first response after last micro step
    // 11/34: physical KY11LB polls 777707 ... ignore this too?
    if (manClkEnable && !receivedUnibusCycleAfterUstep) {
        if (unibusCycle->requested) {
            wxLogError("First UNIBUS cycle in ustep marked 'requested' by M93X2 probe, should be asynchronical");
        }
        unibusCycle->requested = true; // anyhow
        traceController.evalUnibusCycle(unibusCycle);
		autoStepController.evalUnibusCycle(unibusCycle);
        receivedUnibusCycleAfterUstep = true;  // ignore following user EXAM/DEPOSITs
    }
}

// click into a memory grid:
// copy addr & value from grid cell to "manual ExamDeposit controls"
void Pdp11Adapter::onMemoryGridClick(MemoryGridController* gridController, unsigned rowIdx, unsigned colIdx)
{
    uint32_t addr;
    if (gridController->isEmpty())
        addr = 0;
    else addr = gridController->getAddrFromCellCoord(rowIdx, colIdx);
    auto dataText = gridController->grid->GetCellValue(rowIdx, colIdx);
    memoryPanel->manualExamDepositAddrTextCtrl->SetValue(wxString::Format("%06o", addr));
    memoryPanel->manualDepositDataTextCtrl->SetValue(dataText);
}

// load a MACRO11 .mac or absolute papertape .ptap file into
// memory buffer, then deposit to PDP11 via messages
void Pdp11Adapter::loadMemoryFile(wxFileName path, memory_fileformat_t fileFormat) {
    const char* entry_label = "start";
    std::string info = "";
    uint32_t entry_address = MEMORY_ADDRESS_INVALID;
    bool load_ok = false;
    memoryimage.init();
    switch (fileFormat) {
    case fileformat_none:
        wxLogInfo("Loaded no memory image");
        return;
    case fileformat_addr_value_text:
        load_ok = memoryimage.load_addr_value_text(path.GetFullPath().c_str());
        info = "addr-value-pair memory image";
        break;
    case fileformat_macro11_listing:
        load_ok = memoryimage.load_macro11_listing(path.GetFullPath().c_str(), &codelabels);
        if (codelabels.is_defined(entry_label))
            entry_address = codelabels.get_address(entry_label);
        info = "MACRO11 listing memory image";
        break;
    case fileformat_papertape:
        load_ok = memoryimage.load_papertape(path.GetFullPath().c_str(), &codelabels);
        info = "Absolute Papertape memory image";
        if (codelabels.size() > 0) // only one possible label: "entry"
            entry_address = codelabels.begin()->second;
        break;
    case fileformat_binary:
        load_ok = memoryimage.load_binary(path.GetFullPath().c_str());
        info = "Binary memory image";
        break;
    }
    if (!load_ok) {
        wxLogError("Failed loading %s from %s", info, path.GetAbsolutePath());
        return;
    }
    wxLogInfo("Loaded %s from %s", info, path.GetAbsolutePath());

    uint32_t firstaddr, lastaddr;
    memoryimage.get_addr_range(&firstaddr, &lastaddr);
    wxLogInfo("%d words from %0.6o to %0.6o", memoryimage.get_word_count(), firstaddr, lastaddr);

    if (entry_label == nullptr)
        wxLogInfo("No entry address label.\n");
    else if (entry_address != MEMORY_ADDRESS_INVALID)
        wxLogInfo("Entry address at \"%s\" label is %06o.\n", entry_label, entry_address);
    else
        wxLogInfo("No entry address at \"%s\" label.\n", entry_label);

    // show
    memoryGridController.rebuild();
}


// invalidate the memory image range defined by the controller
void Pdp11Adapter::clearMemoryImage(MemoryGridController* gridController)
{
    for (auto addr = gridController->addressFrom; addr < gridController->addressTo; addr += 2)
        gridController->memoryImage->cells[addr / 2].valid = false;
    gridController->rebuild();
}


// Deposit all "valid" memory words into physical memory
// set SACK while active
// output stream of Deposits.
// responses are the detected unibus cycle ResponseUnibusCycle()
// issue serialPortRcvPoll() to process the responses,
//  else serial buffer overflow
void Pdp11Adapter::depositMemoryImage() {
    auto mi = wxGetApp().messageInterface;

    mi->receiveAndProcessResponses(); // process pending

    // activate SACK. remember: serialPortXmtMessage() deletes its argument
    mi->xmtRequest(new RequestUnibusSignalWrite("SACK", 1));

    for (unsigned i = 0; i < MEMORY_WORD_COUNT; i++) {
        uint32_t addr = 2 * i;
        auto cell = memoryimage.get_cell(addr);
        if (cell->valid) {
            // put nxm?
            auto msg = new RequestUnibusDeposit(addr, cell->data);
            mi->xmtRequest(msg);
            wxMilliSleep(1); // do not overload M93X2 probe
            mi->receiveAndProcessResponses(); // process pending
        }
    }
    // deactivate SACK
    mi->xmtRequest(new RequestUnibusSignalWrite("SACK", 0));
}


void Pdp11Adapter::singleExam(uint32_t addr) {
    auto msg = new RequestUnibusExam(addr);
    wxGetApp().messageInterface->xmtRequest(msg);
}

void Pdp11Adapter::singleDeposit(uint32_t addr, uint16_t data) {
    auto msg = new RequestUnibusDeposit(addr, data);
    wxGetApp().messageInterface->xmtRequest(msg);
}

