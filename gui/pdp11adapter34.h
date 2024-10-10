/* pdp11adapter34 - base interface to physical and simulated PDP-11/34
*/
#ifndef __PDP11ADAPTER34_H__
#define __PDP11ADAPTER34_H__

#include "wx/wx.h"
#include "pdp11adapter.h"
#include "documentpageannotations.h"
#include "keywords.h"

class Pdp11Adapter34 : public Pdp11Adapter {
public:
    virtual std::string getTypeLabel() override {
        return "PDP11/34 with M93X2probe and KY11-LB";
    }
    Pdp11Adapter34() : Pdp11Adapter() {}
    virtual ~Pdp11Adapter34() override {}

    // attach/arrange panels generic to all 1134, set directories
    Pdp11uFlowPanel* uFlowPanel = nullptr; // just an empty panel to draw onto
    //Pdp1134uWordPanel* uWordPanel = nullptr;
    Pdp1134DataPathPanel* dataPathPanel = nullptr;
    Pdp1134CpuKY11LBStatusPanelFB* ky11StatusPanel = nullptr; 
    virtual void setupGui(wxFileName _resourceDir) override;
	virtual void updateGui(State state) override;

    virtual unsigned getMpcFetch() override { return 016; }

    virtual void onInit() override;
    virtual void onTimer(unsigned periodMs) override;

    virtual void setManClkEnable(bool manClkEnable) override;
    virtual void uStep() override;
    //virtual void uStepAutoUntilStop(uint32_t stopUpc, int stopUnibusCycle, uint32_t stopUnibusAddress, int stopRepeatCount) override;

    virtual void paintDocumentAnnotations() override;

    virtual void onResponseKM11Signals(ResponseKM11Signals* km11Signals) override;
    virtual void onResponseKY11LBSignals(ResponseKY11LBSignals* ky11Signals) override;
    bool ignoreKY11 = false; // testmode
    virtual void onResponseUnibusSignals(ResponseUnibusSignals* unibusSignals) override;
    virtual void evalUnibusCycle(ResponseUnibusCycle* unibusCycle) override;

    DocumentPageAnnotations uflowPageAnnotations;

    DocumentPageAnnotations datapathPageAnnotations;

    // search in current uflow MPC data for keys of datapth objects
    KeyPatternList uflowTodataPathKeyPatterns;


};
#endif // __PDP11ADAPTER34_H__


