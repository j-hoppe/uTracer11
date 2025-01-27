/* pdp11adapter40 - base interface to physical and simulated PDP-11/40
*/
#ifndef __PDP11ADAPTER40_H__
#define __PDP11ADAPTER40_H__

#include "wx/wx.h"
#include "pdp11adapter.h"
#include "documentpageannotations.h"
#include "keywords.h"
#include "pdp1140km11state.h"

class Pdp11Adapter40 : public Pdp11Adapter {

public:
    virtual std::string getTypeLabel() override {
        return "PDP11/40 with M93X2probe and KY11";
    }
    Pdp11Adapter40() : Pdp11Adapter() {}
	virtual ~Pdp11Adapter40() override {}

    // attach/arrange panels, set directories
    Pdp1140CpuKM11StatusPanelFB* km11StatusPanel = nullptr;
    virtual void setupGui(wxFileName _resourceDir) override;
	virtual void updateGui(State state) override;

    Pdp1140KM11AState km11AState; // CPU KD11-A internal signals. also used in simulaotr
    Pdp1140KM11BState km11BState; // MMU/FPU internal signals

    virtual unsigned getMpcFetch() override { return 016; }

    virtual void onInit() override;
    virtual void onTimer(unsigned periodMs) override;

    virtual void setManClkEnable(bool manClkEnable) override;
    virtual void requestUStep() override;
    //virtual void doAutoStepping(uint32_t stopUpc, int stopUnibusCycle, uint32_t stopUnibusAddress, int stopRepeatCount) override;

    virtual void paintDocumentAnnotations() override;

    bool ignoreKM11 = false; // testmode
    virtual void onResponseKM11Signals(ResponseKM11Signals* km11Signals) override;
    virtual void onResponseKY11LBSignals(ResponseKY11LBSignals* ky11Signals) override;
    virtual void onResponseUnibusSignals(ResponseUnibusSignals* unibusSignals) override;
    virtual void onResponseUnibusCycle(ResponseUnibusCycle* unibusCycle) override;

    DocumentPageAnnotations uflowPageAnnotations;

    DocumentPageAnnotations datapathPageAnnotations;

    // search in current uflow MPC data for keys of datapth objects
    KeyPatternList uflowTodataPathKeyPatterns;

    // document panels
    //pdp1140uFlowPanel = new Pdp1140uFlowPanel(mainFrame->documentsNotebookFB);



};
#endif // __PDP11ADAPTER40_H__


