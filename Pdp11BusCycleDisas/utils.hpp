
#ifndef _UTILS_H_
#define _UTILS_H_

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Compatibility Linux-Visual Studio
#ifdef _MSC_VER
#include <windows.h> // UNREFERENCED_PARAMETER
#ifndef strcasecmp
#define strcasecmp _stricmp	// grmbl
#endif
#else
#include <stdio.h>
// no strcpy_s() under gcc
#define strcpy_s(dest,dest_size,src) strcpy((dest),(src))
#define strcat_s(dest,dest_size,src) strcat((dest),(src))
#define sprintf_s(buffer, sizeofbuffer, ...) sprintf((buffer), __VA_ARGS__)
#define UNREFERENCED_PARAMETER(p) (void)(p)
#endif

void dbg_break(bool condition);

void dbg_msg(const char *fmt, ...);

class CycleList;
struct pdp11bus_cycle_struct;

void dbg_instruction_opcode(CycleList *disas_cycles, int opcode_idx);

#endif
