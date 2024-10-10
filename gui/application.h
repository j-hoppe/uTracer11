
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <wx/cmdline.h>
#include "messageinterface.h"
#include "pdp11adapter.h"

#include "mainframe.h"


// which platform?
#ifdef WIN32
#define __WINDOWS__
// #else
// #define __LINUX__  defined by default in gcc
#endif

class MessageInterface;

// The Application class. This class shows a window
// containing a statusbar with the text "Hello World"
class Application : public wxApp
{
	public:
    MessageInterface* messageInterface = nullptr;
    MainFrame* mainFrame = nullptr;
    // no notebook pages yet, document noteboks are empty

    // app wide hot keys
    int FilterEvent(wxEvent& event) override;

    enum Pdp11Adapter::Type cmdLinePdp11Type; // as parsed
    wxFileName cmdLineResourceRootDir;
    wxFileName cmdLineResourceSymbolFilePath; // for disas
    wxString cmdlineSerialPortName;
    wxString cmdlineSocketAddress;
    Pdp11Adapter* pdp11Adapter = nullptr; // one of several subclasses

    virtual bool OnExceptionInMainLoop() override;
    virtual bool OnInit() override;
    virtual void OnInitCmdLine(wxCmdLineParser& parser) override;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;

    void connectToMessageInterface();

    void setPdp11Type(Pdp11Adapter::Type pdp11Type, wxFileName _cmdLineResourceRootDir);

    void onTimer(unsigned periodMs); // central polling clock from wxFrame

    // event handler, entered into wxFormBuilder
    // void calculateButtonOnClick(wxCommandEvent& event);
};

DECLARE_APP(Application)

#endif // __APPLICATION_H__
