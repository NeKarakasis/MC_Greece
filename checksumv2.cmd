@echo off
echo ------------------------------------------------------------
echo >>> ENTERING checksum.cmd
echo ------------------------------------------------------------
pause

REM ------------------------------------------------------------
REM Handle quoted parameters safely (strip outer quotes)
REM ------------------------------------------------------------
set "RLINK=%~1"
set "ABS=%~2.abs"

echo RLINK executable: %RLINK%
echo ABS file path: %ABS%
pause

REM ------------------------------------------------------------
REM Clean up previous output files
REM ------------------------------------------------------------
echo Deleting old MOT files...
del /Q data.mot
del /Q temp*.mot
pause

REM ------------------------------------------------------------
REM User configuration
REM ------------------------------------------------------------
set OUTMOT=data.mot
set ENDIAN=little

echo Generating CRC blocks...
call :GenCRC FFFF0100 FFFF10FF FFFFFF10 FFFFFF11 temp_crc1.mot
call :GenCRC FFFF1100 FFFF20FF FFFFFF12 FFFFFF13 temp_crc2.mot
call :GenCRC FFFF2100 FFFF30FF FFFFFF14 FFFFFF15 temp_crc3.mot
call :GenCRC FFFF3100 FFFF40FF FFFFFF16 FFFFFF17 temp_crc4.mot
pause

REM ------------------------------------------------------------
REM Merge app and CRC blocks
REM ------------------------------------------------------------
echo Generating full MOT from ABS...
"%RLINK%" "%ABS%" -form=stype -output=main_app.mot
if errorlevel 1 (
    echo ERROR: Failed to generate main_app.mot
    pause
    exit /b
)

echo Copying app + appending CRC blocks into final data.mot...
copy /Y main_app.mot %OUTMOT%
type temp_crc1.mot >> %OUTMOT%
type temp_crc2.mot >> %OUTMOT%
type temp_crc3.mot >> %OUTMOT%
type temp_crc4.mot >> %OUTMOT%
pause

echo Final MOT file created: %OUTMOT%
echo >>> EXITING checksum.cmd
pause
exit /b

REM ------------------------------------------------------------
REM Generate CRC checksum block (romStart, romEnd, crcStart, crcEnd, outputFile)
REM ------------------------------------------------------------
:GenCRC
echo Generating CRC: ROM %1-%2 to CRC %3-%4 in %5
"%RLINK%" "%ABS%" -crc=%3=%1-%2/16:%ENDIAN% -form=stype -output=%5=%3-%4
if errorlevel 1 (
    echo ERROR during CRC generation for %5
    pause
    exit /b
)
echo CRC block %5 generated.
pause
exit /b
