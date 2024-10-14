// mainframe - implementation
// main GUI logic here

#include "application.h"
#include "mainframe.h"

// the frame provides system with polling clock, defined by Start() in application
void MainFrame::updateTimerOnTimer(wxTimerEvent& event) {
    UNREFERENCED_PARAMETER(event);
    wxGetApp().onTimer(updateTimer.GetInterval());
}

void MainFrame::powerCycleButtonOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    wxGetApp().pdp11Adapter->powerDown();
    wxGetApp().pdp11Adapter->powerUp();
}

void MainFrame::manClockEnableButtonOnToggleButton(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    wxGetApp().pdp11Adapter->setManClkEnable(manClockEnableButton->GetValue());
}

void MainFrame::microStepButtonOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    wxGetApp().pdp11Adapter->uStep();
}

void MainFrame::autoStepButtonOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11Adapter = wxGetApp().pdp11Adapter;
    if (pdp11Adapter->state == Pdp11Adapter::State::uMachineAutoStepping) {
        pdp11Adapter->stopAutoStepping = true; // atomic signal to execution loop
    }
    else {
        // Not running? then start!
		// gather stop upc, repeat, unibus cycle, unibus addr
        int stopUpc;
        if (!stopUpcTextCtrl->GetValue().ToInt(&stopUpc, 8))
			stopUpc = Pdp11Adapter::InvalidMpc ;
        int stopUnibusCycle = stopUnibusCycleComboBox->GetSelection(); // 01=DATI,1=DATO, 2=any
        int stopUnibusAddress;
        if (!stopUnibusAddrTextCtrl->GetValue().ToInt(&stopUnibusAddress, 8))
			stopUnibusAddress = Pdp11Adapter::InvalidUnibusAddress ;
        int stopRepeatCount;
        stopRepeatCountTextCtrl->GetValue().ToInt(&stopRepeatCount, 10);
        pdp11Adapter->doAutoStepping(stopUpc, stopUnibusCycle, stopUnibusAddress, stopRepeatCount);
        // Start command loop ... optimal in parallel thread
        // but we must update GUI controls .
    }
}


void MainFrame::OnShow(wxShowEvent& event) {
    UNREFERENCED_PARAMETER(event);
    // must have a status bar designed
    //SetStatusText(_T("Ready!"));
    //        event.Skip(); 
}

/*
void MainFrame::calcButtonOnButtonClick(wxCommandEvent& event) {

    event.Skip();
}

void MainFrame::saveToClipboardButtonOnButtonClick(wxCommandEvent& event) {
    event.Skip();
}
*/


// on notebook page flip? on show? also on resize?
void Pdp11uFlowPanel::Pdp11uFlowPanelOnPaint(wxPaintEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    if (pdp11 == nullptr)
        return;
    // reload image and scale
    pdp11->paintDocumentAnnotations();
}


// painting is slow, check out 
// void wxEvtHandler::CallAfter() 	
// wxFULL_REPAINT_ON_RESIZE 

void Pdp11uFlowPanel::Pdp11uFlowPanelOnSize(wxSizeEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    if (pdp11 == nullptr)
        return;
    // reload image and scale
    pdp11->paintDocumentAnnotations();
}


void MemoryPanel::memoryLoadFilePickerOnFileChanged(wxFileDirPickerEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    // decode combo
    memory_fileformat_t fileFormat = fileformat_none;

    switch (memoryFileFormatComboBox->GetSelection()) {
    case 0: fileFormat = fileformat_addr_value_text; break;
    case 1: fileFormat = fileformat_macro11_listing; break;
    case 2: fileFormat = fileformat_papertape; break;
    }
    if (fileFormat == fileformat_none)
        wxLogError("Invalid File format selected?");
    else
        pdp11->loadMemoryFile(event.GetPath(), fileFormat);
}

void MemoryPanel::depositMemoryButtonOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    pdp11->depositMemoryImage();
}

void MemoryPanel::manualExamButtonOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    uint32_t addr;
    if (!manualExamDepositAddrTextCtrl->GetValue().ToUInt(&addr, 8)) {
        manualExamDepositAddrTextCtrl->SetValue("000000"); // do nothing else
        return;
    }
    addr &= 0x3ffff; // make 18 bit
    pdp11->singleExam(addr);
}

void MemoryPanel::manualDepositButtonOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    uint32_t addr;
    if (!manualExamDepositAddrTextCtrl->GetValue().ToUInt(&addr, 8)) {
        manualExamDepositAddrTextCtrl->SetValue("000000"); // do nothing else
        return;
    }
    addr &= 0x3ffff; // make 18 bit
    uint32_t data;
    if (!manualDepositDataTextCtrl->GetValue().ToUInt(&data, 8)) {
        manualDepositDataTextCtrl->SetValue("000000"); // do nothing else
        return;
    }
    pdp11->singleDeposit(addr, (uint16_t)data);
}

// click into memroy grid: copy addr&data to manual exam/deposit addr&data
void MemoryPanel::memoryGridFBOnGridCellLeftClick(wxGridEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    pdp11->onMemoryGridClick(&pdp11->memoryGridController, event.GetRow(), event.GetCol());
}

void MemoryPanel::ioPageGridFBOnGridCellLeftClick(wxGridEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    pdp11->onMemoryGridClick(&pdp11->ioPageGridController, event.GetRow(), event.GetCol());
}

void MemoryPanel::memoryGridClearButtonFBOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    pdp11->clearMemoryImage(&pdp11->memoryGridController);
}

void MemoryPanel::ioPageGridClearButtonFBOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    pdp11->clearMemoryImage(&pdp11->ioPageGridController);
}

void Pdp1134DataPathPanel::Pdp11DataPathPanelOnPaint(wxPaintEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    if (pdp11 == nullptr)
        return;
    // reload image and scale
    pdp11->paintDocumentAnnotations();
}

void Pdp1134DataPathPanel::Pdp11DataPathPanelOnSize(wxSizeEvent& event)
{
    UNREFERENCED_PARAMETER(event);
    auto pdp11 = wxGetApp().pdp11Adapter;
    if (pdp11 == nullptr)
        return;
    // reload image and scale
    pdp11->paintDocumentAnnotations();
}


void TracePanel::traceClearButtonOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
}

void TracePanel::traceToClipboardButtonOnButtonClick(wxCommandEvent& event)
{
    UNREFERENCED_PARAMETER(event);
}

// trace grid to max width
void TracePanel::TracePanelFBOnSize(wxSizeEvent& event) {
    UNREFERENCED_PARAMETER(event);
    // adapt the width of the grid to the parent panel, leave the height generated by sizer
    auto w = GetClientRect().width - wxSYS_VSCROLL_X;
    auto h = GetSize().GetHeight() - wxSYS_HSCROLL_Y;
    //traceGridFB->SetMinSize(wxSize(w, h));
    traceGridFB->SetSize(wxSize(w, h));
    //traceGridFB->SetColSize(0, 300);
    traceGridFB->AutoSizeColumns();
    traceGridFB->ForceRefresh();
}

