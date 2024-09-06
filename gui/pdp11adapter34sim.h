/* pdp11adapter34sim - interface to simulated PDP-11/34
*/
#ifndef __PDP11ADAPTER34SIM_H__
#define __PDP11ADAPTER34SIM_H__

#include "pdp11adapter34.h"


class Pdp11Adapter34Simulated : public Pdp11Adapter34 {
    using  Pdp11Adapter34::Pdp11Adapter34;
public:

    virtual ~Pdp11Adapter34Simulated() override {}

    Type getType() override {
        return Type::pdp1134sim;
    }

    std::string getTypeLabel() override {
        return "Simulated PDP11/34";
    }
    // attach/arrange panels, set directories
    virtual void setupGui(wxFileName _resourceDir) override;

};

#endif // __PDP11ADAPTER34SIM_H__
