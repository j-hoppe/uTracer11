rem @echo off
rem opens cygwin terminal
setlocal enableextensions
set TERM=
rem cd /d "/cygdrive/%~dp0bin" && c:\cygwin64\bin\bash.exe --login -i

c:\cygwin64\bin\mintty.exe /bin/env CHERE_INVOKING=1 /bin/bash -l