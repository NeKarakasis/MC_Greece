@echo off

REM =========================
REM CONFIGURATION
REM =========================
set PYTHON=C:\Users\a5143778\AppData\Local\Programs\Python\Python312\python.exe

REM =========================
REM INPUT FROM IAR
REM =========================
set ELF_FILE=%1
set OUTPUT_DIR=%~dp1
set PROJECT_NAME=%~n1

REM =========================
REM PATHS
REM =========================
set PROJECT_ROOT=%OUTPUT_DIR%..\..
set SCRIPT=%PROJECT_ROOT%\src\ultimate_parser.py

set OUTPUT_TXT=%OUTPUT_DIR%%PROJECT_NAME%.txt
set OUTPUT_MAP=%OUTPUT_DIR%%PROJECT_NAME%.map
set DEST_MAP=%OUTPUT_DIR%..\..\Src\%PROJECT_NAME%_conv.map

echo [INFO] =========================
echo [INFO] Running ELF parser
echo [INFO] ELF: %ELF_FILE%
echo [INFO] Project: %PROJECT_NAME%
echo [INFO] =========================

REM =========================
REM CLEAN OLD FILES
REM =========================
echo [INFO] Cleaning old files...

if exist "%OUTPUT_TXT%" (
    del "%OUTPUT_TXT%"
    echo [INFO] Deleted old TXT
)

if exist "%OUTPUT_MAP%" (
    del "%OUTPUT_MAP%"
    echo [INFO] Deleted old MAP (Debug\Exe)
)

if exist "%DEST_MAP%" (
    del "%DEST_MAP%"
    echo [INFO] Deleted old MAP (Src)
)

REM =========================
REM RUN PYTHON SCRIPT
REM =========================
%PYTHON% "%SCRIPT%" "%ELF_FILE%" "%OUTPUT_TXT%"

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Python script failed!
    exit /b 1
)

REM =========================
REM RENAME TXT → MAP
REM =========================
move /Y "%OUTPUT_TXT%" "%OUTPUT_MAP%" >nul

echo [INFO] Output generated:
echo [INFO] %OUTPUT_MAP%
echo [INFO] =========================

REM =========================
REM COPY TO SRC FOLDER
REM =========================
echo [INFO] Copying map to Src folder...

copy "%OUTPUT_MAP%" "%DEST_MAP%" >nul

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Copy failed!
    exit /b 1
)

echo [INFO] Copied to:
echo [INFO] %DEST_MAP%

echo [INFO] Done.