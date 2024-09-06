/* pdp1140phys - Interface to physical PDP-11/40 via M93X2probe and KM11s
*/
#include "application.h"
#include "pdp11adapter40phys.h"


void Pdp11Adapter40Physical::setupGui(wxFileName _resourceDir) {
    // generic panels before and optical left of specific ones
    Pdp11Adapter40::setupGui(_resourceDir);

    auto app = &wxGetApp();

    // hardware state not for simulation!
    km11StatusPanel = new Pdp1140CpuKM11StatusPanelFB(app->mainFrame->pdp11StatusPanelFB);
    app->mainFrame->pdp11StatusSizerFB->Add(km11StatusPanel, 1, wxALL, 5);

    app->mainFrame->documentsNotebookFB->SetPageText(1, "PDP11/40 phys CPU signals"); // label for status panel
}
