/* pdp11Adapter - logic between a target PDP11 and wxWidgets GUI
 In the model-view-controller pattern:
 model = target PDP11 physical (via M93X2probe) or simulator
 view = wxWidget GUI
 controller - pdp11adapter

Abstract base class
 Sub classes needed for
 11/40 with KM11,
 11/40 simulation (Angelo)
 11/34 with KY11
 and others

*/
#ifndef __PDP11ADAPTER_H__
#define __PDP11ADAPTER_H__

#include "wx/wx.h"
#include <vector> // std headers after wx.h, else tons of C4996 strcpy()
#include <map>
#include <stdint.h>
#include "utils.h" // linux/vs macros
#include "binary.h"
#include "variables.h"
#include "messages.h" // from M93X2 probe project
#include "memoryimage.h" // UNIBUS memory, clone from QUniBone
#include "memorygridcontroller.h"
#include "tracecontroller.h"
#include "script.h"
#include "mainframe.h"


// describes one bit sub field in the micro code control word bit array
// example: pdp11/34 has a 48 bit width field
class ControlWordField {
public:
    unsigned bitFrom; //
    unsigned bitTo;
    unsigned normalValue; // from DEC doc for "normal" (= inactive?) values
    wxString fieldLabel; // like in XMLs, used for optical annotation

    ControlWordField(unsigned _bitFrom, unsigned _bitTo, unsigned _normalValue, wxString _fieldLabel) :
        bitFrom(_bitFrom), bitTo(_bitTo), normalValue(_normalValue), fieldLabel(_fieldLabel) {}

    unsigned bitCount() {
        return bitTo - bitFrom + 1;
    }

    // return
    int bitIdx(unsigned controlwordBitIdx) {
        return controlwordBitIdx - bitFrom ;
    }

    unsigned extract(uint64_t controlword) {
        unsigned result = 0;
        unsigned iDst, iSrc;
        for (iDst = 0, iSrc = bitFrom; iSrc <= bitTo; iDst++, iSrc++)
            if (getBit(controlword, iSrc))
                result |= 1 << iDst;
        return result;
    }
};


class Application ;

class Pdp11Adapter {
public:
  Application *app =nullptr; // give pdp11adapter access to wxWidgets Elements

    // State controls function of GUI elements
    enum class State {
        init, // while starting
        uMachineRunning, // PDP11 executing under own control
        uMachineManualStepping, // micro steps by manual button press
        scriptRunning // multiple steps until breakpoint
    };
    State state;

    static const uint16_t InvalidMpc = 0xffff;
    static const uint32_t InvalidUnibusAddress = 0x7fffffff;


protected:
    unsigned timerUnprocessedMs; // helper to reduce onTimer() frequency
    bool manClkEnable;
    uint32_t microProgramCounter = 0;

    bool receivedUnibusCycleAfterUstep = false; // wait for UNIBUS capture after ustep

public:

    enum class Type {
        none, pdp1134phys, pdp1134sim, pdp1140phys, pdp1140sim
    };



    static enum Type parseTypeName(std::string typeName);
    static std::vector<std::string> getTypeNames();

    virtual Type getType() {
        wxLogFatalError("Abstract Pdp11Adapter::getType() called");
        return Type::none;
    }

    // short name, long label
    virtual std::string getTypeLabel() {
        wxLogFatalError("Abstract Pdp11Adapter::getTypeLabel() called");
        return "Pdp11Adapter";
    }

    // microstep address which fetches opcode, needed to sync disassembly
    virtual unsigned getMpcFetch() {
        wxLogFatalError("Abstract Pdp11Adapter::getMpcFetch() called");
        return 0;
    }

    std::string version;

    wxFileName resourceDir; // absolute root path for images and OCRed data

    // bit segments in the control word
    std::vector<ControlWordField> controlWordFields;

    memoryimage_c   memoryimage; // buffer for 256KB RAM
    codelabel_map_c codelabels; // for disas

    // state variables: Definitions constructed from 3 sources
    //  - basic (built in)
    // - from M93X2 probe behaviour (static in Pdp11adapterxxx.onInit()), (physical, perhaps simulators)
    // - ResponseRegisterDef (simulators only)
    // Values:
    // - from M93x2 probe signals (onResponseXXXXSignals)
    // - ResponseRegVals (simulators only)
    VariableMap	stateVars ;
    std::map <unsigned,unsigned>	stateVarIndexOfRegister ; // REGDEF/REGVAL position in stateVars
    //Defunsigned stateVarsFirstRegisterIndex; // for evalResponseRegisterValues


