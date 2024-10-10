/* pdp11adapter34phys - interface to physical PDP-11/34 via M93X2probe and KY11-LB
*/
#ifndef __PDP11ADAPTER34PHYS_H__
#define __PDP11ADAPTER34PHYS_H__

#include "pdp11adapter34.h"


class Pdp11Adapter34Physical : public Pdp11Adapter34 {
    using  Pdp11Adapter34::Pdp11Adapter34;
public:
	
    virtual ~Pdp11Adapter34Physical() override {}
	
    Type getType() override {
        return Type::pdp1134phys;
    }

    std::string getTypeLabel() override {
        return "PDP11/34 with M93X2probe and KY11";
    }

    // attach/arrange panels, set directories
    virtual void setupGui(wxFileName _resourceDir) override;
	virtual void updateGui(State state) override;

};

#endif // __PDP11ADAPTER34PHYS_H__
