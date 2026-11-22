@echo off
setlocal EnableExtensions EnableDelayedExpansion
rem ==========================================================================
rem  Conquest of SkyrimNet - one-click build of ConquestOfSkyrimNetUI.dll
rem  Double-click this file. It takes 10-30 minutes the first time
rem  (vcpkg compiles CommonLibSSE-NG). Everything is logged to build_log.txt.
rem ==========================================================================

set "SRC=%~dp0"
set "WORK=%USERPROFILE%\ffbuild"
set "LOG=%SRC%build_log.txt"

echo Building ConquestOfSkyrimNetUI.dll ... this can take 10-30 minutes.
echo Progress is written to "%LOG%"
echo.

call :main > "%LOG%" 2>&1
set "RC=%ERRORLEVEL%"
echo.
if "%RC%"=="0" (
  echo SUCCESS. Install "%SRC%ConquestOfSkyrimNetUI_DLL.zip" with Vortex.
) else (
  echo BUILD FAILED ^(step %RC%^). Tell Claude - it will read build_log.txt.
)
echo.
if /i not "%~1"=="nopause" pause
exit /b %RC%

:main
echo === %DATE% %TIME% ===
echo SRC=%SRC%
echo WORK=%WORK%

rem ---- 1. Visual Studio -----------------------------------------------------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" ( echo [1] vswhere not found & exit /b 1 )
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSPATH=%%i"
if not defined VSPATH ( echo [1] Visual Studio with C++ tools not found & exit /b 1 )
echo VSPATH=%VSPATH%
call "%VSPATH%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64
if errorlevel 1 ( echo [1] VsDevCmd failed & exit /b 1 )

rem ---- 2. git and cmake (VS ships both) ------------------------------------
where git >nul 2>&1 || set "PATH=%VSPATH%\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer\Git\cmd;%PATH%"
where git || ( echo [2] git not found - install Git for Windows & exit /b 2 )
git --version

rem ---- 3. Copy project to a path without spaces ----------------------------
if not exist "%WORK%" mkdir "%WORK%"
robocopy "%SRC%." "%WORK%\ConquestOfSkyrimNetUI" /E /IS /IT /XD build _deps /XF build_log.txt ConquestOfSkyrimNetUI_DLL.zip /NFL /NDL /NJH /NJS
if errorlevel 8 ( echo [3] copy failed & exit /b 3 )
set "PROJ=%WORK%\ConquestOfSkyrimNetUI"

rem ---- 3b. Nothing changed since the last good build: keep that DLL --------------
set "HASH="
for /f "usebackq delims=" %%h in (`powershell -NoProfile -Command "$f = Get-ChildItem -LiteralPath '%SRC%src','%SRC%include','%SRC%CMakeLists.txt','%SRC%vcpkg.json','%SRC%vcpkg-configuration.json' -Recurse -File -ErrorAction SilentlyContinue | Sort-Object FullName; ($f | ForEach-Object { (Get-FileHash -Algorithm SHA1 -LiteralPath $_.FullName).Hash }) -join ''"`) do set "HASH=%%h"
set "OLDHASH="
if exist "%WORK%\dll_source_hash.txt" set /p OLDHASH=<"%WORK%\dll_source_hash.txt"
if defined HASH if "!HASH!"=="!OLDHASH!" if exist "%SRC%ConquestOfSkyrimNetUI_DLL.zip" (
  echo UI plugin source unchanged since the last build - keeping ConquestOfSkyrimNetUI_DLL.zip
  exit /b 0
)

