///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.1.0-0-g733bf3d)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/grid.h>
#include <wx/combobox.h>
#include <wx/filepicker.h>
#include <wx/textctrl.h>
#include <wx/tglbtn.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/timer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class Pdp1134CpuKY11LBStatusPanelFB
///////////////////////////////////////////////////////////////////////////////
class Pdp1134CpuKY11LBStatusPanelFB : public wxPanel
{
	private:

	protected:
		wxScrolledWindow* m_scrolledWindow1;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText13;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText17;

	public:
		wxStaticText* pdp1134Ky11MpcText;
		wxStaticText* pdp1134Ky11FpText;
		wxStaticText* pdp1134Ky11SbfText;
		wxStaticText* pdp1134Ky11LirText;
		wxStaticText* pdp1134Ky11PbpText;

		Pdp1134CpuKY11LBStatusPanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 352,253 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~Pdp1134CpuKY11LBStatusPanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class UnibusSignalsPanelFB
///////////////////////////////////////////////////////////////////////////////
class UnibusSignalsPanelFB : public wxPanel
{
	private:

	protected:
		wxPanel* m_panel8;
		wxPanel* m_panel11;
		wxStaticText* m_staticUnibusSignalText1;
		wxStaticText* m_staticUnibusSignalText12119;
		wxStaticText* m_staticUnibusSignalText12;
		wxStaticText* m_staticUnibusSignalText1211;
		wxStaticText* m_staticUnibusSignalText1213;
		wxStaticText* m_staticUnibusSignalText1215;
		wxPanel* m_panel9;
		wxPanel* m_panel12;
		wxStaticText* m_staticUnibusSignalText14;
		wxStaticText* m_staticText148;
		wxStaticText* m_staticText150;
		wxStaticText* m_staticText152;
		wxStaticText* m_staticText154;
		wxStaticText* m_staticText156;
		wxPanel* m_panel10;
		wxPanel* m_panel13;
		wxStaticText* m_staticText159;
		wxStaticText* m_staticText161;
		wxStaticText* m_staticText163;

	public:
		wxStaticText* unibusSignalAddrText;
		wxStaticText* unibusSignalDataText;
		wxStaticText* unibusSignalControlText;
		wxStaticText* unibusSignalMsynSsynText;
		wxStaticText* unibusSignalPaPbText;
		wxStaticText* unibusSignalIntrText;
		wxStaticText* unibusSignalBr4567Text;
		wxStaticText* unibusSignalBg4567Text;
		wxStaticText* unibusSignalNprText;
		wxStaticText* unibusSignalNpgText;
		wxStaticText* unibusSignalSackText;
		wxStaticText* unibusSignalBbsyText;
		wxStaticText* unibusSignalInitText;
		wxStaticText* unibusSignalAcloText;
		wxStaticText* unibusSignalDcloText;

		UnibusSignalsPanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~UnibusSignalsPanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class InternalStatePanelFB
///////////////////////////////////////////////////////////////////////////////
class InternalStatePanelFB : public wxPanel
{
	private:

	protected:

	public:
		wxScrolledWindow* internalStateFlexGridSizerPanelFB;
		wxFlexGridSizer* internalStateFlexGridSizerFB;

		InternalStatePanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~InternalStatePanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class TracePanelFB
///////////////////////////////////////////////////////////////////////////////
class TracePanelFB : public wxPanel
{
	private:

	protected:
		wxPanel* m_panel14;

		// Virtual event handlers, override them in your derived class
		virtual void TracePanelFBOnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void traceClearButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void traceToClipboardButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void traceGridFBOnSize( wxSizeEvent& event ) { event.Skip(); }


	public:
		wxButton* traceClearButtonFB;
		wxButton* traceToClipboardButtonFB;
		wxGrid* traceGridFB;

		TracePanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 589,458 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~TracePanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Pdp11DataPathPanelFB
///////////////////////////////////////////////////////////////////////////////
class Pdp11DataPathPanelFB : public wxPanel
{
	private:

