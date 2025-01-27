/*  pdp11adapter34 - base interface to physical and simulated PDP-11/34
*/
#include <string>
#include <stdexcept>

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
    case State::scriptRunning:
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

	// add KY11LB signals to stateVars
	// mpc already registered, but tune length
		stateVars.get("MPC")->bitCount = 9 ; // 0..511
		stateVars.add(Variable("FP11", "FP11 present?", 1, VariableType::signal)) ; // static, do not trace
		stateVars.add(Variable("SBF", "SERVICE BR FAIL", 1, VariableType::signal | VariableType::trace)) ;
		stateVars.add(Variable("LIR", "LOAD IR", 1, VariableType::signal | VariableType::trace)) ;
		stateVars.add(Variable("PBP", "PFAIL BR PEND", 1, VariableType::signal | VariableType::trace)) ;
	displayStateVarsDefinition() ;

    // generate messages to init gui, until first status updates comes in
    auto cpuSignals = ResponseKY11LBSignals(NOTAG, 0, 0, 0, 0, 0);
    cpuSignals.process(); // calls virtual onRcvMessageFromPdp11() and updates GUI

    // convert "LSB-first" binary string to value
#define LSB1ST(s) (BinaryString((s), /*msbfirst*/false).value)

    // setup bit fields. "normal" bit string like in DEC doc, LSB 1st, fieldnames like markers in XML! 
    controlWordFields = {
        // impossible noram value => shown always as "active"
        ControlWordField(0, 8, 99999, "NEXT_MPC_ADDRESS"),
        ControlWordField(9, 11, LSB1ST("000"), "MISCELLANEOUS_CONTROL"),
        ControlWordField(12 ,12, LSB1ST("0"), "DATA_TRAN"),
        ControlWordField(13, 14, LSB1ST("00"), "BUS_CONTROL"),
        ControlWordField(15, 15, LSB1ST("0"), "ENAB_MAINT"),
        ControlWordField(16, 16, LSB1ST("0"), "LOAD_BAR"),
        ControlWordField(17, 17, LSB1ST("1"), "LONG_CYCLE_L"),
        ControlWordField(18, 18, LSB1ST("0"), "AUX_CONTROL"),
        ControlWordField(19, 23, LSB1ST("00101"), "ALU,BLEG_CONTROL"),
        ControlWordField(24, 27, LSB1ST("0000"), "B,BX,OVX,DBE_CONTROL"),
        ControlWordField(28, 29, LSB1ST("00"), "SSMUX_CONTROL"),
        ControlWordField(30, 31, LSB1ST("01"), "AMUX_CONTROL"),
        ControlWordField(32, 35, LSB1ST("0000"), "BUT_BITS"),
        ControlWordField(36, 37, LSB1ST("00"), "SPA_SRC_SEL"),
        ControlWordField(38, 39, LSB1ST("11"), "SPA_DST_SEL"),
        ControlWordField(40, 40, LSB1ST("1"), "FORCE_RS_V_1_L"),
        ControlWordField(41, 41, LSB1ST("1"), "PREVIOUS_MODE_L"),
        ControlWordField(42, 42, LSB1ST("0"), "BUT_SERVICE"),
        ControlWordField(43, 43, LSB1ST("0"), "FORCE_KERNEL"),
        ControlWordField(44, 47, LSB1ST("0000"), "ROM_SPA")
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

    // search in current uflow MPC data for keys of datapath objects
    // like "d:\retrocmp\dec\pdp11\uTracer11\resources\pdp1134\"
    uflowTodataPathKeyPatterns.loadXml(resourceDir, ".", "uflow2datapath.xml");
    auto controlKeyList = datapathPageAnnotations.getKeyList();
    uflowTodataPathKeyPatterns.assertKeysAgainst(controlKeyList, "uflow2datapath.xml must reference mp00082-datapath.xml");

    // verify matching of regexes to datafields
    // list all annotations datafields[2] with
    uflowPageAnnotations.dumpKeywordsForDatafields(uflowTodataPathKeyPatterns);

    // set manclock to initial state of "ManClock" ToggleButton. false  = not pressed
    auto _manClkEnable = app->mainFrame->manClockEnableButton->GetValue();
    setManClkEnable(_manClkEnable); // state change

}

