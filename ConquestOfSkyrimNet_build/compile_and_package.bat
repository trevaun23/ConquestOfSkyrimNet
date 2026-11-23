@echo off
setlocal EnableExtensions EnableDelayedExpansion
rem ==========================================================================
rem  Conquest of SkyrimNet - compile the Papyrus scripts and build one Vortex mod zip
rem  Output: ..\ConquestOfSkyrimNet_v<version>.zip (version from fomod\info.xml)   Log: compile_log.txt
rem ==========================================================================
set "HERE=%~dp0"
for %%a in ("%HERE%..") do set "ROOT=%%~fa"
set "LOG=%HERE%compile_log.txt"
echo Compiling Conquest of SkyrimNet scripts and packaging the mod...
set "ZIPNAME=ConquestOfSkyrimNet_Full.zip"
call :main > "%LOG%" 2>&1
set "RC=%ERRORLEVEL%"
if "%RC%"=="0" ( echo SUCCESS - "%ROOT%\!ZIPNAME!" is ready. ) else ( echo FAILED at step %RC% - see compile_log.txt )
exit /b %RC%

:main
set "GAME=C:\Program Files (x86)\The.Elder.Scrolls.V.Skyrim.Special.Edition.v1.6.1170\game"
set "DATA=%GAME%\Data"
set "COS=%ROOT%\Conquest of Skyrim 0.3.9"
set "STAGE=%USERPROFILE%\ffbuild\csnmod"
set "PC=%GAME%\Papyrus Compiler\PapyrusCompiler.exe"
echo ROOT=%ROOT%
if not exist "%PC%" ( echo [1] PapyrusCompiler.exe not found & exit /b 1 )
if not exist "%COS%\scripts\source" ( echo [1] CoS sources not found & exit /b 1 )
if not exist "%HERE%CSN_Dev.zip" ( echo [1] CSN_Dev.zip not found & exit /b 1 )

set "FLG="
for %%p in ("%DATA%\source\Scripts\TESV_Papyrus_Flags.flg" "%DATA%\Scripts\Source\TESV_Papyrus_Flags.flg" "%DATA%\Source\Scripts\TESV_Papyrus_Flags.flg") do if exist "%%~p" if not defined FLG set "FLG=%%~p"
if not defined FLG ( echo [1] TESV_Papyrus_Flags.flg not found & exit /b 1 )
echo FLAGS=%FLG%

rem ---- 2. Stage: CoS assets minus its plugin, then Conquest of SkyrimNet on top ----
if exist "%STAGE%" rmdir /s /q "%STAGE%"
mkdir "%STAGE%"
robocopy "%COS%" "%STAGE%" /E /XF conquerskyrim.esp /NFL /NDL /NJH /NJS
if errorlevel 8 ( echo [2] copy of CoS assets failed & exit /b 2 )
powershell -NoProfile -Command "Expand-Archive -Force -LiteralPath '%HERE%CSN_Dev.zip' -DestinationPath '%STAGE%'"
if errorlevel 1 ( echo [2] unzip of CSN_Dev.zip failed & exit /b 2 )
rem the zip is named after the version in fomod\info.xml (e.g. ConquestOfSkyrimNet_v1.7.4.zip)
set "VER="
for /f "usebackq delims=" %%v in (`powershell -NoProfile -Command "try { ([xml](Get-Content -LiteralPath '%STAGE%\_fomod\info.xml')).fomod.Version } catch { '' }"`) do set "VER=%%v"
if defined VER ( set "ZIPNAME=ConquestOfSkyrimNet_v!VER!.zip" ) else ( echo NOTE: no version in fomod\info.xml - using ConquestOfSkyrimNet_Full.zip )
echo ZIP=!ZIPNAME!
rem the patched SkyBuild.dll you built last (Build_SkyBuild_Patched) wins over the one in CSN_Dev.zip
if exist "%USERPROFILE%\ffbuild\sbp\SkyBuild.dll" (
  if not exist "%STAGE%\_skybuild\SKSE\Plugins" mkdir "%STAGE%\_skybuild\SKSE\Plugins"
  copy /y "%USERPROFILE%\ffbuild\sbp\SkyBuild.dll" "%STAGE%\_skybuild\SKSE\Plugins\SkyBuild.dll" >nul
  echo SkyBuild.dll taken from ffbuild\sbp
)
if exist "%STAGE%\SEQ\conquerskyrim.seq" move /y "%STAGE%\SEQ\conquerskyrim.seq" "%STAGE%\SEQ\ConquestOfSkyrimNet.seq"
if exist "%HERE%ui\ConquestOfSkyrimNetUI_DLL.zip" (
  powershell -NoProfile -Command "Expand-Archive -Force -LiteralPath '%HERE%ui\ConquestOfSkyrimNetUI_DLL.zip' -DestinationPath '%STAGE%'"
  echo ConquestOfSkyrimNetUI.dll included
) else (
  echo NOTE: ConquestOfSkyrimNetUI.dll not built - menus fall back to vanilla boxes
)

