/* pdp1134phys - Interface to physical PDP-11/34 via M93X2probe and KY11-LB
*/

#include "application.h"
#include "pdp11adapter34phys.h"

// phys and sim have slightly different GUI

// attach/arrange panels, set directories
void Pdp11Adapter34Physical::setupGui(wxFileName _resourceDir) {
    auto app = &wxGetApp();
    // generic panels before and optical left of specific ones
    Pdp11Adapter34::setupGui(_resourceDir);
    
    // hardware state not for simulation!
    ky11StatusPanel = new Pdp1134CpuKY11LBStatusPanelFB(app->mainFrame->pdp11StatusPanelFB);
    app->mainFrame->pdp11StatusSizerFB->Add(ky11StatusPanel, 1, wxALL, 5);

    app->mainFrame->documentsNotebookFB->SetPageText(1, "PDP11/34 phys CPU signals"); // label for statuspanel
}

