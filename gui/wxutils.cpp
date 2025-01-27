/* wxutils - helpers for wxWidgets 
 */


#include <wx/file.h>
#include <wx/textfile.h>
#include <wx/log.h>

#include "wxutils.h"


// load a multiline wxString from a text file

wxString wxLoadTextFromFile(const wxString& filename) {
    wxString content;

    if (!wxFileExists(filename)) {
        wxLogError("File does not exist: %s", filename);
        return content;
    }

    wxTextFile file;
    if (file.Open(filename)) {
        for (size_t i = 0; i < file.GetLineCount(); ++i) {
            content += file.GetLine(i) + "\n"; // Append each line with a newline
        }
        file.Close();
    } else {
        wxLogError("Failed to open the file: %s", filename);
    }

    return content;
}


// save a multiline wxString to a file
void wxSaveTextToFile(const wxString& filename, const wxString& content) {
    wxFile file;

    // Open the file in write mode, create if it doesn't exist
    if (file.Open(filename, wxFile::write) || file.Create(filename)) {
        wxString encodedContent = content;
#ifdef __WINDOWS__
        // Convert line endings to CRLF for Windows
        encodedContent.Replace("\n", "\r\n");
#endif
        file.Write(encodedContent);
        file.Close();
        wxLogMessage("File saved successfully.");
    } else {
        wxLogError("Failed to open or create the file.");
    }
}


