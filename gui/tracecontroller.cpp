/* tracecontroller - manages the ustep / UNIBUS / disasassembly form

Display in grid like:
  col 0     1       2       3       4       5         6                 7       8           9
 #    MPC   Cntrl   Addr    Data    Label   Opcode    Operands          Addr    data hex    Comment
 1    016
 2          DATI    003740  022767          cmp       #------,------    007E0   25F7        fetch opcode
 3    015   pupp=123,c=1
 4    123   c:1->0
 5          DATI    003742  104777          cmp       #104777,------    007E2   89FF        read data, address = pc
 6    234
 7    235
 8          DATI    003744  177752          cmp       #104777,------    007E4   FFEA        fetch displacement from PC
 9    402
10    404
11          DATI    003720  104622                                      007D0   8992        read data, address = 177752+pc
                                            cmp       #104777,3720


 Handling of disassembly:
 PDP-11 instructions execute multiple bus cycles:
 Example:   mov @#1234,@#3456
    1. fetch opcode   "mov" + operand fields
    2. DATI load address1 1234 from pc+2
    3. DATI value from address1
    4. DATI load address2 3456 from pc+4
    5. DATO value to address2
 When microstepping, cycles appear one-by-one,
 and the opcode display is updated multiple times
 Example: At fetch MPC=16 only the opcode "mov" with addressing modes is known.
 it is displayed in line #1 as "mov @#?????,@#?????"
 When the 1st address is read in line #5, the display changes to mov @#1234,@#?????"
 When the 2nd address is read, the display changes to mov @#1234,@#3456"


 The disassembler module receives a list of pdp1bus_cycle_t, for the DATI/DATOs generated.
 The disas doesn't handles truncated instructions well.
 So the cycle list padded with dummy cycles, so the disas has enough cycles to process.
These are cycles of type "LISTFILLUP", with an address of 0777733
So after fetch only the opcode is known, but the cycle list is
    1. fetch opcode   "mov" + operand fields
    2. FILLUP load address1 0777733 from pc+2
    3. FILLUP value from address1
    4. FILLUP load address2 0777733 from pc+4
    5. FILLUP value to address2
This is returned first as "mov @#777733,@#777733",
With an symbol-entry "777733 = ??????" we get the final "mov @#??????,@#??????"


 The list is cleared on "fetch" (line #1), and grows as more DATI/DATO are captured.

*/
#include <wx/clipbrd.h>
#include "logger.h"
#include "csvreader.h"
#include "tracecontroller.h"


// setup grid
void TraceController::init(wxGrid* _grid, Pdp11Adapter* _pdp11Adapter, wxFileName symbolFilePath)
{
    grid = _grid;
    pdp11Adapter = _pdp11Adapter;

    switch (pdp11Adapter->getType()) {
    case Pdp11Adapter::Type::pdp1134phys:
    case Pdp11Adapter::Type::pdp1134sim:
        disasPdp11Type = DISAS_CPU_34;
        break;
    case Pdp11Adapter::Type::pdp1140phys:
    case Pdp11Adapter::Type::pdp1140sim:
        disasPdp11Type = DISAS_CPU_40;
        break;
    default:
        wxLogFatalError("TraceController::init() unknown pdp11Type");
    }

    // read symbols
    disasReadSymbolFile(symbolFilePath);

    // Set CPU type for disassembler
    // PARAM_N_CPU = 4, see parameters.hpp
    disas_param_set(4, disasPdp11Type);
    clear();
    nextUnibusCycleIsOpcodeFetch = false;
}


void TraceController::clear() {
    // delete all grid rows
    grid->DeleteRows(0, grid->GetNumberRows());
    stateVarsActiveCellCoords.Set(9999999,999999);// invalid
    singleStepCount = 0 ;
    stateVarsGridVisibleVals.clear();
    stateVarsActiveCellVals.clear();
}


// Function to check if a row in the grid is empty
bool TraceController::isRowEmpty(int row) {
    int cols = grid->GetNumberCols();
    for (int col = 0; col < cols; ++col) {
        wxString value = grid->GetCellValue(row, col);
        if (!value.IsEmpty()) {
            return false; // Row is not empty
        }
    }
    return true; // Row is empty
}