// called periodically
void Pdp11Adapter34::onTimer(unsigned periodMs) {
    timerUnprocessedMs += periodMs;
    bool pollSlow = (timerUnprocessedMs > 500);
    if (pollSlow)
        timerUnprocessedMs -= 500;
pollSlow = false;

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
        auto reqKy11 = new RequestKY11LBSignalsRead(NOTAG);
        app->messageInterface->xmtRequest(reqKy11); // send+delete
        auto reqUnibus = new RequestUnibusSignalsRead(NOTAG);
        app->messageInterface->xmtRequest(reqUnibus); // send+delete

        bool updateStateVars = (state == State::uMachineManualStepping);
        if (updateStateVars && stateVars.size() > 0) {
            // singlestepping: the pdp11 publishes its internal state, update it
            auto reqRegVal = new RequestRegVal(NOTAG);
            app->messageInterface->xmtRequest(reqRegVal); // send+delete
        }
    }
}


// annotate the control word page. 
// key = 3 digit mpc
void Pdp11Adapter34::paintMicroStoreDocumentAnnotations(std::string mpcAsKey) {
    // skip before startup and if not visible
    if (controlwordPanel == nullptr || !controlwordPanel->IsShownOnScreen())
        return;

    try { // every error prints a message and exits this method

        // resulting list of annotations to display:
        // bit numbers, field and table markers
        std::vector<std::string> keyList;

        // get current controlword
        unsigned mpc = stol(mpcAsKey, nullptr, 8); // need numeric value
        if (controlStore.count(mpc) == 0)
            throw std::logic_error("Control word for key \"" + mpcAsKey + "\" not in controlStore");
        uint64_t controlword = controlStore.at(mpc); // current 48 bit word

        // 1. paint "0" or "1" into the controlword onto bit positions
        //   
        // uword: for each mpc a list of 0s and 1s (48 bit field)
        // 2. extract certain bit subfields (eg: 28..31 = SSMUX control)
        // 		this is the key for a geometry (box around SSMUX table entry)
        //controlwordPageAnnotations.paintScaled(key, uwordPanel);

        // for each control word bit field
        for (ControlWordField& cwf : controlWordFields) {
            unsigned fieldValue = cwf.extract(controlword);
            // 1. paint the single bits into the long 48 bit header on MP00082-15.jpg
            for (unsigned fieldBitIdx = 0; fieldBitIdx < cwf.bitCount(); fieldBitIdx++) {
                unsigned controlwordBitIdx = fieldBitIdx + cwf.bitFrom;
                // get "0"/"1" annotations of each bit
                // key for a single bit is like "BIT.47", see XML
                std::string bitKey = wxString::Format("BIT.%0.2d", controlwordBitIdx).ToStdString();
                // find text annotation for each bit
                DocumentPageAnnotation* dpa = controlwordPageAnnotations.find(bitKey);
                if (dpa == nullptr)
                    throw std::logic_error("Annotation " + bitKey + " not found");
                // "bit" annotation must have a single "text" geometry, may throw.
                DocumentPageAnnotationText* textgeom = dynamic_cast<DocumentPageAnnotationText*>(dpa->geometries.at(0));
                if (textgeom == nullptr)
                    throw std::logic_error("Annotation " + bitKey + " is found, but not a text");
                bool controlwordBitValue = getBit(controlword, controlwordBitIdx);
                bool fieldNormalBitValue = getBit(cwf.normalValue, fieldBitIdx);
                // paint bits which differ from their "normal" value in red
                textgeom->text = controlwordBitValue ? "1" : "0";
                bool activeBit = (controlwordBitValue != fieldNormalBitValue);
                if (cwf.fieldLabel.IsSameAs("NEXT_MPC_ADDRESS")) // all bits from MPC field always red
                    activeBit = true;
                if (activeBit)
                    textgeom->fontColor = controlwordPageAnnotations.geometryFontColor;
                else textgeom->fontColor = *wxBLACK;
                keyList.push_back(bitKey); // always paint all bits
            }

            // 2. for every field: mark its label, if bits not "normal" 
            // label key like "FIELDLABEL.AUX_CONTROL"
            if (fieldValue != cwf.normalValue) {
                std::string fieldLabelKey = wxString::Format("FIELDLABEL.%s", cwf.fieldLabel).ToStdString();
                DocumentPageAnnotation* dpa = controlwordPageAnnotations.find(fieldLabelKey);
                if (dpa == nullptr)
                    throw std::logic_error("Annotation " + fieldLabelKey + " not found");
                keyList.push_back(fieldLabelKey);
            }

            // 3. for every field with a value table: 
            // mark the table entry for current field value, if shown on MP00082-15
            // table entry key like "FIELDLABEL.AUX_CONTROL.000"
            {
                BinaryString valueBinaryString(fieldValue, cwf.bitCount(), /*msbfirst*/true);
                std::string fieldValueEntryKey = wxString::Format("FIELDVALUE.%s.%s",
                    cwf.fieldLabel.ToStdString(), valueBinaryString.text).ToStdString();
                DocumentPageAnnotation* dpa = controlwordPageAnnotations.find(fieldValueEntryKey);
                if (dpa != nullptr) // not all fields have tables for theirs values
                    keyList.push_back(fieldValueEntryKey);
            }

            /*
                std::vector<std::string> keyList =
                { "BIT.00", "BIT.24", "BIT.47", "FIELDLABEL.NEXT_MPC_ADDRESS", "FIELDVALUE.ROM_SPA.0000" };
            */
        }

        // Finally: paint all required annotations
        controlwordPageAnnotations.paintScaled(keyList, controlwordPanel);
    }
    catch (const std::exception& e) {
        wxLogError("in paintMicroStoreDocumentAnnotations(): %s", e.what());
    }
}


