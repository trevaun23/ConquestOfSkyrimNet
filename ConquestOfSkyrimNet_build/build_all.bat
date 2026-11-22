@echo off
rem Conquest of SkyrimNet - builds the UI DLL (first run 10-30 min) and then the mod zip.
set "HERE=%~dp0"
echo [1/2] Building ConquestOfSkyrimNetUI.dll ...
call "%HERE%ui\build_dll.bat" nopause
echo [2/2] Compiling scripts and packaging ...
call "%HERE%compile_and_package.bat"
echo.
echo Finished. Logs: ui\build_log.txt and compile_log.txt
pause
