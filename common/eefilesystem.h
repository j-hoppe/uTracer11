/* eefilesystem - layout of a simple file system in 1MBit/128K eeprom


Targets:
- an eeprom is byte-addressable, no need to align every struct
  on a block boundary
- Arduino must only be able get word stream of file <fileno>
    to DEPOSIT into memory
- GUI accesses EEPROM via low level read/write and implements
    file system structure.
- fix allocatable file size of 512 byte, == "block"
   (see 24AAxxx data sheets)

  Layout:
  1. info record with filesystem properties
  2. "block map", assigns each block a file number
  	a block address is byte -> files can use ~250 datablocks
  	Arduino traverses theis to get stream of a file
  3. list of directory entries, 0 terminated

  4. Whole eeprom is addressabe as array of data blocks.
    structs 1-3 use space at eeprom begin, not aligned to blocks,
    but occupying	some of the blocks
   Remaining space devided into datablocks, contain file data
*/

#ifndef __EEFILESYSTEM_H__
#define __EEFILESYSTEM_H__

#include <stdint.h>

// abstract base class
// read/write EEProm cells depends on platform
// I2C on Arduino, messages on PC host
// Also reduced functioality on Arduino:
// on read file stream possible
class EeFilesystemBase {

typedef uint8_t DataBlock [512] ;

// for each datablock list <fileno> where block belongs to
// 0 = not used, or used for non file data
// filecontent given by ascending stream of blocks
typedef uint8_t BlockMap[256] ;

// a single directory entry
// - end of directoy is marked with name="\0"
// - erased files have a type == 0
typedef struct {
    char name[8] ;
    char ext[3] ;
    uint8_t d,m,y ; // file date
    uint32_t size ; // file size in bytes
    uint8_t type ; // type = 0
    uint8_t flags ;
} FileDescriptor;


// Layout parameters
// must be global consts, to be used in fix Layout [] struct

// bytecount used for filesystem
// (for 24aa1025: max 128K bytes  = 256 blocks per 512 bytes
static const uint32_t 	filesystemSize = 256*512;

// byte offset to block map, behind Info
// should be sizeof(Info), but technically not possible in struct Info itself
static const  uint32_t blockMapOffset = 64 ;

// max number of files and file descriptors (only 16 adressable by dip switch)
static const uint32_t	numberFiles = 64 ; // number of files (#0 not used)
// offset to "directory" = list of file descriptors, behind BlockMap
static const uint32_t	fileDescriptorsOffset = blockMapOffset + sizeof(BlockMap) ;

// position of datablocks, behind "directory" 
static const uint32_t	dataOffset = fileDescriptorsOffset + numberFiles * sizeof(FileDescriptor) ;
// remaining space available for data blocks
static const uint32_t numberDatablocks = (filesystemSize - dataOffset) / sizeof(DataBlock) ;

// record to hold filesystem metadata in EEPROM, struct < one block
typedef struct Info {
    uint8_t	versionMajor = 1; // version of filesystem
    uint8_t	versionMinor = 0;

    uint32_t _dataBlockSize = sizeof(DataBlock) ;
    // bytecount used for filesystem
    // (for 24aa1025: max 128K bytes  = 256 blocks per 512 bytes
    uint32_t _filesystemSize = filesystemSize ;

    // byte offset to block map, behind Info
    // should be sizeof(Info), but technically not possible in struct Info itself
    uint32_t _blockMapOffset = blockMapOffset ;

    // max number of files and file descritpors (only 16 adressable by dip switch)
    uint32_t _numberFiles = numberFiles ; // number of files (#0 not used)
    // offset to list of file descriptors, behind BlockMap
    uint32_t _fileDescriptorsOffset = fileDescriptorsOffset ;

    uint32_t _dataOffset = dataOffset ;

    // remaining space available for data blocks
    uint32_t _numberDatablocks = numberDatablocks ;
} Info ;


// full image of Eeprom content.
// not availabe on Arduino, data structs too large
#if !defined(PLATFORM_ARDUINO)
typedef struct {
    Info	info ; // first the info block,
    BlockMap blockMap ; // the assignment of blocks to file
    FileDescriptor filesDescriptors[numberFiles] ;
    DataBlock dataBlocks[numberDatablocks] ; // max index: Info::numberDatablocks-1
} Layout ;
#endif

// read/write actual eeprom cells
// implementation: direct I2C on aArduino,  messages on PC host
virtual void eeRead(uint32_t offset, uint32_t size, uint8_t *data) = 0 ;
	virtual void eeWrite(uint32_t offset, uint32_t size, uint8_t *data) = 0 ;


// read a given file byte-wise, with read cache to eeread()
// usage:
//	FileReader fr ;
//	fr.open(fileNo) ;
//	while (!fr.eof()) {
//     uint8_t b = fr.read() ;
//		... process ...
//  }
// does not use 
class FileReader {
private:
		static const unsigned cacheSize=16 ; // small meory foot print on Arduino
		uint8_t	cache[cacheSize] ;
		unsigned cacheByteReadIdx ; // next pos to read from cachce

		uint8_t	fileNo ; 
		uint8_t blockMapIdx ; // cache loaded from blckMap[blockmapidx]
		uint32_t	cacheBlockoffset ; // cache contains block[offset..offset+cacheSize-1]
public:
	open(uint8_t _fileno) ;
	uint8_t get() ;
	bool eof() ;
};

};

// abstract base class:
// access to dpeends on hardware platform

/*
// full layout in Eeprom
class Eefilesystem {
    // total size, blocksize, max number of files
    // to Info
    void initLayout(128K, 512, 64) ;

    struct Info info ; // also always at position 0 in eeprom
    void loadInfo() ;
    void writeInfo() ; // info struct to eeprom

    uint32_t offsetBlockMap() ; // byte position in eeprom
    uint32_t offsetFiledescriptr(int i) ;


    Info	info ; // first the info block,
    BlockMap blockMap ; // the assignment of blocks to file
    FileDescriptor filesDescriptors[Info::numberFiles] ;
    DataBlock dataBlocks[] ; // max index: Info::numberDatablocks-1
} Layout ;
  */



#endif // __EEFILESYSTEM_H__