// paint datapath image. Several objects marked
// which are mentioned in current uflow annotation datafield
// get datafields for current upc
// key = 3 digit mpc
void Pdp11Adapter34::paintDatapathDocumentAnnotations(std::string mpcAsKey) {
    // skip before startup and if not visible
    if (dataPathPanel == nullptr || !dataPathPanel->IsShownOnScreen())
        return;

    auto dpa = uflowPageAnnotations.find(mpcAsKey);
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
    std::string mpcAsKey = wxString::Format("%03o", microProgramCounter).ToStdString();

    // uflow: a single geometry for the mpc
    if (uFlowPanel != nullptr) // startup passed?
        uflowPageAnnotations.paintScaled(mpcAsKey, uFlowPanel);

    paintMicroStoreDocumentAnnotations(mpcAsKey);

    paintDatapathDocumentAnnotations(mpcAsKey);
}

void Pdp11Adapter34::setManClkEnable(bool _manClkEnable)
{
    auto msg = new RequestKY11LBSignalWrite(AUTOTAG, "MCE", _manClkEnable);
    app->messageInterface->xmtRequest(msg); // send+delete
    // answer from M93X2probe is ResponseKY11LBSignals
    Pdp11Adapter::setManClkEnable(_manClkEnable); // actions same for all pdp11s
}

// step and query new state
void Pdp11Adapter34::requestUStep()
{
    auto msg1 = new RequestKY11LBSignalWrite(AUTOTAG, "MC", 2/*Pulse*/);
    app->messageInterface->xmtRequest(msg1); // send+delete
    // answer from M93X2probe is ResponseKY11LBSignals
    Pdp11Adapter::uStepStart(); // actions same for all pdp11s

    auto msg2 = new RequestRegVal(AUTOTAG);
    app->messageInterface->xmtRequest(msg2); // send+delete
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
	
    ky11Signals->mpc &= 0x1ff; // strip off bit 10, to 0..511
	stateVars.get("MPC")->setValue(ky11Signals->mpc);
	stateVars.get("FP11")->setValue(ky11Signals->fp11);
	stateVars.get("SBF")->setValue(ky11Signals->sbf);
	stateVars.get("LIR")->setValue(ky11Signals->lir);
	stateVars.get("PBP")->setValue(ky11Signals->pbp);
	displayStateVarsValues() ;

    if (ky11StatusPanel) {
        wxString s;
		s = stateVars.get("MPC")->valueText() ;
        ky11StatusPanel->pdp1134Ky11MpcText->SetLabel(s);
        s = stateVars.get("FP11")->valueText();
        ky11StatusPanel->pdp1134Ky11FpText->SetLabel(s);
        s = stateVars.get("SBF")->valueText();
        ky11StatusPanel->pdp1134Ky11SbfText->SetLabel(s);
        s = stateVars.get("LIR")->valueText();
        ky11StatusPanel->pdp1134Ky11LirText->SetLabel(s);
        s = stateVars.get("PBP")->valueText();
        ky11StatusPanel->pdp1134Ky11PbpText->SetLabel(s);
    }
    // process MPC events only for change
    if (microProgramCounter != ky11Signals->mpc) {
        // MPC changed => new value, => event + display update
        onResponseMpc(ky11Signals->mpc);
    }
}

// Unibus-Signal panel same for all Pdp11s
// route message to Pdp11-model specific panel
void Pdp11Adapter34::onResponseUnibusSignals(ResponseUnibusSignals* unibusSignals) {
    Pdp11Adapter::onResponseUnibusSignals(unibusSignals); // same for all pdp11's
}

// Unibus-Signal panel same for all Pdp11s
// route message to Pdp11-model specific panel
void Pdp11Adapter34::onResponseUnibusCycle(ResponseUnibusCycle* unibusCycle)
{
    Pdp11Adapter::onResponseUnibusCycle(unibusCycle); // same for all pdp11's
}

