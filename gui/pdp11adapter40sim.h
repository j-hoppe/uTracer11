/* pdp11adapter40sim - Interface to simulated PDP-11/40
*/
#ifndef __PDP11ADAPTER40SIM_H__
#define __PDP11ADAPTER40SIM_H__

#include "wx/wx.h"
#include "pdp11adapter40.h"

class Pdp11Adapter40Simulated : public Pdp11Adapter40 {
public:
	virtual ~Pdp11Adapter40Simulated() override {}

    Type getType() override {
        return Type::pdp1140sim;
    }

    std::string getTypeLabel() override {
        return "PDP11/40 simulation";
    }

    // attach/arrange panels, set directories
    virtual void setupGui(wxFileName _resourceDir) override;
	virtual void updateGui(State state) override;

};

#endif // __PDP11ADAPTER40SIM_H__
