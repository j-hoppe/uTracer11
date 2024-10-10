/* tracecontroller - manages the upstep/UNIBUS /disasassembly form and disassembly

Display in grid like:
  col 0     1       2       3       4       5         6                 7       8           9
 #    MPC   Cntrl   Addr    Data    Label   Opcode    Operands          Addr    data hex    Comment
 1    016
 2          DATI    003740  022767          cmp       #------,------    007E0   25F7        fetch opcode
 3    015
 4    123
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
#include "logger.h"
#include "csvreader.h"
#include "tracecontroller.h"


// setup grid
void TraceController::init(wxGrid* _grid, Pdp11Adapter* pdp11Adapter, wxFileName symbolFilePath)
{
    grid = _grid;
    mpcFetch = pdp11Adapter->getMpcFetch();

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
}

// returns row index of new empty row
int TraceController::newRow() {
    grid->AppendRows(1);
    auto row = grid->GetNumberRows() - 1;
    return row;
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



// display a micro step
void TraceController::evalUStep(unsigned mpc) {
    if (mpc == mpcFetch) {
        // resync disassembly, new opcode
        startOfMacroInstruction();
    }
    // col 0 MPC, 3 digits octal
    int row = newRow();
    grid->SetCellValue(row, 0, wxString::Format("%03o", mpc));

    //grid->MakeCellVisible(row, 0);
    grid->ScrollLines(9999);

}

// display DATI/DATO
// feed-in only cycles generated by CPU
// gather in disasBusCycles, also disassemble
// only called if "manClkEnable". 
void TraceController::evalUnibusCycle(ResponseUnibusCycle* unibusCycle) {
    // Ignore UNIBUS cycles until "StartOfMacroInstruction()" is called, to sync with code fetch.
    if (!syncronizedWithMicroMachine)
        return;
    assert(disasBusCyclesCount < disasBusCyclesBufferSize);
    // 1. update cycle in disassembler list
    assert(disasBusCycles.capacity() > 0); // ???
    pdp11bus_cycle_t* disasCycle = &disasBusCycles[disasBusCyclesCount];
    disasCycle->bus_address_lsb_bit_count = 18;
    disasCycle->bus_address.val = unibusCycle->addr;
    disasCycle->bus_address.iopage = unibusCycle->addr >= 0760000; // last 8k page?
    disasCycle->bus_timeout = unibusCycle->nxm ? 1 : 0;
    disasCycle->bus_data = unibusCycle->data;
    wxString cycleText = "";
    switch (unibusCycle->c1c0) {
    case 0: disasCycle->bus_cycle = BUSCYCLE_DATI; cycleText = "DATI"; break;
    case 1: disasCycle->bus_cycle = BUSCYCLE_DATIP; cycleText = "DATIP"; break;
    case 2: disasCycle->bus_cycle = BUSCYCLE_DATO; cycleText = "DATO"; break;
    case 3: disasCycle->bus_cycle = BUSCYCLE_DATOB; cycleText = "DATOB"; break;
    default:
        wxLogFatalError("TraceController::evalUnibusCycle() invalid bus cycle");
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
    int row = newRow();
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