// returns row index of new empty row
// make sure a single empty dummy row is located BELOW,
// as scrolling may partial hide the last row
int TraceController::newDataRow() {
    grid->AppendRows(1);
    int newRow = grid->GetNumberRows() - 1;
    if (newRow == 0 || !isRowEmpty(newRow - 1))
        // no emtpy predecessor: add a 2nd empty row
        grid->AppendRows(1);

    int newDataRow = grid->GetNumberRows() - 2; // this is the row before the dummy row
    assert(newDataRow >= 0);
    return newDataRow;
}


// the grid is dumped with col separatrors in CSV format
void TraceController::gridToClipboard() {
    // 1. format grid content
    const char *sep = ";" ;
    wxString text ;
    // 1.1 column headers
    for (int col=0; col < grid->GetNumberCols() ; col++) {
        text += col ? sep : "";
        text += grid->GetColLabelValue(col) ;
    }
    text += "\n" ;
    // 1.2 data rows
    for (int row = 0 ; row < grid->GetNumberRows() ; row++) {
        for (int col=0; col < grid->GetNumberCols() ; col++) {
            text += col ? sep : "";
            text += grid->GetCellValue(row, col) ;
        }
        text += "\n" ; // end of row
    }

    // 2. set clipboard
    if (wxTheClipboard->Open()) {
        wxTheClipboard->Clear(); // Clear clipboard before setting new data

        wxTheClipboard->SetData(new wxTextDataObject(text)); // Write text to clipboard
        // After this function has been called, the clipboard owns the data, so do not delete the data explicitly.

        wxTheClipboard->Close(); // Close the clipboard
    }
    else {
        wxLogError("Unable to open clipboard.");
    }
}


// produce the buscycle which is used to fillup incomplete instruction UNIBUS activity
pdp11bus_cycle_t TraceController::getFillupCycle() {
    pdp11bus_cycle_t fillupCycle;
    fillupCycle.bus_address_lsb_bit_count = 18;
    fillupCycle.bus_address.val = cycleListFillupAddress;
    fillupCycle.bus_address.iopage = false;
    fillupCycle.bus_timeout = 0;
    // 16bit address, data2addr makes it 18bit IOpage
    // symbol 777733 set to "?????"", display like "mov @????,..." instead of "mov @17733,..."
    fillupCycle.bus_data = cycleListFillupAddress & 0xffff; // 177733
    fillupCycle.bus_cycle = BUSCYCLE_FILLUP;
    fillupCycle.bus_cycle_physical = BUSCYCLE_FILLUP;
    fillupCycle.disas_call_level = 0;
    strcpy(fillupCycle.disas_sym_label, "");
    strcpy(fillupCycle.disas_opcode, "");
    strcpy(fillupCycle.disas_operands, "");
    strcpy(fillupCycle.disas_debug_info, "");
    strcpy(fillupCycle.disas_comment, "");
    return fillupCycle;
}



// prepare for begin of new opcode
// delete list of unibus cycles
void TraceController::startOfMacroInstruction() {
    disasBusCyclesCount = 0;
    disasBusCycles.clear();
    nextUnibusCycleIsOpcodeFetch = true;
    syncronizedWithMicroMachine = true;
    opcodeDisplayGridRow = -1; // invalid until opcode fetched

    // fill the whole list with dummy cycles for the disassembler
    pdp11bus_cycle_t fillupCycle = getFillupCycle();
    disasBusCycles.reserve(disasBusCyclesBufferSize);
    for (int i = 0; i < disasBusCyclesBufferSize; i++)
        disasBusCycles.push_back(fillupCycle);
}

// micro stepping terminated, umachine free running
// no further valid data
void TraceController::evalUClockRun() {
    // inhibit cycle disassembly when micro engine is free running,
    // or single stepping  did not yet passed a "opcode fetch"
    syncronizedWithMicroMachine = false;
    int row = newDataRow();
    // merge cols1..n: one big horizontal cell
    int colsToMerge = grid->GetNumberCols() - 1 ;
    grid->SetCellSize(row, 1, 1, colsToMerge) ;
    grid->SetCellValue(row, 0, "AUTO CLK");
    grid->SetCellValue(row, 1, "micro machine running full speed, no state data now");
    singleStepCount = 0 ; // first MPC after MAN CLK with full data
}

