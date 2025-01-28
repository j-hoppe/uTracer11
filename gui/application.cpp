/* application - wxWidgets frame and object factory



There are many GUI panels.
  - screen filling ones plug to the main "DocumentsNoteBook"
  - Smaller data plug to the "Status" sub notebook


  Panels include:
     - Unibus signal lines,
     - CPU state as measured by KM11 and KY11 hardware probes
     - internal CPU state from simulators
     - "Trace": captured micro program events and UNIBUS cycles with disassembly
     - "UFlow": images of annotated DEC doc: micro program flow
     - "UWord: "   "             "       " : current microWord fields PROMS
     - "Datapath"   "             "       " : Datapath componets

  Panels depend on PDP11base model 11/34,40,05,..) and implementation (physical, simulation)
  They are owned by members of the pdp11adapter* classes hierarchy
  and placed by derived and inherited setupGui() methods.

  GUI panels are added programmatically from "generic to "specfic",
  Visual panel order then automatically: "from left to right"

  Snapshot June 2024:

  Panel                     Parent              Owning class
  ----------------          ------              ------------
  Events,Errorlog           DocumentsNoteBook   application
  Memory                    DocumentsNoteBook   pdp11adapter
  Trace                     DocumentsNoteBook   pdp11adapter

  Datapath 11/34            DocumentsNoteBook   pdp11adapter34
  Micro program flow 11/34  DocumentsNoteBook   pdp11adapter34

  Datapath 11/40            DocumentsNoteBook   pdp11adapter40
  Micro program flow 11/40  DocumentsNoteBook   pdp11adapter40

  CPU Signals:
  Unibus signals            StatusPanel         pdp11adapter
  KY11 signals              StatusPanel         pdp11adapter34phys
  11/34 internal state      StatusPanel         pdp11adapter34sim
  KM11  signals             StatusPanel         pdp11adapter40phys
  11/40 internal state      StatusPanel         pdp11adapter40sim
  */





  // For compilers that don't support precompilation, include "wx/wx.h"
  // Include search path $(WXWIN)\include;$(WXWIN)\include\msvc
  // LInker: <AdditionalLibraryDirectories>$(WXWIN)\lib\vc_x64_lib\< / AdditionalLibraryDirectories>

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <sstream>
#include "logger.h"
#include "application.h"

#include "messageinterfaceserial.h"
#include "messageinterfacetcpip.h"

#include "pdp11adapter34phys.h"
#include "pdp11adapter34sim.h"
#include "pdp11adapter40phys.h"
#include "pdp11adapter40sim.h"

//#include "tracecontroller.h"

