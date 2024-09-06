rem Create data records from PDP11-Software for Arduino on M93X2probe.
set EXE=..\pdp11code2c\Release\pdp11code2c.exe
set DIR=..\M93X2probe.pio\src


%EXE% -ml serial.lst -cs %DIR%\pdp11data_serial.inc -cid serialRecordList --entryaddress start
%EXE% -ml odt07.lst -cs %DIR%\pdp11data_m9312.inc -cid m9312RecordList --entryaddress DIAG
%EXE% -pt DEC-11-AJPB-PB.ptap -cs %DIR%\pdp11data_basic.inc -cid basicRecordList
