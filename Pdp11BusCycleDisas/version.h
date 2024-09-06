
#ifndef _VERSION_H_
#define _VERSION_H_


#define	VERSION_STR		"1.9.3"

/*

UnibusCycleDisas revision history:
----------------------------------
23.10.2019  V 1.9.3
                 - opcodes assigned to "instruction sets", 
                   actual CPU limited to valid opcodes.
11.10.2019  V 1.9.2
                  - enhanced output of PC relative addressing
29.9.2019   V 1.9.1
                  - optimized scan for optentail bus timeout trap
                  - snippet "code" vs "DMA" decision a bit independet from
                    parsed code length.
15.9.2019   V 1.9.0        
                     - change of disassembly algorithm:
                        cycle stream segmeneted in "snippets", which are disassembeld speculative
                        in some variants.
6.9.2019   V 1.8.1
                    - fixes clrb, mfpi etc, trap push/pop sequence
                    TODO: mfpi ?
21.8.2019   V 1.8.0
                    - adaptive exclusion of Non-CPU bus cycles
2.10.2018   V 1.7.0 
                    - added symbol tables
11.5.2018   V 1.6.3
                    - start handling PDP-11/05
6.8.2014	V 1.6.2
					- Bugfix TRAP instruction
					- Start implemention of CPU selection 
3.8.2014	V 1.6.1
					- Bugfix macro CYCLES_CYCLE_VALID 
25.7.2014   V 1.6.0
					- proper handling of known, but unimplemented floating point
					  instructions
14.7.2014	V 1.5.0
					- bug fixes: EMT <trap nr>
					- mode 7, counting of pc increments by dispalcment fetch
23.6.2014	V 1.4.0
					- check for UNIBUS contorl signals can be switched off
					  (This dll is also used for "Pdp11InstructionList" program)
25.5.2014	V 1.3.1
					- expanded trap names
7.5.2014	v 1.3.0
					- signal duration
6.5.2014	v 1.2.0
					- added "bus_timeout" columns

5.5.2014	v 1.1.1 - bugfix

5.5.2014	v 1.1.0 - parameter interface
					- bugfix: add, sub, bic, bis:
					  3 cycles: src read, dst read-modify-write

4.5.2014	v1.0.0	all instructions implemented,
					tested only with CPU basic instruction test CXCPAG
*/


#endif

