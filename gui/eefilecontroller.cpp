/* eefilecontroller - manages FILESYSTEM on M93X2 probe eeprom
   Interfaces both messge interface and wxWidgets GUI

   Managing the Eeprom files is completely independent of PDP-11 
   functionality.

   GUI: show wxGrid,
   with one selected row for active file,
   then "action" buttons for
   - delete
   - upload (with file select dialog then)
    may fail if selected file too big
    Also "state" label, with "blocks free" display
*/

#include <wx/wx.h>
#include "eefilecontroller.h"

	EefileController::EefileController() {
	eeStructureValid = false ;
		lastError.Clear() ;
		}

	
	~EefileController::EefileController() {
		}


 // Eefilesystem: read info, blockmap, filelist
	void EefileController::eeReadStructure() {
	eeStructureValid = true ;
	lastError.Clear() ;
		}


	// GUI: fill grid with file list
    void EefileController::showDirecory(wxGrid *grid) {
    	}
	

	// GUI: delete file from filesystem
    void EefileController::deleteFile(int fileNo) {
    	}


  // GUI: load one file up to the eeprom filesystem
	// result: error ("file system full")
    wxString EefileController::uploadFile(int fileNo, wxFileName filename) {
    	}

 // GUI: statistics for file system, to be dispalyed in status line
    wxString EefileController::getSummary() {
    	}
	