// start micro stepping, manual micro clock
// no UNIBUS/probe signals/state vars now sampled and consistent
void TraceController::evalUClockSingle() {
    // inhibit cycle disassembly when micro engine is single stepping
    // and did not yet passed a "opcode fetch"
    syncronizedWithMicroMachine = false;
    int row = newDataRow();
    // merge cols1..n: one big horizontal cell
    int colsToMerge = grid->GetNumberCols() - 1 ;
    grid->SetCellSize(row, 1, 1, colsToMerge) ;
    grid->SetCellValue(row, 0, "MAN CLK");
    grid->SetCellValue(row, 1, "micro machine stopped");
    singleStepCount = 0; // first MPC after MAN CLK with full data
}


// display a micro step
void TraceController::onResponseMpc(unsigned mpc) {
    if (mpc == pdp11Adapter->getMpcFetch()) {
        // resync disassembly, new opcode
        startOfMacroInstruction();
    }
    // col 0 MPC, 3 digits octal
    int row = newDataRow();
    // merge cols1..n: one big horizontal cell
    int colsToMerge = grid->GetNumberCols() - 1 ;
    grid->SetCellSize(row, 1, 1, colsToMerge) ;
    grid->SetCellValue(row, 0, wxString::Format("%03o", mpc));
    grid->SetCellValue(row, 1, "(updated on response)");
    stateVarsActiveCellCoords.Set(row,1); // "statevar=value" pairs in same row as MPC

    // last active statevar row is now not worked on any more
    // copy stuff visibile in last row to global visible state
    for (auto varval = stateVarsActiveCellVals.begin() ; varval != stateVarsActiveCellVals.end() ; ++varval)
        stateVarsGridVisibleVals[varval->first] = varval->second;
    stateVarsActiveCellVals.clear(); // all moved to grid

    singleStepCount++ ;

    //grid->MakeCellVisible(row, 0);
    grid->ScrollLines(9999);
}


// display full or changed stateVars in the row of last MPC
// 3 cases for each statevar:
// 1. stateVar not yet shown -> visibileStateVarVals[] not present -> display "varname=val"
// 2. stateVar shown earlier, but changed -> visibileStateVarVals[] != var.val -> "varname: old->new"
// 3. stateVar not changed -> visibileStateVarVals[] == var.val -> supress display


// 1. stateVar not yet shown in grid
// //       -> visibleGridStateVarVals[] not present -> display "varname=val"
// 2. stateVar shown earlier, but changed -> visibileStateVarVals[] != var.val -> "varname: old->new"
// 3. stateVar not changed -> visbileStateVarVals[] == var.val -> supress display


// can be called multiple times for the same row, as responses from multiple sources
// update the global statevar list.
// as current row is overwritten then,
void TraceController::displayStateVars() {
    if (stateVarsActiveCellCoords.GetRow() >= grid->GetNumberRows())
        return ; // grid in ill state
    std::string displayLine;
    std::string sep = "";
    if (singleStepCount == 0) // (re)start of singlestepping: force full display
        stateVarsGridVisibleVals.clear();
    for (unsigned i = 0; i < pdp11Adapter->stateVars.size(); i++) {
        Variable* stateVar = pdp11Adapter->stateVars.get(i);
        if (stateVar->isType(VariableType::trace)) {
            auto visibleStateVar = stateVarsGridVisibleVals.find(stateVar->name);
            // entry->first = name, entry->second = value
            if (visibleStateVar == stateVarsGridVisibleVals.end()) {
                // case 1
                displayLine += sep;
                displayLine += format_string("%s=%s", stateVar->name.c_str(), stateVar->valueText().c_str());
                sep = ",";
                // save result for grid update after MPC change
                stateVarsActiveCellVals[stateVar->name] = stateVar->value; // add new
            } else if (visibleStateVar->second != stateVar->value) {
                // case 2
                displayLine += sep;
                displayLine += format_string("%s:%s->%s",
                                             stateVar->name.c_str(),
                                             stateVar->valueText(visibleStateVar->second).c_str(),
                                             stateVar->valueText().c_str());
                sep = ",";
                // update current cell for grid update after MPC change
                stateVarsActiveCellVals[stateVar->name]= stateVar->value;
            }
            else
                // case 3
                ;
        }
    }
    grid->SetCellValue(stateVarsActiveCellCoords, displayLine);
}