	protected:

		// Virtual event handlers, override them in your derived class
		virtual void Pdp11DataPathPanelOnPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void Pdp11DataPathPanelOnSize( wxSizeEvent& event ) { event.Skip(); }


	public:

		Pdp11DataPathPanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~Pdp11DataPathPanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Pdp11uFlowPanelFB
///////////////////////////////////////////////////////////////////////////////
class Pdp11uFlowPanelFB : public wxPanel
{
	private:

	protected:

		// Virtual event handlers, override them in your derived class
		virtual void Pdp11uFlowPanelOnPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void Pdp11uFlowPanelOnSize( wxSizeEvent& event ) { event.Skip(); }


	public:

		Pdp11uFlowPanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~Pdp11uFlowPanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Pdp11uWordPanelFB
///////////////////////////////////////////////////////////////////////////////
class Pdp11uWordPanelFB : public wxPanel
{
	private:

	protected:

		// Virtual event handlers, override them in your derived class
		virtual void Pdp11uWordPanelOnPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void Pdp11uWordPanelOnSize( wxSizeEvent& event ) { event.Skip(); }


	public:

		Pdp11uWordPanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~Pdp11uWordPanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Pdp1140CpuKM11StatusPanelFB
///////////////////////////////////////////////////////////////////////////////
class Pdp1140CpuKM11StatusPanelFB : public wxPanel
{
	private:

	protected:
		wxPanel* m_panel15;
		wxPanel* m_panel161;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText13;
		wxStaticText* m_staticText172;
		wxStaticText* m_staticText1721;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText17;
		wxStaticText* m_staticText1115;
		wxStaticText* m_staticText1112;
		wxStaticText* m_staticText1113;
		wxStaticText* m_staticText1114;
		wxStaticText* m_staticText111;
		wxPanel* m_panel16;
		wxPanel* m_panel162;
		wxStaticText* m_staticText61;
		wxStaticText* m_staticText134;
		wxStaticText* m_staticText135;
		wxStaticText* m_staticText151;
		wxStaticText* m_staticText171;
		wxStaticText* m_staticText1311;
		wxStaticText* m_staticText1312;
		wxStaticText* m_staticText1313;
		wxStaticText* m_staticText1314;
		wxStaticText* m_staticText1315;
		wxStaticText* m_staticText13151;
		wxStaticText* m_staticText13152;
		wxStaticText* m_staticText13153;
		wxStaticText* m_staticText131531;
		wxStaticText* m_staticText131532;
		wxStaticText* m_staticText131533;

	public:
		wxStaticText* pdp1140Km11aPuppText;
		wxStaticText* pdp1140Km11aBuppText;
		wxStaticText* pdp1140Km11aTrapText;
		wxStaticText* pdp1140Km11aSsynText;
		wxStaticText* pdp1140Km11aMsynText;
		wxStaticText* pdp1140Km11aCText;
		wxStaticText* pdp1140Km11aVText;
		wxStaticText* pdp1140Km11aZText;
		wxStaticText* pdp1140Km11aNText;
		wxStaticText* pdp1140Km11aTText;
		wxStaticText* pdp1140Km11bPbaText;
		wxStaticText* pdp1140Km11bRomAbText;
		wxStaticText* pdp1140Km11bRomCText;
		wxStaticText* pdp1140Km11bRomDText;
		wxStaticText* pdp1140Km11bB15Text;
		wxStaticText* pdp1140Km11bEcin00Text;
		wxStaticText* pdp1140Km11bExpUnflText;
		wxStaticText* pdp1140Km11bExpOvflText;
		wxStaticText* pdp1140Km11bDr00Text;
		wxStaticText* pdp1140Km11bDr09Text;
		wxStaticText* pdp1140Km11bMsr00Text;
		wxStaticText* pdp1140Km11bMsr01Text;
		wxStaticText* pdp1140Km11bEpsCText;
		wxStaticText* pdp1140Km11bEpsVText;
		wxStaticText* pdp1140Km11bEpsZText;
		wxStaticText* pdp1140Km11bEpsNText;

