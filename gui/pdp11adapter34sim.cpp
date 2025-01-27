/* pdp1134sim - Interface to simulated PDP-11/34
*/

#include "application.h"
#include "pdp11adapter34sim.h"

void Pdp11Adapter34Simulated::setupGui(wxFileName _resourceDir) {
    // generic panels before and optical left of specific ones
    Pdp11Adapter34::setupGui(_resourceDir);

    // hardware state not for simulation

    app->mainFrame->documentsNotebookFB->SetPageText(1, "PDP11/34 Simulator CPU signals"); // label for status panel

}


// Set State of control, visibility and functions
void Pdp11Adapter34Simulated::updateGui(State state) {
    // internal state panel ?
	Pdp11Adapter34::updateGui(state); // base
}