IMPLEMENT_APP(Application)

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
     { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters",
          wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
     { wxCMD_LINE_OPTION , "t", "pdptype", "PDP-11 model like \"pdp1134phys\"",
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY  },
#ifdef __WINDOWS__
     { wxCMD_LINE_OPTION , "sp", "serial", "serial port to PDP-11 hardware probe, like \"COM2\"",
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
#else
     { wxCMD_LINE_OPTION , "sp", "serial", "serial port to PDP-11 hardware probe, like \"/dev/ttyS0\"",
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
#endif
     { wxCMD_LINE_OPTION , "sa", "socket", "TCP/IP socket address of remote PDP-11, like \"localhost:65392\"",
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
     { wxCMD_LINE_OPTION , "rd", "resourcedir", "path to resource directory",
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY  },
     { wxCMD_LINE_OPTION , "sf", "symbolfile", "file with disassembler symbols in resource directory",
          wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
     { wxCMD_LINE_NONE, nullptr, nullptr, nullptr, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM }
};

// called before OnInit()
void Application::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc(g_cmdLineDesc);
#ifdef __WINDOWS__
    parser.SetSwitchChars("-/");
#else
    // Linuxes: must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars("-");
#endif
}

bool Application::OnCmdLineHelp(wxCmdLineParser& parser)
{
    wxString msg = parser.GetUsageString();
    wxMessageBox(msg, "Commandline help", wxICON_INFORMATION);
    return false;
}

bool Application::OnCmdLineError(wxCmdLineParser& parser)
{
    wxString msg = parser.GetUsageString();
    wxMessageBox(msg, "Commandline error", wxICON_ERROR);
    return false;
}


// called before OnInit()
// so delay processing into evalCmdLine(), after Logger setup
//
// instantiate PDP11 and Com port
bool Application::OnCmdLineParsed(wxCmdLineParser& parser)
{
    wxString cmdlinePdp11TypeName, cmdlineResourceDir, cmdlineSymbolFile;
    wxString errorMessage;
    cmdlineSerialPortName = "";
    cmdlineSocketAddress = "";
    parser.Found("serial", &cmdlineSerialPortName);
    parser.Found("socket", &cmdlineSocketAddress);
    parser.Found("pdptype", &cmdlinePdp11TypeName);
    parser.Found("resourcedir", &cmdlineResourceDir);
    cmdlineResourceDir += wxFileName::GetPathSeparator(); // slash, so it's a directory path
    cmdLineResourceRootDir = wxFileName(cmdlineResourceDir);
    if (parser.Found("symbolfile", &cmdlineSymbolFile))
        cmdLineResourceSymbolFilePath = cmdlineResourceDir + cmdlineSymbolFile;
    else cmdLineResourceSymbolFilePath.Clear();

    // validate pdp11 type.
    // instantiate pdp11 model later and set GUI, no app here yet
    cmdLinePdp11Type = Pdp11Adapter::parseTypeName(cmdlinePdp11TypeName.ToStdString());
    if (cmdLinePdp11Type == Pdp11Adapter::Type::none) {
        std::vector<std::string> vs = Pdp11Adapter::getTypeNames();
        std::stringstream ss; // all names as single comma list string for error help
        for (auto it = vs.begin(); it != vs.end(); it++) {
            ss << *it;
            if (it != vs.end() - 1)
                ss << ", ";
        }
        //std::copy(vs.begin(), vs.end(), std::ostream_iterator<std::string>(ss, ","));
        errorMessage = wxString::Format("Invalid PDP11 type \"%s\"\nPossible: %s", cmdlinePdp11TypeName, ss.str());
    } // port now open

    if (!errorMessage.IsEmpty()) {
        wxMessageBox(errorMessage, "Command line error", wxICON_ERROR);
        abort();
    }

    // validate resource directory in setPdp11Type()

    /*
  // to get at your unnamed parameters use
  wxArrayString files;
  for (int i = 0; i < parser.GetParamCount(); i++) {
      files.Add(parser.GetParam(i));
  }
  // and other command line parameters
  // then do what you need with them.
  */
    return true;
}


int Application::FilterEvent(wxEvent& event)
{
    if (event.GetEventType() == wxEVT_KEY_DOWN) {
        wxKeyEvent& kevent = (wxKeyEvent&)event;
        //int keyCode = ((wxKeyEvent&)event).GetKeyCode();
        switch (kevent.GetKeyCode()) {
        case WXK_F2: // man clock enable
            mainFrame->manClockEnableButton->SetValue(true);
            pdp11Adapter->setManClkEnable(true);
            return Event_Processed;
        case WXK_F3: // man clock disable
            mainFrame->manClockEnableButton->SetValue(false);
            pdp11Adapter->setManClkEnable(false);
            return Event_Processed;
        case WXK_F4: // single step
            pdp11Adapter->requestUStep();
            return Event_Processed;
        case WXK_F10: // toggle full screen
        	if (mainFrame->IsFullScreen()) {
				mainFrame->ShowFullScreen(false, 0);
        	} else {
		        mainFrame->ShowFullScreen(true, 0); // with menubars, caption an all other decoration
        	}
			return Event_Processed;
        }
    }
    return Event_Skip; // continue processing all others
}

// when called?
bool Application::OnExceptionInMainLoop()
{
    wxString error;
    try {
        throw; // rethrow, so I can catch
    }
    catch (const std::exception& e) {
        error = e.what();
    }
    wxLogError("Unexpected exception has occurred: %s, the program will terminate.", error);
    return false;
}

// This is executed upon startup, like 'main()' in non-wxWidgets programs.
bool Application::OnInit()
{
    try {
        //default behaviour, Mandatory
        if (!wxApp::OnInit())
            return false;

        // deal with jpeg images, call before first use of wxImage
        wxImage::AddHandler(new wxJPEGHandler) ;

        /* returns always 0 ??
        wxString name;
        wxOperatingSystemId id = wxPlatformInfo::GetOperatingSystemId(name);
        isOsWindows = (id & wxOS_WINDOWS) > 0;
        isOsLinux = (id & wxOS_UNIX_LINUX) > 0;
        */

        mainFrame = new MainFrame(nullptr);

        // messageInterface instantiate in commandline evaluation

        mainFrame->toolsNotebook->SetSelection(0);

        // now the loggerTextCtl exists
        //wxLog::SetActiveTarget(nullptr);
        //wxLog::SetActiveTarget(new MyLogTarget(mainFrame->logTextCtrl, "log.txt"));
        // is also system log
        wxLog::SetActiveTarget(new MyLogTarget(mainFrame->eventsTextCtrl, "log.txt"));
        wxLog::SetLogLevel(wxLOG_Max); // all!


        mainFrame->toolsNotebook->SetSelection(0);

        connectToMessageInterface();

        // instantiate a pdp11 and setup GUI
        setPdp11Type(cmdLinePdp11Type, cmdLineResourceRootDir); // can not be done in cmd line parser, no app there

        mainFrame->Show(true);
        mainFrame->ShowFullScreen(true, 0); // with menubars, caption an all other decoration
        mainFrame->topHorizontalSizer->Layout(); // arrange subwindows


        wxLogStatus(this, "Status test");
        wxLogDebug("Debug test");
        wxLogMessage("Message test");
        wxLogInfo("Info test");
        //wxLogError("Error test");


        wxLogInfo("uTracer11 software up&running!");
#if defined( __WINDOWS__)
        wxLogInfo("Platform is Windows");
#elif defined(__LINUX__)
        wxLogInfo("Platform is Linux");
#else
        wxLogInfo("Platfom is unknown");
#endif

        SetTopWindow(mainFrame);

        // last: enable Timer with 10 Millisconds period
        mainFrame->updateTimer.Start(10, wxTIMER_CONTINUOUS);

        mainFrame->Layout();
        mainFrame->Refresh();
		
		wxLogInfo("Toggle fullscreen with F10");

	
	// query version from M93X2probe
	messageInterface->xmtRequest(new RequestVersionRead(MsgTag::next));
	// ResponseVersion via readevent, calls SetLabel()
	mainFrame->SetLabel("uTracer11 - waiting for connection"); // default if ResponseVersion fails

	// query internal state panel, if PDP11 exposes any	
	// response adds vars to stateVars
	messageInterface->xmtRequest(new RequestRegDef(MsgTag::next));

    }
    catch (const std::exception& e) {
        wxLogFatalError("Unexpected exception in initialization: %s", e.what());
    }
    return true;
}


void Application::connectToMessageInterface()
{
    if (cmdlineSerialPortName.IsEmpty() && cmdlineSocketAddress.IsEmpty()) {
        wxMessageBox("Neither --serial nor --socket specified", "Command line error", wxICON_ERROR);
        abort();
    }
    else if (!cmdlineSerialPortName.IsEmpty() && !cmdlineSocketAddress.IsEmpty()) {
        wxMessageBox("Both --serial and --socket specified", "Command line error", wxICON_ERROR);
        abort();
    }


    if (!cmdlineSerialPortName.IsEmpty()) {
        // it's a pdp11 which uses a M93202 probe viaRS232. validate serial port
        auto messageInterfaceSerial = new MessageInterfaceSerial();
        if (!messageInterfaceSerial->connect(cmdlineSerialPortName.ToStdString())) {
            wxLogFatalError("Invalid serial port \"%s\"", cmdlineSerialPortName);
        } // port now open
        messageInterface = messageInterfaceSerial;
    }
    if (!cmdlineSocketAddress.IsEmpty()) {

        // it's a pdp11 which uses a M93202 probe viaRS232. validate serial port
        auto messageInterfaceTcpIp = new MessageInterfaceTcpIp();
        if (!messageInterfaceTcpIp->connect(cmdlineSocketAddress.ToStdString())) {
            wxLogFatalError("Invalid serial port \"%s\"", cmdlineSerialPortName);
        } // port now open
        messageInterface = messageInterfaceTcpIp;
    }
}


// instantiate correct Pdp11 logic, call only once!
// assign path to resources,
// setup GUI Notebook panels
void Application::setPdp11Type(Pdp11Adapter::Type pdp11Type, wxFileName _resourceDir)
{
    // wipe status panel for all cpu type notebooks
    //wxSizer* sizer = mainFrame->pdp11StatusSizerFB;
    //while (sizer->GetItemCount() > 0)
    //    sizer->Remove(0);

    if (pdp11Adapter != nullptr)
        delete pdp11Adapter;

    // validate resource directory
    _resourceDir.MakeAbsolute();
    if (!_resourceDir.Exists()) {
        auto errormsg = wxString::Format("Invalid resource root directory \"%s\"", _resourceDir.GetFullPath());
        wxMessageBox(errormsg, "Command line error", wxICON_ERROR);
        abort();
    }

    switch (pdp11Type) {
    case Pdp11Adapter::Type::pdp1134phys: {
        pdp11Adapter = new Pdp11Adapter34Physical();
        pdp11Adapter->setupGui(_resourceDir);
        break;
    }
    case Pdp11Adapter::Type::pdp1134sim: {
        pdp11Adapter = new Pdp11Adapter34Simulated();
        pdp11Adapter->setupGui(_resourceDir);
        break;
    }
    case Pdp11Adapter::Type::pdp1140phys: {
        pdp11Adapter = new Pdp11Adapter40Physical();
        pdp11Adapter->setupGui(_resourceDir);
        break;
    }
    case Pdp11Adapter::Type::pdp1140sim: {
        pdp11Adapter = new Pdp11Adapter40Simulated();
        pdp11Adapter->setupGui(_resourceDir);
        break;
    }
    default:
        pdp11Adapter = nullptr;
    }
    // for simplicity, check final resource dir *after* object generation ...
    // we terminate anyhow
    if (!_resourceDir.Exists()) {
        auto errormsg = wxString::Format("Invalid resource directory \"%s\"", _resourceDir.GetFullPath());
        wxMessageBox(errormsg, "Command line error", wxICON_ERROR);
        abort();
    }

    if (pdp11Adapter == nullptr)
        return;

    //wxLogInfo("Resource dir %s valid", _resourceDir.GetFullPath());
    pdp11Adapter->updateGui(Pdp11Adapter::State::init) ;
    pdp11Adapter->onInit();
    pdp11Adapter->updateGui(Pdp11Adapter::State::init) ;
}



void Application::onTimer(unsigned periodMs)
{
    messageInterface->receiveAndProcessResponses();
    pdp11Adapter->onTimer(periodMs);
}