rem ---- 3. Compile the changed scripts ---------------------------------------
set "IMP=%STAGE%\Scripts\Source;%HERE%stubs;%DATA%\Scripts\Source;%DATA%\source\Scripts"
"%PC%" "%STAGE%\Source\Scripts" -all -f="%FLG%" -i="%IMP%" -o="%STAGE%\Scripts" > "%USERPROFILE%\ffbuild\papyrus.txt" 2>&1
type "%USERPROFILE%\ffbuild\papyrus.txt"
findstr /c:"compilation failed" "%USERPROFILE%\ffbuild\papyrus.txt" >nul && ( echo [3] Papyrus compile errors - see above & exit /b 3 )
rmdir /s /q "%STAGE%\Source"

rem ---- 4. FOMOD layout: fomod\, 00 Core\ (always), 10 SkyrimNet Context\ and 20 SkyBuild Navmesh\ (optional) ----
set "PKG=%USERPROFILE%\ffbuild\csnpkg"
if exist "%PKG%" rmdir /s /q "%PKG%"
mkdir "%PKG%\fomod"
mkdir "%PKG%\10 SkyrimNet Context\SKSE\Plugins\SkyrimNet\external\conquestofskyrimnet.skyrimnet"
robocopy "%STAGE%\_fomod" "%PKG%\fomod" /E /MOVE /NFL /NDL /NJH /NJS
robocopy "%STAGE%\SKSE\Plugins\SkyrimNet\external\conquestofskyrimnet.skyrimnet" "%PKG%\10 SkyrimNet Context\SKSE\Plugins\SkyrimNet\external\conquestofskyrimnet.skyrimnet" /E /MOVE /NFL /NDL /NJH /NJS
if exist "%STAGE%\_skybuild" (
  mkdir "%PKG%\20 SkyBuild Navmesh"
  robocopy "%STAGE%\_skybuild" "%PKG%\20 SkyBuild Navmesh" /E /MOVE /NFL /NDL /NJH /NJS
  if exist "%STAGE%\_skybuild" rmdir /s /q "%STAGE%\_skybuild"
  echo SkyBuild navmesh option included
)
robocopy "%STAGE%" "%PKG%\00 Core" /E /MOVE /NFL /NDL /NJH /NJS
if errorlevel 8 ( echo [4] FOMOD layout failed & exit /b 4 )
if not exist "%PKG%\fomod\ModuleConfig.xml" ( echo [4] fomod files missing & exit /b 4 )

rem ---- 5. Zip for Vortex ----------------------------------------------------
if exist "%ROOT%\!ZIPNAME!" del /q "%ROOT%\!ZIPNAME!"
powershell -NoProfile -Command "Compress-Archive -Path '%PKG%\*' -DestinationPath '%ROOT%\!ZIPNAME!'"
if errorlevel 1 ( echo [5] zip failed & exit /b 5 )
echo === done ===
exit /b 0
