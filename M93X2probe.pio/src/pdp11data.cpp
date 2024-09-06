/* pdp11data - format records to hold loadable PDP-11 binary code
        loads only into first 56l page
*/

#include "pdp11data.h"

#include "console.h" // LOG()

// include code tables
#include "pdp11data_basic.inc"
#include "pdp11data_m9312.inc"
#include "pdp11data_serial.inc"

Pdp11Data thePdp11Data;

// enumerate the embedded code tables
const PROGMEM Pdp11DataRecordList *Pdp11Data::getRecordListByNumber(int index) {
    switch (index) {
    case 1:
        return &m9312RecordList;
    case 2:
        return &basicRecordList;
    case 3:
        return &serialRecordList;
    }
    return nullptr;
}

#ifdef DEMO
/* define non-contiguous blocks */

static Pdp11DataRecord trapVector = {
    024, 2,     // PDP11 trap vector has 2 words:
    05000, 0340 // vector code entry address, PSW
};

// fake code block of 16 words
static Pdp11DataRecord fakeCode = {
    05000, 16, //
    00000, 00001, 00002, 00003, 00004, 00005, 00006, 00007,
    00010, 00011, 00012, 00013, 00014, 00015, 00016, 00017};

// link all records into one list
Pdp11DataRecordList pdp11DataRecords = {
    2,
    &trapVector,
    &fakeCode};
#endif

// to be implemented by iteration callee
bool Pdp11DataUser::onPdp11DataIteration(int functioncode, uint16_t addr, uint16_t data) {};

// iterate through records and data in PROGMEM, access only via pgm_read_word()
// call iteration callback bool method(functioncode, addr,data) for each addr/data pair
bool Pdp11Data::iterate(const PROGMEM Pdp11DataRecordList *recordList, int functionCode) {
    // recList and rec look like pointers, but can not be dereferenced
    uint16_t entryAddress = pgm_read_word(&(recordList->entryAddress));
    uint16_t recordCount = pgm_read_word(&(recordList->recordCount));
    for (unsigned recordIdx = 0; recordIdx < recordCount; recordIdx++) {
        auto rec = (const Pdp11DataRecord *)pgm_read_word(&(recordList->records[recordIdx]));
        // const Pdp11DataRecord *rec = (const Pdp11DataRecord *)pgm_read_word(&(recList->records[recordIdx]));
        uint16_t startAddr = pgm_read_word(&(rec->startAddr));
        uint16_t wordCount = pgm_read_word(&(rec->wordCount));
        for (unsigned wordIdx = 0, addr = startAddr; wordIdx < wordCount; wordIdx++, addr += 2) {
            uint16_t dataval = pgm_read_word(&(rec->words[wordIdx]));
            bool result = (calleeObj->*calleeMethod)(functionCode, addr, dataval);
            if (result == false)
                return false; // stop iteration if calle signals error
        }
    }
    return true;
}

#ifdef XXX
// iterate through records and data in PROGMEM, access only via pgm_read_word()
void Pdp11Data::recordsTestLog(const PROGMEM Pdp11DataRecordList *recordList) {
    // recList and rec look like pointers, but can not be dereferenced
    uint16_t entryAddress = pgm_read_word(&(recordList->entryAddress));
    uint16_t recordCount = pgm_read_word(&(recordList->recordCount));
    LOG("RecordList: entryAddr=%06o, recordCount=%06o\n", entryAddress, recordCount);
    for (unsigned recordIdx = 0; recordIdx < recordCount; recordIdx++) {
        auto rec = (const Pdp11DataRecord *)pgm_read_word(&(recordList->records[recordIdx]));
        // const Pdp11DataRecord *rec = (const Pdp11DataRecord *)pgm_read_word(&(recList->records[recordIdx]));
        uint16_t startAddr = pgm_read_word(&(rec->startAddr));
        uint16_t wordCount = pgm_read_word(&(rec->wordCount));
        LOG("  Record[%d]: wordCount=%u\n", recordIdx, wordCount);
        for (unsigned wordIdx = 0, addr = startAddr; wordIdx < wordCount; wordIdx++, addr += 2) {
            uint16_t dataval = pgm_read_word(&(rec->words[wordIdx]));
            if (wordIdx < 20) // limit output
                LOG("    word[%3u]: addr=%06o, data=%06o\n", wordIdx, addr, dataval);
        }
    }
}
#endif
