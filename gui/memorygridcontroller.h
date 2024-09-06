/* memorygridcontroller - visualize PDP11 memory conent with a wxGrid */

#ifndef __MEMORYGRIDCONTROLLER_H__
#define __MEMORYGRIDCONTROLLER_H__

#include <wx/grid.h>
#include <vector>
#include "memoryimage.h"

class MemoryGridController {
private:

    // grid row[i] starts with rowStartAddresses[i], indexes from 0
    // keep sorted
    std::vector<uint32_t> rowStartAddresses;

    // a single address cell is marked
    // todo: sync color with document page marker color
    wxColor markedAddressColor = wxColour("#fc8080"); // bright unsaturated red
    uint32_t markedAddress = MEMORY_ADDRESS_INVALID;

public:
    // get row and column of a address. false = not found
    bool getCellCoordFromAddr(uint32_t addr, int* rowIdx, int* colIdx);
    uint32_t getAddrFromCellCoord(int rowIdx, int colIdx);
    void buildRowStartAddresses();

    void setGridCell(int rowIdx, int colIdx, memory_cell_t* memcell);

    const unsigned wordsPerRow = 8;

    wxGrid* grid = nullptr;
    // shared with pdp11 and other memory grids
    memoryimage_c* memoryImage; // data base, needed for full rebuild
    uint32_t addressFrom; // displayed range from..to-1
    uint32_t addressTo;

    void init(wxGrid* _grid, memoryimage_c* _image, uint32_t _addressFrom, uint32_t _addressTo) {
        grid = _grid;
        memoryImage = _image;
        addressFrom = _addressFrom;
        addressTo = _addressTo;
    }

    // remove all rows
    void clear();

    bool isEmpty();

    // full new sync with memoryimage
    void rebuild();

    // add or update a data cell, value from image
    void updateSingleAddress(uint32_t addr);

    // a single cell can be marked.
    // INVALID_ADDR-> no cell marked
    void setMarkedAddress(uint32_t addr);

};


#endif // __MEMORYGRIDCONTROLLER_H__
