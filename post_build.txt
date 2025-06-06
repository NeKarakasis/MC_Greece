@echo off
echo ------------------------------------------------------------
echo STARTING CHECKSUM PROCESS
echo ------------------------------------------------------------

REM ------------------------------------------------------------
REM Paths: Set these according to your system
REM ------------------------------------------------------------
echo Setting up paths...
set RLINK="C:\Program Files (x86)\Renesas\RX\3_7_0\bin\rlink.exe"
set ABS_BASE="C:\Git Hub\RX13T_ESB_SPM_LESS_FOC_E2S_V120\HardwareDebug\RX13T_ESB_SPM_LESS_FOC_E2S_V120"
echo RLINK path: %RLINK%
echo ABS path base: %ABS_BASE%
pause

REM ------------------------------------------------------------
REM Call the checksum generation script
REM ------------------------------------------------------------
echo Calling checksum.cmd with provided paths...
pause
call "C:\Git Hub\RX13T_ESB_SPM_LESS_FOC_E2S_V120\checksum.cmd" %RLINK% %ABS_BASE%
echo Returned from checksum.cmd
pause
REM ------------------------------------------------------------
REM Done
REM ------------------------------------------------------------
echo CHECKSUM PROCESS COMPLETE.
echo Output should be: data.mot and temp_crcX.mot files.
pause