// display DATI/DATO
// feed-in only cycles generated by CPU
// gather in disasBusCycles, also disassemble
// only called if "manClkEnable".
void TraceController::onResponseUnibusCycle(ResponseUnibusCycle* unibusCycle) {
    // Ignore UNIBUS cycles until "StartOfMacroInstruction()" is called, to sync with code fetch.
    wxString cycleText = unibusCycle->cycleText[unibusCycle->c1c0] ;
    if (!syncronizedWithMicroMachine) {
		wxLogWarning("Ignoring ResponseUnibusCycle %s addr=%06o", cycleText.mb_str(), unibusCycle->addr );
        return;
	}
    assert(disasBusCyclesCount < disasBusCyclesBufferSize);
    // 1. update cycle in disassembler list
    assert(disasBusCycles.capacity() > 0); // ???
    pdp11bus_cycle_t* disasCycle = &disasBusCycles[disasBusCyclesCount];
    disasCycle->bus_address_lsb_bit_count = 18;
    disasCycle->bus_address.val = unibusCycle->addr;
    disasCycle->bus_address.iopage = unibusCycle->addr >= 0760000; // last 8k page?
    disasCycle->bus_timeout = unibusCycle->nxm ? 1 : 0;
    disasCycle->bus_data = unibusCycle->data;
    switch (unibusCycle->c1c0) {
    case 0:
        disasCycle->bus_cycle = BUSCYCLE_DATI;
        break;
    case 1:
        disasCycle->bus_cycle = BUSCYCLE_DATIP;
        break;
    case 2:
        disasCycle->bus_cycle = BUSCYCLE_DATO;
        break;
    case 3:
        disasCycle->bus_cycle = BUSCYCLE_DATOB;
        break;
    default:
        wxLogFatalError("TraceController::evalResponseUnibusCycle() invalid bus cycle");
    }
    disasBusCyclesCount++;

    // fillup the list with dummy cycles, so the disassemble always has enough cycles to process
    // must be erased again, as the previous call to disas_pdp11buscycles() left results in the fillups
    pdp11bus_cycle_t fillupCycle = getFillupCycle();
    for (unsigned i = disasBusCyclesCount; i < disasBusCycles.size(); i++)
        disasBusCycles[i] = fillupCycle;

    // 2. disassemble
    // disas gets the whole list with fillup cycles and produces additional trash instructions
    // only the first instruction starting at the fetched opcode is valid
    // len = NOT disasBusCyclesBufferSize
    disas_pdp11buscycles(disasBusCycles.data(), disasBusCycles.size(),
                         disasSymbols.data(), disasSymbols.size());

    // 3. display
    int row = newDataRow();
    if (nextUnibusCycleIsOpcodeFetch) {
        opcodeDisplayGridRow = row; // this is the row where disas shows the full instruction
        // updated every following cycle
        nextUnibusCycleIsOpcodeFetch = false;
    }

    // 1) add current UNIBUS cycle to grid
    // col 1 # ctrl
    grid->SetCellValue(row, 1, cycleText);
    // col 2 # addr
    grid->SetCellValue(row, 2, wxString::Format("%06o", unibusCycle->addr));
    // col 3 # data
    if (disasCycle->bus_timeout)
        grid->SetCellValue(row, 3, "NXM");
    else
        grid->SetCellValue(row, 3, wxString::Format("%06o", (unsigned)unibusCycle->data));
    // col 4 # symbol label
    grid->SetCellValue(row, 4, disasCycle->disas_sym_label);
    // col 5 # disassembled opcode
    grid->SetCellValue(row, 5, disasCycle->disas_opcode);
    // col 6 # disassembled operands
    grid->SetCellValue(row, 6, disasCycle->disas_operands);
    // col 7 # hex address
    grid->SetCellValue(row, 7, wxString::Format("%05X", disasCycle->bus_address.val));
    // col 8 # hex data
    if (!disasCycle->bus_timeout)
        grid->SetCellValue(row, 8, wxString::Format("%04X", disasCycle->bus_data));
    // col 9 # comments on operands
    grid->SetCellValue(row, 9, disasCycle->disas_comment);

    // 2) update the "living" full instruction in display row of first fetch
    // example: "mov @#777733,@#777733" -> "mov @#001234,@#777733" -> "mov @#001234,@#3456"
    assert(opcodeDisplayGridRow >= 0); // set on opcode fetch
    auto opcodeDisasCycle = &disasBusCycles[0];
    grid->SetCellValue(opcodeDisplayGridRow, 6, opcodeDisasCycle->disas_operands);
    grid->SetCellValue(opcodeDisplayGridRow, 9, opcodeDisasCycle->disas_comment);

    //grid->MakeCellVisible(row, 0);
    grid->ScrollLines(9999);


}