    Pdp11Adapter(); // directory parsed by application
    virtual ~Pdp11Adapter() {}

    // GUI panels, generic for all UNIBUS CPUs
    MemoryPanel* memoryPanel = nullptr;
    TracePanel* tracePanel = nullptr;
    UnibusSignalsPanelFB* unibusSignalsPanel = nullptr;
    InternalStatePanelFB* internalStatePanel = nullptr;
    Pdp11uFlowPanel* uFlowPanel = nullptr;

    // all have a micro code control store. long controlword upto 64 bits index by mpc.
    std::map<unsigned, uint64_t>	controlStore;

    virtual void setupGui(wxFileName _resourceDir);
    // Set State of control, visibility and functions
    virtual void updateGui(enum State newState);

    virtual void onInit();

    void onResponseVersion(ResponseVersion* responseVersion);

    void evalResponseRegisterDefinition(ResponseRegDef* responseRegDef);
    void evalResponseRegisterValues(ResponseRegVals* responseRegVals);

    virtual void onTimer(unsigned periodMs) {
        UNREFERENCED_PARAMETER(periodMs);
        wxLogFatalError("Abstract Pdp11Adapter::onTimer() called");
    }

    // update all annoted document page images, depending on GUI state
    virtual void paintDocumentAnnotations() {
        wxLogFatalError("Abstract Pdp11Adapter::paintDocumentAnnotations() called");
    }


    // message from PDP11 hardware or simulation received, updates state for GUI
    virtual void onRcvMessageFromPdp11(Message* msg) {
        UNREFERENCED_PARAMETER(msg);
        wxLogFatalError("Abstract Pdp11Adapter::onRcvMessageFromPdp11() called");
    }

    // message to PDP11 hardware, serial or direct interface to simulation
    virtual void sentMessageToPdp11(Message* msg) {
        UNREFERENCED_PARAMETER(msg);
        wxLogFatalError("Abstract Pdp11Adapter::sentMessageToPdp11() called");
    }

    // reset with ACLO,DCLO via UNIBUS, same for all PDP11s
    virtual void powerDown();
    virtual void powerUp();

    // false = machine running, true = under step control
    virtual void setManClkEnable(bool _manClkEnable);

    // execute a single µstep
    virtual void requestUStep() {
        wxLogFatalError("Abstract Pdp11Adapter::requestUStep() called");
    }

    void uStepStart();

    void uStepComplete(unsigned mpc);

	Script script ;
	wxFileName scriptFilePath ;
	void scriptStart() ;
	void scriptAbort() ;
	void onScriptComplete(Script::RunState completeReason) ;
	

    virtual void onResponseKM11Signals(ResponseKM11Signals* km11Signals) {
        UNREFERENCED_PARAMETER(km11Signals);
        wxLogFatalError("Abstract Pdp11::onResponseKM11Signals() called");
    }
    virtual void onResponseKY11LBSignals(ResponseKY11LBSignals* ky11Signals) {
        UNREFERENCED_PARAMETER(ky11Signals);
        wxLogFatalError("Abstract Pdp11::onResponseKY11LBSignals() called");
    }

    void onResponseMpc(uint16_t newMpc);

    // all UNIBUS update send to same base Form
    virtual void onResponseUnibusSignals(ResponseUnibusSignals* unibusSignals);

    void logEvent(const char* format, ...);
    ResponseUnibusCycle lastUnibusCycle = ResponseUnibusCycle(NOTAG);
    virtual void onResponseUnibusCycle(ResponseUnibusCycle* unibusCycle);

    void displayStateVarsDefinition() ;
    void displayStateVarsValues() ;

    // 2 grid for memory and iopage
    MemoryGridController memoryGridController;
    MemoryGridController ioPageGridController;
    bool updateManualMemoryExamData;// show next UNIBUS data as "EXAM" button response


    TraceController traceController;

    void loadControlStore(wxFileName resourcePath, std::string subDir, std::string xmlFileName);

    void loadMemoryFile(wxFileName path, memory_fileformat_t fileFormat);
    void depositMemoryImage();
    void singleExam(uint32_t addr);
    void singleDeposit(uint32_t addr, uint16_t);

    void onMemoryGridClick(MemoryGridController* gridController, unsigned rowIdx, unsigned colIdx);
    void clearMemoryImage(MemoryGridController* gridController);
};

/*
// methods
    // memory
    size memory
    single step until break
    breakpitns on µpcm unibus dati/dato
        */

#endif // __PDP11ADAPTER_H__
