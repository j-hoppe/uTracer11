/* memorygridcontroller - visualize PDP11 memory content with a wxGrid

- grid is updatedby pushing a addr/data pair into it

- the grid shows in a row one address and 8 data cells
data cells are
 - empty = undefined (not yet queried from PDP-11
 - "NXM" = UNIBUS access error
 - data

 The grid only shows rows with at least on cell defined
 so on new data, additional rows are created

 - a single cell may be highlights, use d to indicate the last added value
    => the last UNIBUS transaction
    (red background)



*/

#include <wx/wx.h>
#include "memorygridcontroller.h"


void MemoryGridController::clear()
{
    markedAddress = MEMORY_ADDRESS_INVALID;
    rowStartAddresses.clear();
    grid->ClearGrid();
    // show empty?
}

bool MemoryGridController::isEmpty()
{
    return (rowStartAddresses.size() == 0);
}

// memory -> grid
// return row and offset in row. all data begining with 0
// result: false, if now row for addr in grid
bool MemoryGridController::getCellCoordFromAddr(uint32_t addr, int* rowIdx, int* colIdx)
{
    assert(addr >= addressFrom);
    assert(addr < addressTo);
    uint32_t rowStartAddress = (addr / (2 * wordsPerRow)) * 2 * wordsPerRow; // integer division truncs to 0

    // classic binary search
    // check whether a given value exists in the array or not
    auto it = std::find(rowStartAddresses.begin(), rowStartAddresses.end(), rowStartAddress);
    if (it == rowStartAddresses.end())
        return false; // not found
    *rowIdx = it - rowStartAddresses.begin(); // 
    assert(rowStartAddresses[*rowIdx] == rowStartAddress);
    /*
    auto it = std::lower_bound(rowStartAddresses.begin(), rowStartAddresses.end(), rowStartAddress);
    if (it == rowStartAddresses.end())
        return false; // not found
    *rowIdx = *it;
*/
// 01110->0, 01112->1, 01114->2, ..., 01120 -> 4
    *colIdx = (addr / 2) % wordsPerRow; // offset in row modulo
    return true;
}

// grid -> memory
uint32_t MemoryGridController::getAddrFromCellCoord(int rowIdx, int colIdx) {
    assert(colIdx < grid->GetNumberCols());
    assert(rowIdx < (int)rowStartAddresses.size());
    assert(rowIdx < grid->GetNumberRows());
    uint32_t rowStartAddress = rowStartAddresses[rowIdx];
    uint32_t addr = rowStartAddress + 2 * (unsigned)colIdx;
    assert(addr >= addressFrom);
    assert(addr < addressTo);
    return addr;
}

// decide which addresses to show in which grid cells
// rowStartAddresses sorted!
void MemoryGridController::buildRowStartAddresses() {
    // scan image for valid cells
    rowStartAddresses.clear();

    // scan rows
    for (uint32_t rowStartAddr = addressFrom; rowStartAddr < addressTo; rowStartAddr += 2 * wordsPerRow) {
        // any data in row
        bool anyData = false;
        for (uint32_t addrOffset = 0; addrOffset < 2 * wordsPerRow; addrOffset += 2)
            anyData |= memoryImage->get_cell(rowStartAddr + addrOffset)->valid;
        if (anyData)
            rowStartAddresses.push_back(rowStartAddr); // register row for display
    }
}

void MemoryGridController::setGridCell(int rowIdx, int colIdx, memory_cell_t* memcell) {
    if (!memcell->valid)
        grid->SetCellValue(rowIdx, colIdx, "");
    else if (memcell->nxm)
        grid->SetCellValue(rowIdx, colIdx, "NXM");
    else
        grid->SetCellValue(rowIdx, colIdx, wxString::Format("%06o", memcell->data));
}

// setup grid to show part of memoryimage
void MemoryGridController::rebuild()
{
    // build columns for offsets 0 .. wordsPerRow-1
    grid->DeleteCols(0, 999);
    grid->AppendCols(wordsPerRow);
    for (unsigned colIdx = 0; colIdx < wordsPerRow; colIdx++)
        // +00 +02 +04 + +06
        grid->SetColLabelValue(colIdx, wxString::Format("+%02o", 2 * colIdx));

    // build rows
    while (grid->GetNumberRows() > 0)
        grid->DeleteRows(0, 999);
    buildRowStartAddresses();
    if (rowStartAddresses.size() == 0) {
        // grid is total empty: one empty row without address label
        grid->AppendRows(1);
        grid->SetRowLabelValue(0, "empty");
    }
    else {
        for (unsigned rowIdx = 0; rowIdx < rowStartAddresses.size(); rowIdx++) {
            grid->AppendRows(1);
            grid->SetRowLabelValue(rowIdx, wxString::Format("%06o", rowStartAddresses[rowIdx]));
            for (unsigned colIdx = 0; colIdx < wordsPerRow; colIdx++) {
                uint32_t addr = getAddrFromCellCoord(rowIdx, colIdx);
                memory_cell_t* cell = memoryImage->get_cell(addr);
                setGridCell(rowIdx, colIdx, cell);
            }
        }
    }


}


void MemoryGridController::updateSingleAddress(uint32_t addr)
{
    int  rowIdx, colIdx;
    if (getCellCoordFromAddr(addr, &rowIdx, &colIdx)) {
        // show in grid cell. manages nxm and data
        memory_cell_t* memcell = memoryImage->get_cell(addr);
        setGridCell(rowIdx, colIdx, memcell);
    }
    else {
        // need new row, full re-display. new data must be inserted into image
        rebuild();
        // now row present!
        assert(getCellCoordFromAddr(addr, &rowIdx, &colIdx));
    }
}


// a single cell can be marked.
// INVALID_ADDR-> no cell marked
// on set, previous cell msut be unmarked
void MemoryGridController::setMarkedAddress(uint32_t addr)
{
    int rowIdx, colIdx;
    if (markedAddress != MEMORY_ADDRESS_INVALID) {
        // clear previous mark
        if (getCellCoordFromAddr(markedAddress, &rowIdx, &colIdx))
            grid->SetCellBackgroundColour(rowIdx, colIdx, grid->GetDefaultCellBackgroundColour());
    }
    if (addr != MEMORY_ADDRESS_INVALID) {
        // set new
        if (!getCellCoordFromAddr(addr, &rowIdx, &colIdx))
            markedAddress = MEMORY_ADDRESS_INVALID;
        else {
            grid->SetCellBackgroundColour(rowIdx, colIdx, markedAddressColor);
            markedAddress = addr;
            grid->MakeCellVisible(rowIdx, colIdx); // scroll to
        }
    }
    grid->Refresh();
}