rem ---- 4. vcpkg ---------------------------------------------------------------
rem The vcpkg clone is NOT updated on every build: a new baseline means rebuilding every package (10-30 min),
rem and the package versions this plugin needs don't change. Delete %WORK%\vcpkg to start over.
set "VCPKG=%WORK%\vcpkg"
if not exist "%VCPKG%\.git" (
  git clone https://github.com/microsoft/vcpkg "%VCPKG%" || ( echo [4] vcpkg clone failed & exit /b 4 )
)
if not exist "%VCPKG%\vcpkg.exe" (
  call "%VCPKG%\bootstrap-vcpkg.bat" -disableMetrics || ( echo [4] vcpkg bootstrap failed & exit /b 4 )
)
where cmake >nul 2>&1 || (
  for /f "delims=" %%c in ('"%VCPKG%\vcpkg.exe" fetch cmake') do set "CMAKE_EXE=%%c"
)
if defined CMAKE_EXE for %%d in ("!CMAKE_EXE!") do set "PATH=%%~dpd;!PATH!"
where ninja >nul 2>&1 || (
  for /f "delims=" %%n in ('"%VCPKG%\vcpkg.exe" fetch ninja') do set "NINJA_EXE=%%n"
)
if defined NINJA_EXE for %%d in ("!NINJA_EXE!") do set "PATH=%%~dpd;!PATH!"
where cmake || ( echo [4] could not get cmake & exit /b 4 )
where ninja || ( echo [4] could not get ninja & exit /b 4 )
cmake --version
for /f %%h in ('git -C "%VCPKG%" rev-parse HEAD') do set "BASE_MS=%%h"
rem colorglass (CommonLibSSE-NG) baseline: looked up once, then kept, so its packages aren't rebuilt each time
set "BASE_CG="
if exist "%WORK%\colorglass_baseline.txt" set /p BASE_CG=<"%WORK%\colorglass_baseline.txt"
if not defined BASE_CG (
  for /f "tokens=1" %%h in ('git ls-remote https://gitlab.com/colorglass/vcpkg-colorglass HEAD') do set "BASE_CG=%%h"
  if defined BASE_CG ( >"%WORK%\colorglass_baseline.txt" echo !BASE_CG!)
)
if not defined BASE_CG ( echo [4] could not reach the colorglass registry & exit /b 4 )
echo baseline microsoft=%BASE_MS%  colorglass=%BASE_CG%
powershell -NoProfile -Command "(Get-Content '%PROJ%\vcpkg-configuration.json') -replace 'REPLACE_WITH_BASELINE_MS','%BASE_MS%' -replace 'REPLACE_WITH_BASELINE_CG','%BASE_CG%' | Set-Content '%PROJ%\vcpkg-configuration.json'"
type "%PROJ%\vcpkg-configuration.json"

rem ---- 5. PrismaUI_API.h --------------------------------------------------
if not exist "%PROJ%\include\PrismaUI_API.h" (
  if not exist "%WORK%\prisma-example\.git" (
    git clone --depth 1 https://github.com/PrismaUI-SKSE/example-skse-plugin "%WORK%\prisma-example" || ( echo [5] could not download PrismaUI example & exit /b 5 )
  )
  for /r "%WORK%\prisma-example" %%f in (PrismaUI_API.h) do if exist "%%f" copy /y "%%f" "%PROJ%\include\PrismaUI_API.h" >nul
)
if not exist "%PROJ%\include\PrismaUI_API.h" ( echo [5] PrismaUI_API.h not found in example repo & exit /b 5 )
echo PrismaUI_API.h ready

rem ---- 6. Configure + build ----------------------------------------------
rem vcpkg's cache of git registries can get corrupted ("fetch-pack: invalid index-pack output"), which makes
rem configure retry forever. It is only a cache: clear it so it is fetched fresh.
if exist "%LOCALAPPDATA%\vcpkg\registries" rmdir /s /q "%LOCALAPPDATA%\vcpkg\registries"
cmake -S "%PROJ%" -B "%PROJ%\build" -G Ninja -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_TOOLCHAIN_FILE="%VCPKG%\scripts\buildsystems\vcpkg.cmake" ^
  -DVCPKG_TARGET_TRIPLET=x64-windows-static-md
if errorlevel 1 ( echo [6] cmake configure failed & exit /b 6 )
cmake --build "%PROJ%\build" --config Release --clean-first
if errorlevel 1 ( echo [7] compile failed & exit /b 7 )

rem ---- 7. Package for Vortex ---------------------------------------------
set "DLL="
for /r "%PROJ%\build" %%f in (ConquestOfSkyrimNetUI.dll) do if exist "%%f" set "DLL=%%f"
if not defined DLL ( echo [8] ConquestOfSkyrimNetUI.dll not produced & exit /b 8 )
set "OUT=%WORK%\out"
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%\SKSE\Plugins"
copy /y "!DLL!" "%OUT%\SKSE\Plugins\" >nul
powershell -NoProfile -Command "Compress-Archive -Force -Path '%OUT%\SKSE' -DestinationPath '%SRC%ConquestOfSkyrimNetUI_DLL.zip'"
if errorlevel 1 ( echo [8] zip failed & exit /b 8 )
if defined HASH ( >"%WORK%\dll_source_hash.txt" echo !HASH!)
echo Built !DLL!
echo === done %TIME% ===
exit /b 0
