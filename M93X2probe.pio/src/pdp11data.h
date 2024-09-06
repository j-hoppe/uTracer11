/* pdp11data - data records to hold loadable PDP-11 binary code

        16bit Addresses: loads only into first 56k page
*/

#ifndef __PDP11DATA_H__
#define __PDP11DATA_H__

#include "utils.h" // uint16_t

// a single record: many words at ascending addresses after .startAddr
typedef struct {
    uint16_t startAddr;
    uint16_t wordCount;
    uint16_t words[];
} Pdp11DataRecord;

// many data records
typedef struct {
    uint16_t entryAddress; // program execution starts here
    unsigned recordCount;
    const Pdp11DataRecord *records[];
} Pdp11DataRecordList;

// another object registers a addr/data callback
class Pdp11DataUser {
  public:
    // processes a addr/data pair, result false if problem
    // function code specifies what todo
    virtual bool onPdp11DataIteration(int functioncode, uint16_t addr, uint16_t data);
};

// thin wrapper around some service functions
// can iterate over all data values and call a method
class Pdp11Data {
  private:
    Pdp11DataUser *calleeObj = nullptr;
    bool (Pdp11DataUser::*calleeMethod)(int, uint16_t, uint16_t) = nullptr;

  public:
    // access a member from PROGMEM
    uint16_t getWord(const uint16_t *progmemWordPtr) {
        return pgm_read_word(progmemWordPtr) ;
    }

    // get embedded data by file number
    // name of record specified by run of "pdp11code2c" converter
    const PROGMEM Pdp11DataRecordList *getRecordListByNumber(int index);
    // void recordsTestLog(const PROGMEM Pdp11DataRecordList *recordList);

    // Method to set the method pointer and object
    void setIterationCallback(Pdp11DataUser *user, bool (Pdp11DataUser::*method)(int, uint16_t, uint16_t)) {
        calleeObj = user;
        calleeMethod = method;
    }

    // run callback for all addr/data pairs, until callback returns false
    // result false: stop because of any call back false
    bool iterate(const PROGMEM Pdp11DataRecordList *recordList, int functionCode);
};

// Singletons
extern Pdp11Data thePdp11Data;

#endif __PDP11DATA_H__
