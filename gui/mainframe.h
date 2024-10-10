// MainFrame
// wrapper for mainform build with wxFormBuilder
// generated main class is "MainFrameFB"
#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#include "uTracer11FB.h"

// inherit to implement own event handlers
class MainFrame : public MainFrameFB {
    using MainFrameFB::MainFrameFB;
public:
    // inherited event handlers

    virtual void OnShow(wxShowEvent& event) override;

    virtual void updateTimerOnTimer(wxTimerEvent& event) override;
    virtual void powerCycleButtonOnButtonClick(wxCommandEvent& event) override;
    virtual void manClockEnableButtonOnToggleButton(wxCommandEvent& event) override;
    virtual void microStepButtonOnButtonClick(wxCommandEvent& event) override;
    virtual void microRunUntilButtonOnButtonClick(wxCommandEvent& event) override;
};

// Notebook pages are own panels, not assigned to notebooks by wxFormBuilder project
// but in code on PDP-11 model selection
// Own version derived from *FB only necessary if event handling needed,

class MemoryPanel : public MemoryPanelFB {
    using MemoryPanelFB::MemoryPanelFB;
public:
    virtual void memoryLoadFilePickerOnFileChanged(wxFileDirPickerEvent& event) override;
    virtual void depositMemoryButtonOnButtonClick(wxCommandEvent& event) override;
    virtual void manualExamButtonOnButtonClick(wxCommandEvent& event) override;
    virtual void manualDepositButtonOnButtonClick(wxCommandEvent& event) override;
    virtual void memoryGridFBOnGridCellLeftClick(wxGridEvent& event) override;
    virtual void ioPageGridFBOnGridCellLeftClick(wxGridEvent& event) override;
    virtual void memoryGridClearButtonFBOnButtonClick(wxCommandEvent& event) override;
    virtual void ioPageGridClearButtonFBOnButtonClick(wxCommandEvent& event) override;
};

class TracePanel : public TracePanelFB {
    using TracePanelFB::TracePanelFB;
public:
    virtual void traceClearButtonOnButtonClick(wxCommandEvent& event) override;
    virtual void traceToClipboardButtonOnButtonClick(wxCommandEvent& event) override;
    virtual void TracePanelFBOnSize(wxSizeEvent& event) override;
};

class Pdp11uFlowPanel : public Pdp11uFlowPanelFB {
    using Pdp11uFlowPanelFB::Pdp11uFlowPanelFB;
public:
    virtual void Pdp11uFlowPanelOnPaint(wxPaintEvent& event) override;
    virtual void Pdp11uFlowPanelOnSize(wxSizeEvent& event) override;
};

class Pdp1134DataPathPanel : public Pdp11DataPathPanelFB {
    using Pdp11DataPathPanelFB::Pdp11DataPathPanelFB;
public:
    virtual void Pdp11DataPathPanelOnPaint(wxPaintEvent& event) override;
    virtual void Pdp11DataPathPanelOnSize(wxSizeEvent& event) override;
};


#endif // __MAINFRAME_H__

