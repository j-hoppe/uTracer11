/* pdp11adapter40 - base interface to physical and simulated PDP-11/40
*/
#ifndef __PDP11ADAPTER40_H__
#define __PDP11ADAPTER40_H__

#include "wx/wx.h"
#include "pdp11adapter.h"
#include "documentpageannotations.h"
#include "keywords.h"

// Signals exposed o the PDP-11/40 KM11 A and B diagnostic headers
class Pdp1140KM11State {
public:
    // member variables are exact the labels on DEC 11/40 KM11 overlay sheets
    // inputs from CPU to KM11A
    uint16_t pupp = 0;
    uint16_t bupp = 0;
    bool trap = 0;
    bool ssyn = 0;
    bool msyn = 0;
    bool t = 0;
    bool c = 0;
    bool v = 0;
    bool z = 0;
    bool n = 0;
    // inputs MMU/FPU to KM11B
    uint32_t pba = 0; //17..08
    bool rom_a = 0;
    bool rom_b = 0;
    bool rom_c = 0;
    bool rom_d = 0;
    bool b_15 = 0;
    bool ecin_00 = 0;
    bool exp_unfl = 0;
    bool exp_ovfl = 0;
    bool dr00 = 0;
    bool dr09 = 0;
    bool msr00 = 0;
    bool msr01 = 0;
    bool eps_c = 0;
    bool eps_v = 0;
    bool eps_z = 0;
    bool eps_n = 0;

    // outputs from KM11A to CPU
    bool mstop;
    bool mclk;
    bool mclk_enab;

    // lowlevel encode/decode
    void inputsFromKM11A(ResponseKM11Signals* respKm11A);
    void inputsFromKM11B(ResponseKM11Signals* respKm11B);
    void outputsToKM11A(RequestKM11SignalsWrite* reqKm11A);
    // no outputs over KM11B
}
;

class Pdp11Adapter40 : public Pdp11Adapter {

public:
    virtual std::string getTypeLabel() override {
        return "PDP11/40 with M93X2probe and KY11";
    }
    Pdp11Adapter40() : Pdp11Adapter() {}
	virtual ~Pdp11Adapter40() override {}

    // attach/arrange panels, set directories
    Pdp11uFlowPanel* uFlowPanel = nullptr;
    Pdp1140CpuKM11StatusPanelFB* km11StatusPanel = nullptr;
    virtual void setupGui(wxFileName _resourceDir) override;

    Pdp1140KM11State km11State; // CPU internal signals. not clear wether used in simulator ?

    virtual unsigned getMpcFetch() override { return 016; }

    virtual void onInit() override;
    virtual void onTimer(unsigned periodMs) override;

    virtual void setManClkEnable(bool manClkEnable) override;
    virtual void uStep() override;
    virtual void uStepUntilStop(uint32_t stopUpc, int stopUnibusCycle, uint32_t stopUnibusAddress, int stopRepeatCount) override;

    virtual void paintDocumentAnnotations() override;

    bool ignoreKM11 = false; // testmode
    virtual void onResponseKM11Signals(ResponseKM11Signals* km11Signals) override;
    virtual void onResponseKY11LBSignals(ResponseKY11LBSignals* ky11Signals) override;
    virtual void onResponseUnibusSignals(ResponseUnibusSignals* unibusSignals) override;
    virtual void evalUnibusCycle(ResponseUnibusCycle* unibusCycle) override;

    DocumentPageAnnotations uflowPageAnnotations;

    DocumentPageAnnotations datapathPageAnnotations;

    // search in current uflow MPC data for keys of datapth objects
    KeyPatternList uflowTodataPathKeyPatterns;

    // document panels
    //pdp1140uFlowPanel = new Pdp1140uFlowPanel(mainFrame->documentsNotebookFB);



};
#endif // __PDP11ADAPTER40_H__


