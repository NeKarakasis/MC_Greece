 @echo off
REM ------------------------------------------------------------------
REM Prohibited editing part
REM ------------------------------------------------------------------
set RLINK=%1
set ABS=%2.abs
set TEMP=temp.mot
REM ------------------------------------------------------------------
REM User editable part
REM ------------------------------------------------------------------
REM Download module name

set OUTMOT=data.mot
del /Q %OUTMOT%
REM Set Endian (little or big)
set ENDIAN=little
REM Generate CRC checksum
REM GenCRC romStart romEnd crcStart crcEnd
call:GenCRC FFFF0100 FFFF10FF FFFFFF10 FFFFFF11
call:GenCRC FFFF1100 FFFF20FF FFFFFF12 FFFFFF13
call:GenCRC FFFF2100 FFFF30FF FFFFFF14 FFFFFF15
call:GenCRC FFFF3100 FFFF40FF FFFFFF16 FFFFFF17
REM ------------------------------------------------------------------
REM Prohibited editing part
REM ------------------------------------------------------------------
REM Merge
copy /Y %OUTMOT% %TEMP%
%RLINK% %TEMP% -form=stype -output=%OUTMOT%
del /Q %TEMP%
exit
REM Generate CRC checksum (CRC-16)
:GenCRC
 %RLINK% %ABS% -crc=%3=%1-%2/16:%ENDIAN% -form=stype -output=%TEMP%=%3-%4 > nul 2>&1
REM %RLINK% %ABS% -crc=%3=%1-%2/16:%ENDIAN% -form=stype -output=%TEMP%=%3-%4
type %TEMP% >> %OUTMOT%
exit /b