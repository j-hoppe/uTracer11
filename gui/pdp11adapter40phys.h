/*  pdp11adapter40phys - Interface to physical PDP-11/40  via M93X2probe and KM11s
*/
#ifndef __PDP11ADAPTER40PHYS_H__
#define __PDP11ADAPTER40PHYS_H__

#include "wx/wx.h"
#include "pdp11adapter40.h"

class Pdp11Adapter40Physical : public Pdp11Adapter40 {
    using  Pdp11Adapter40::Pdp11Adapter40;

public:
	virtual ~Pdp11Adapter40Physical() override {}
    Type getType() override {
        return Type::pdp1140phys;
    }

    std::string getTypeLabel() override {
        return "PDP11/40 with M93X2probe and KM11";
    }
    // attach/arrange panels, set directories
    void setupGui(wxFileName _resourceDir) override;
};

#endif // __PDP11ADAPTER40PHYS_H__