/* Load symbols from csv
 Format like:

#
# addr   type        group        symbol      info
0777500; DEVICE_REG; "Cassette"; "TA11.TACS"; "command & status"
0777502; DEVICE_REG; "Cassette"; "TA11.TADB"; "data buffer"

 all the "disas" stuff is retro C-C++ ish     :-/
*/
unsigned TraceController::disasAddrtypeFromText(std::string txt) {
    const char* s = txt.c_str();
    if (!strcasecmp(s, "SYMBOL"))
        return ADDRTYPE_SYMBOL;
    else if (!strcasecmp(s, "ASSIGNED"))
        return ADDRTYPE_ASSIGNED;
    else if (!strcasecmp(s, "VECTOR_PC"))
        return ADDRTYPE_VECTOR_PC;
    else if (!strcasecmp(s, "VECTOR_PSW"))
        return ADDRTYPE_VECTOR_PSW;
    else if (!strcasecmp(s, "DEVICE_REG"))
        return ADDRTYPE_DEVICE_REG;
    else throw std::invalid_argument("addrtypeFromText(): invalid string = " + txt);
    return 0;
}


void TraceController::disasReadSymbolFile(wxFileName fullpath)
{
    pdp11_symbol_t sym;

    disasSymbols.clear();
    if (!fullpath.IsOk())
        return; // no name given

    // open and read
    // comment, separator, quotes
    CSVFile csvFile('#', ';', '"', fullpath.GetFullPath().ToStdString());

    auto disasSymbolsCount = csvFile.getRowCount();
    disasSymbols.reserve(disasSymbolsCount + 10); // I add some constants later
    for (unsigned rowIdx = 0; rowIdx < csvFile.getRowCount(); rowIdx++) {
        auto fields = csvFile.rows[rowIdx];
        if (fields.size() != 5)
            wxLogFatalError("Symbol line must have 5 fields separated by ';' in line %s", std::to_string(rowIdx));

        const char* cvalue;
        cvalue = fields[0].c_str();
        sym.addrval.val = (unsigned)strtol(cvalue, nullptr, 8);
        sym.addrval.iopage = (sym.addrval.val >= 0xE000);
        sym.addrtype = disasAddrtypeFromText(fields[1]);
        sym.attribute = 0;
        strcpy(sym.group, fields[2].c_str());
        strcpy(sym.text, fields[3].c_str());
        strcpy(sym.info, fields[4].c_str());
        disasSymbols.push_back(sym);
    }

    // Incomplete instructions use "fillup" dummy DATIs
    // the address there must be shown as "??????"
    sym.addrval.val = cycleListFillupAddress;
    sym.addrval.iopage = (sym.addrval.val >= 0xE000);
    sym.addrtype = ADDRTYPE_SYMBOL;
    sym.attribute = 0;
    strcpy(sym.group, "dummy");
    strcpy(sym.text, "??????");
    strcpy(sym.info, "operand incomplete");
    disasSymbols.push_back(sym);
}
