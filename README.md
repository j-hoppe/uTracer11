# uTracer11

„uTracer11“ (read: "micro-program-tracer for PDP-11") is a signal probe that is plugged into older UNIBUS PDP-11s.

The CPU state is visualized on a host PC via a graphical user interface (GUI)

It is primarily used for micro-stepping the CPU, as a function demo, repair tool, or for entertainment.

Directory structure:
common - C++ sources needed for M93X2 Arduino and GUI ... message protocol
CSerialPort - C++ sources for RS232 interface Linux/Windows
gui - main user interface, WxWidgets project Linux/Windows
KM11.pcb - KiCad project for KM11 adapter probe
M93X2probe.pcb - KiCad project for M93X2 probe PCB
M93X2probe.pio - M93X2 probe Arduino firmware, PlatformIO
M93X2probe.pdp11data - PDP-11 code to be embedded into M93X2 BOOT logic
Pdp11BusCycleDisas - source branch of UNIBUS traffic disassmebler for different PDP-11 CPUs
pdp1134sim - case study for a micro machine based simulator, example is a 11/34 simple program loop
resources - helper data files, scanned jpegs