		Pdp1140CpuKM11StatusPanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,512 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~Pdp1140CpuKM11StatusPanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class MemoryPanelFB
///////////////////////////////////////////////////////////////////////////////
class MemoryPanelFB : public wxPanel
{
	private:

	protected:
		wxPanel* memoryLoadPanel;
		wxPanel* m_panel16;
		wxStaticText* m_staticText45;
		wxStaticText* m_staticText44;
		wxPanel* m_panel17;
		wxPanel* m_panel13;
		wxStaticText* m_staticText48;
		wxStaticText* m_staticText50;
		wxStaticText* m_staticText46;
		wxButton* memoryGridClearButtonFB;
		wxStaticText* m_staticText47;
		wxButton* ioPageGridClearButtonFB;

		// Virtual event handlers, override them in your derived class
		virtual void memoryFileFormatComboBoxOnCombobox( wxCommandEvent& event ) { event.Skip(); }
		virtual void memoryLoadFilePickerOnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void depositMemoryButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void manualExamButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void manualDepositButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void memoryGridClearButtonFBOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void memoryGridFBOnGridCellLeftClick( wxGridEvent& event ) { event.Skip(); }
		virtual void ioPageGridClearButtonFBOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void ioPageGridFBOnGridCellLeftClick( wxGridEvent& event ) { event.Skip(); }


	public:
		wxComboBox* memoryFileFormatComboBox;
		wxFilePickerCtrl* memoryLoadFilePickerWxFB;
		wxButton* depositMemoryButton;
		wxTextCtrl* manualExamDepositAddrTextCtrl;
		wxButton* manualExamButton;
		wxTextCtrl* manualExamDepositDataTextCtrl;
		wxButton* manualDepositButton;
		wxGrid* memoryGridFB;
		wxGrid* ioPageGridFB;

		MemoryPanelFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 32767,588 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~MemoryPanelFB();

};

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrameFB
///////////////////////////////////////////////////////////////////////////////
class MainFrameFB : public wxFrame
{
	private:

	protected:
		wxPanel* m_panel23;
		wxPanel* ScriptPanelFB;
		wxStatusBar* statusBar;

		// Virtual event handlers, override them in your derived class
		virtual void OnShow( wxShowEvent& event ) { event.Skip(); }
		virtual void powerCycleButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void manClockEnableButtonOnToggleButton( wxCommandEvent& event ) { event.Skip(); }
		virtual void microStepButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void scriptLoadButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void scriptSaveButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void scriptRunButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void scriptAbortButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void updateTimerOnTimer( wxTimerEvent& event ) { event.Skip(); }


	public:
		wxBoxSizer* topHorizontalSizer;
		wxNotebook* toolsNotebook;
		wxPanel* runPanel;
		wxStaticText* uMachineStateText;
		wxStaticText* uMachineStateText2;
		wxButton* powerCycleButton;
		wxToggleButton* manClockEnableButton;
		wxButton* microStepButton;
		wxPanel* scriptPanel;
		wxButton* scriptLoadButton;
		wxButton* scriptSaveButton;
		wxButton* scriptRunButton;
		wxButton* scriptAbortButton;
		wxStaticText* scriptStatusText;
		wxNotebook* documentsNotebookFB;
		wxPanel* EventPanelFB;
		wxTextCtrl* eventsTextCtrl;
		wxTextCtrl* scriptTextCtrlFB;
		wxPanel* pdp11StatusPanelFB;
		wxBoxSizer* pdp11StatusSizerFB;
		wxTimer updateTimer;

		MainFrameFB( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("uTracer11"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 604,498 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainFrameFB();

};

