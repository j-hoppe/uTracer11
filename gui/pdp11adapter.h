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
#include "utils.h" // linux/vs macros
#include "messages.h" // from M93X2 probe project
#include "memoryimage.h" // UNIBUS memory, clone from QUniBone
#include "memorygridcontroller.h"
#include "tracecontroller.h"
#include "autostepcontroller.h"
#include "mainframe.h"

class Pdp11Adapter {
public:
		// State controls function of GUI elements
	enum class State {
		init, // while starting
		uMachineRunning, // PDP11 executing under own control
		uMachineManualStepping, // micro steps by manual button press
		uMachineAutoStepping // multiple steps until breakpoint
	} ;
	State state ;

	static const uint16_t InvalidMpc = 0xffff ;
	static const uint32_t InvalidUnibusAddress = 0x7fffffff ;
		

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

	// mirrored state of PDP-11 under test
    wxFileName resourceDir; // absolute root path for images and OCRed data

	std::string version ;

    memoryimage_c   memoryimage; // buffer for 256KB
    codelabel_map_c codelabels;

    std::vector<MessagesStateVar> cpuStateVars; // internal simualtor vars, if any


    Pdp11Adapter(); // directory parsed by application
    virtual ~Pdp11Adapter() {}

    // GUI panels, genric for all UNIBUS CPUs
    MemoryPanel* memoryPanel = nullptr;
    TracePanel* tracePanel = nullptr;
    UnibusSignalsPanelFB* unibusSignalsPanel = nullptr;
    InternalStatePanelFB* internalStatePanel = nullptr;

    virtual void setupGui(wxFileName _resourceDir);
	// Set State of control, visibility and functions
	virtual void updateGui(enum State newState) ;

    virtual void onInit();

    void onResponseVersion(ResponseVersion* responseVersion);

	void evalInternalStateDefinition(ResponseStateDef *responseStateDef) ;
	void evalInternalStateValues(ResponseStateVals *responseStateVals) ;

    virtual void onTimer(unsigned periodMs) {
        UNREFERENCED_PARAMETER(periodMs) ;
        wxLogFatalError("Abstract Pdp11Adapter::onTimer() called");
    }

    // update all annoted document page images, depending on GUI state
    virtual void paintDocumentAnnotations() {
        wxLogFatalError("Abstract Pdp11Adapter::paintDocumentAnnotations() called");
    }


    // message from PDP11 hardware or simulation received, updates state for GUI
    virtual void onRcvMessageFromPdp11(Message* msg) {
        UNREFERENCED_PARAMETER(msg) ;
        wxLogFatalError("Abstract Pdp11Adapter::onRcvMessageFromPdp11() called");
    }

    // message to PDP11 hardware, serial or direct interface to simulation
    virtual void sentMessageToPdp11(Message* msg) {
        UNREFERENCED_PARAMETER(msg) ;
        wxLogFatalError("Abstract Pdp11Adapter::sentMessageToPdp11() called");
    }

    // reset with ACLO,DCLO via UNIBUS, same for all PDP11s
    virtual void powerDown();
    virtual void powerUp();

    // false = machine running, true = under step control
    virtual void setManClkEnable(bool _manClkEnable);

    // execute a single µstep
    virtual void uStep() {
        wxLogFatalError("Abstract Pdp11Adapter::uStep() called");
    }

    void uStepStart();

    void uStepComplete(unsigned mpc);

	volatile bool stopAutoStepping;
    void doAutoStepping(uint32_t stopUpc, int stopUnibusCycle, uint32_t stopUnibusAddress, int stopRepeatCount) ;

    virtual void onResponseKM11Signals(ResponseKM11Signals* km11Signals) {
        UNREFERENCED_PARAMETER(km11Signals) ;
        wxLogFatalError("Abstract Pdp11::onResponseKM11Signals() called");
    }
    virtual void onResponseKY11LBSignals(ResponseKY11LBSignals* ky11Signals) {
        UNREFERENCED_PARAMETER(ky11Signals) ;
        wxLogFatalError("Abstract Pdp11::onResponseKY11LBSignals() called");
    }

	void doEvalMpc(uint16_t newMpc) ;

    // all UNIBUS update send to same base Form
    void doEvalUnibusSignals(ResponseUnibusSignals* unibusSignals);

    virtual void onResponseUnibusSignals(ResponseUnibusSignals* unibusSignals) {
        UNREFERENCED_PARAMETER(unibusSignals) ;
        wxLogFatalError("Abstract Pdp11::evalUnibusSignals() called");
    }

    void doLogEvent(const char* format, ...);
	ResponseUnibusCycle lastUnibusCycle;
    void doEvalUnibusCycle(ResponseUnibusCycle* unibusCycle);

    virtual void evalUnibusCycle(ResponseUnibusCycle* unibusCycle) {
        UNREFERENCED_PARAMETER(unibusCycle) ;
        wxLogFatalError("Abstract Pdp11::evalUnibusCycle() called");
    }

    // 2 grid for memory and iopage
    MemoryGridController memoryGridController;
    MemoryGridController ioPageGridController;


    TraceController traceController;

	AutoStepController autoStepController ;

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
