/* logtarget to wxtextCtrl, optional to file
  https://stackoverflow.com/questions/73614571/how-to-split-log-messages-into-file-and-screen-in-wxwidgets

  Usage:
class MyFrame : public wxFrame
{
public:
    MyFrame(wxWindow* parent = nullptr) : wxFrame(parent, wxID_ANY, "Test")
    {
        wxTextCtrl* logCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
        wxLog::SetActiveTarget(new MyLogTarget(logCtrl, "log.txt"));

        wxLogDebug("Debug test");
        wxLogMessage("Message test");
        wxLogInfo("Info test");
        wxLogError("Error test");
    }
    ~MyFrame()
    {
        delete wxLog::SetActiveTarget(nullptr);
    }
};

class MyApp : public wxApp
{
public:
    bool OnInit() override
    {
        (new MyFrame())->Show();
        return true;
    }
}; wxIMPLEMENT_APP(MyApp);
*/
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <wx/log.h>
#include <wx/defs.h>
#include <wx/ffile.h>
#include <wx/wx.h>
#include "application.h" // wxGetApp()


class MyLogTarget : public wxLog
{
public:
    // textCtrl must have longer lifetime than MyLogTarget
    MyLogTarget(wxTextCtrl* textCtrl, const wxString& fileName)
        : m_textCtrl(textCtrl), m_file(fileName, "a")
    {}

    wxMessageOutputDebug messageOutputDebug;

    std::string levelText(wxLogLevel level) {
        switch (level) {
        case wxLOG_FatalError: return "FATAL";
        case wxLOG_Error: return "ERROR";
        case wxLOG_Warning: return "WARNING";
        case wxLOG_Message: return "MSG";
        case wxLOG_Status: return "STATUS";
        case wxLOG_Info: return "INFO";
        case wxLOG_Debug: return "DEBUG";
        case wxLOG_Trace: return "TRACE";
        case wxLOG_Progress: return "PROGRESS";
        case wxLOG_User: return "USER";
        case wxLOG_Max: return "MAX";
        default: return "???";
        }
    }

protected:
    void DoLogTextAtLevel(wxLogLevel level, const wxString& msg) override
    {
        // wxLOG_FatalError, wxLOG_Error, wxLOG_Warning, wxLOG_Message, wxLOG_Status
        // also to status bar, if frame yet instantiated
        if (level <= wxLOG_Status && wxGetApp().mainFrame != nullptr)
            wxGetApp().mainFrame->SetStatusText(msg);

        // Everything to textCtrl
        if (m_textCtrl != nullptr)
            m_textCtrl->AppendText(msg + "\n");

        // force user to commit
        if (level == wxLOG_Error || level == wxLOG_FatalError) {
            wxMessageBox(msg, levelText(level), wxICON_ERROR);
        }

        // logging to windows debug log
        messageOutputDebug.Printf("LOG %s: %s", levelText(level), msg);
        //||level == wxLOG_Debug || level == wxLOG_Trace)
        /*
        if (level == wxLOG_Info || level == wxLOG_Error)
            m_textCtrl->AppendText(msg + "\n");

        if (m_file.IsOpened())
            m_file.Write(msg + "\n");
        */
    }
private:
    wxTextCtrl* m_textCtrl;
    wxFFile     m_file;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(wxWindow* parent = nullptr) : wxFrame(parent, wxID_ANY, "Test")
    {
        wxTextCtrl* logCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
        wxLog::SetActiveTarget(new MyLogTarget(logCtrl, "log.txt"));

        wxLogDebug("Debug test");
        wxLogMessage("Message test");
        wxLogInfo("Info test");
        wxLogError("Error test");
    }
    ~MyFrame()
    {
        delete wxLog::SetActiveTarget(nullptr);
    }
};

#endif // __LOGGER_H__

