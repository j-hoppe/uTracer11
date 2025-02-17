/* eefilecontroller - manages FILESYSTEM on M93X2 probe eeprom */

#ifndef __EEFILECONTROLLER_H__
#define __EEFILECONTROLLER_H__

#include <wx/grid.h>
#include <wx/filename.h>

class Pdp11Adapter; // include-circle with pdp11adapter.h

// link reafdile, writefile to message interface
class EeFilesystem: public EeFilesystemBase {
} ;

class EefileController {

private:
	EefileController() ;
	~EefileController() ;

// interface to remote file system
bool eeStructureValid ; // info, blockmap, filelists read?
void eeReadStructure() ;

public:
	wxString lastError ; // empty if last operation OK

	// interface to GUI
    void showDirecory(wxGrid *grid) ;
    void deleteFile(int fileNo) ;
    wxString uploadFile(int fileNo, wxFileName filename) ;
    wxString getSummary() ;

};


#endif // __EEFILECONTROLLER_H__
