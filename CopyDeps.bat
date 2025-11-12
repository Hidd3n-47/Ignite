@echo off
setlocal

set "SRC=.\deps\Lib"
set "DEV_Dest=.\Scratch\Bin\Dev\IgniteGame"
set "REL_Dest=.\Scratch\Bin\Release\IgniteGame"

set DLLs=SDL3.dll SDL3_image.dll SDL3_ttf.dll

if not exist "%DEV_Dest%" mkdir "%DEV_Dest%"
if not exist "%REL_Dest%" mkdir "%REL_Dest%"

for %%F in (%DLLs%) do (
    echo Copying %%F to Dev and Release...
    copy /Y "%SRC%\%%F" "%DEV_Dest%"
    copy /Y "%SRC%\%%F" "%REL_Dest%"
)

set "ConsoleDev=.\Scratch\Bin\Dev\DebugConsole\DebugConsole.dll"
set "ConsoleRel=.\Scratch\Bin\Release\DebugConsole\DebugConsole.dll"

if exist "%ConsoleDev%" (
    echo Copying %ConsoleDev% to Dev...
    copy /Y "%ConsoleDev%" "%DEV_Dest%"
)
if exist "%ConsoleRel%" (
    echo Copying %ConsoleRel% to Release...
    copy /Y "%ConsoleRel%" "%REL_Dest%"
)

echo Done.
pause
endlocal
