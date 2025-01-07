/* tracecontroller - manages the ustep/UNIBUS/disassembly form and disassembly
*/
#ifndef __TRACECONTROLLER_H__
#define __TRACECONTROLLER_H__

#include <wx/grid.h>
#include <wx/filename.h>
#include <vector>
#include "messages.h"
//#include "pdp11adapter.h"
#include "../Pdp11BusCycleDisas/libmain.h"

class Pdp11Adapter; // include-circle with pdp11adapter.h

class TraceController {

private:
    // UNIBUS cycles generated in current opcode
    std::vector<pdp11bus_cycle_t> disasBusCycles;
    static const int disasBusCyclesBufferSize = 10;
    int disasBusCyclesCount = 0; // # of valid cycles in current instruction
    //    pdp11bus_cycle_t disasBusCycles[disasBusCyclesBufferSize];

    unsigned singleStepCount ; // > 2: display only changed values
    // cell where incomung statevars repsones are to be displayed
    wxGridCellCoords stateVarDisplayGridCoords ;


    static const unsigned cycleListFillupAddress = 0777733; // unused PDP-11 address to mark dummy cycles
    void startOfMacroInstruction();
    bool nextUnibusCycleIsOpcodeFetch;

    int opcodeDisplayGridRow; // grid row where the updating full instruction is show

    // symbols
    std::vector<pdp11_symbol_t> disasSymbols;
    unsigned disasAddrtypeFromText(std::string s);

    pdp11bus_cycle_t getFillupCycle();

	std::string stateVarsFullAsText() ;
		std::string stateVarsChangedAsText();

public:
    Pdp11Adapter* pdp11Adapter;
    wxGrid* grid;
    unsigned disasPdp11Type; // type for cycle disassembler

    void init(wxGrid* _grid, Pdp11Adapter* _pdp11Adapter, wxFileName symbolFilePath);
    void clear();
    bool isRowEmpty(int row) ;
    int newDataRow();

    void gridToClipboard() ;

    // false if micro engine not in "Manual Clock", and
    // not set opcode fetch detected on switch to Manual CLock.
    bool syncronizedWithMicroMachine ;
    void evalUClockRun() ;
    void evalUClockSingle() ;

    void evalUStep(unsigned mpc);
    void displayStateVars() ;
    void evalUnibusCycle(ResponseUnibusCycle* unibusCycle);

    void disasReadSymbolFile(wxFileName fullpath);
};


#endif // __TRACECONTROLLER_H__
