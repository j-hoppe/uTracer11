/* pdp1140sim - Interface to simulated PDP-11/40
*/
#include "application.h"
#include "pdp11adapter40sim.h"

void Pdp11Adapter40Simulated::setupGui(wxFileName _resourceDir) {
    // generic panels before and optical left of specific ones
    Pdp11Adapter40::setupGui(_resourceDir);

    // hardware state not for simulation

    app->mainFrame->documentsNotebookFB->SetPageText(1, "PDP11/40 sim CPU signals"); // label for status panel
}



// Set State of control, visibility and functions
void Pdp11Adapter40Simulated::updateGui(State state) {
	Pdp11Adapter40::updateGui(state); // base
}



