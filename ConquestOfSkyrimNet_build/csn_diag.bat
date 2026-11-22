@echo off
echo Checking each Conquest of SkyrimNet script (a few minutes)...
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0csn_diag.ps1"
echo Done. Results in csn_diag_log.txt
