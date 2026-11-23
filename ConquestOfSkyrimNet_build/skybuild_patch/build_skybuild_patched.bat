@echo off
rem Builds SkyBuild.dll with the Papyrus navmesh functions Conquest of SkyrimNet calls.
rem Works with Visual Studio 2022 or 2026 (C++ desktop workload). Everything goes to build_log.txt.
setlocal EnableExtensions
cd /d "%~dp0"
set "HERE=%~dp0"
rem a new log each run (a stuck process from an earlier run can keep an old one locked)
set "LOG=%HERE%build_%RANDOM%%RANDOM%.log"
> "%HERE%last_log.txt" echo %LOG%
echo Building the patched SkyBuild.dll. This can take 20-60 minutes the first time.
echo Progress is written to %LOG%
echo [%date% %time%] start> "%LOG%"

rem ---- Visual Studio
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" ( echo ERROR: vswhere.exe not found - is Visual Studio installed?>> "%LOG%" & goto :fail )
set "VSDIR="
set "VSYEAR="
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -prerelease -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSDIR=%%i"
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -prerelease -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property catalog_productLineVersion`) do set "VSYEAR=%%i"
if "%VSDIR%"=="" ( echo ERROR: no Visual Studio with the C++ desktop workload found. Add "Desktop development with C++" in the Visual Studio Installer.>> "%LOG%" & goto :fail )
echo Visual Studio %VSYEAR% at %VSDIR%>> "%LOG%"
rem (its output goes elsewhere: a helper it starts in the background would keep the log locked)
set "VSCMD_SKIP_SENDTELEMETRY=1"
call "%VSDIR%\Common7\Tools\VsDevCmd.bat" -arch=amd64 -host_arch=amd64 > "%HERE%vsdevcmd_log.txt" 2>&1
set "GEN=Visual Studio 17 2022"
if "%VSYEAR%"=="2026" set "GEN=Visual Studio 18 2026"
if "%VSYEAR%"=="18" set "GEN=Visual Studio 18 2026"
echo Generator: %GEN%>> "%LOG%"

rem ---- git and cmake
where git >nul 2>&1
if errorlevel 1 set "PATH=%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer\Git\cmd;%PATH%"
where git >nul 2>&1
if errorlevel 1 ( echo ERROR: git not found. Install Git for Windows.>> "%LOG%" & goto :fail )

rem ---- vcpkg (an existing one, else a fresh one here)
if exist "C:\Users\treva\ffbuild\vcpkg\scripts\buildsystems\vcpkg.cmake" set "VCPKG_ROOT=C:\Users\treva\ffbuild\vcpkg"
if not "%VCPKG_ROOT%"=="" if not exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" set "VCPKG_ROOT="
if "%VCPKG_ROOT%"=="" goto :getvcpkg
goto :havevcpkg
:getvcpkg
if exist "%HERE%vcpkg\vcpkg.exe" goto :setvcpkg
echo Getting vcpkg...>> "%LOG%"
git clone https://github.com/microsoft/vcpkg.git "%HERE%vcpkg" >> "%LOG%" 2>&1 || goto :fail
call "%HERE%vcpkg\bootstrap-vcpkg.bat" -disableMetrics >> "%LOG%" 2>&1 || goto :fail
:setvcpkg
set "VCPKG_ROOT=%HERE%vcpkg"
:havevcpkg
echo vcpkg: %VCPKG_ROOT%>> "%LOG%"
if not exist "%VCPKG_ROOT%\vcpkg.exe" call "%VCPKG_ROOT%\bootstrap-vcpkg.bat" -disableMetrics >> "%LOG%" 2>&1

rem ---- cmake and ninja (from vcpkg when Visual Studio has none)
where cmake >nul 2>&1
if errorlevel 1 for /f "delims=" %%c in ('"%VCPKG_ROOT%\vcpkg.exe" fetch cmake') do set "CMAKE_EXE=%%c"
if defined CMAKE_EXE for %%d in ("%CMAKE_EXE%") do set "PATH=%%~dpd;%PATH%"
where ninja >nul 2>&1
if errorlevel 1 for /f "delims=" %%n in ('"%VCPKG_ROOT%\vcpkg.exe" fetch ninja') do set "NINJA_EXE=%%n"
if defined NINJA_EXE for %%d in ("%NINJA_EXE%") do set "PATH=%%~dpd;%PATH%"
where cmake >> "%LOG%" 2>&1
if errorlevel 1 ( echo ERROR: could not get cmake.>> "%LOG%" & goto :fail )
where ninja >> "%LOG%" 2>&1
if errorlevel 1 ( echo ERROR: could not get ninja.>> "%LOG%" & goto :fail )
cmake --version >> "%LOG%" 2>&1

rem ---- SkyBuild source at the patched commit
set /p BASE=<"%HERE%BASE_COMMIT.txt"
if exist "%HERE%SkyBuild\.git" goto :cloned
if exist "%HERE%SkyBuild" rmdir /s /q "%HERE%SkyBuild"
echo Cloning SkyBuild...
echo Cloning SkyBuild...>> "%LOG%"
git clone --recursive https://github.com/Sokco/SkyBuild.git "%HERE%SkyBuild" >> "%LOG%" 2>&1 || goto :fail
:cloned
cd /d "%HERE%SkyBuild"
git checkout -f %BASE% >> "%LOG%" 2>&1 || goto :fail
rem an earlier patch's new files go too (the build folder stays, so libraries aren't rebuilt)
git clean -fdq -e build >> "%LOG%" 2>&1
git submodule update --init --recursive >> "%LOG%" 2>&1
git apply --check --whitespace=nowarn "%HERE%skybuild-navmesh-papyrus.patch" >nul 2>&1
if not errorlevel 1 goto :apply
git apply --check -R --whitespace=nowarn "%HERE%skybuild-navmesh-papyrus.patch" >nul 2>&1
if errorlevel 1 ( echo ERROR: the patch does not apply.>> "%LOG%" & goto :fail )
echo Patch already applied.>> "%LOG%"
goto :configure
:apply
git apply --whitespace=nowarn "%HERE%skybuild-navmesh-papyrus.patch" >> "%LOG%" 2>&1 || goto :fail
echo Patch applied.>> "%LOG%"

:configure
rem SkyBuild's preset settings, with Ninja instead of the Visual Studio 2022 generator (works with any VS)
if exist "%LOCALAPPDATA%\vcpkg\registries" rmdir /s /q "%LOCALAPPDATA%\vcpkg\registries"
echo Configuring (vcpkg builds the libraries now; this is the long part)...
echo [%date% %time%] configure>> "%LOG%"
cmake -S . -B build/ninja -G Ninja --fresh -DCMAKE_BUILD_TYPE=Release ^
  "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
  -DVCPKG_TARGET_TRIPLET=x64-windows-skse -DVCPKG_HOST_TRIPLET=x64-windows-skse ^
  "-DVCPKG_OVERLAY_TRIPLETS=%HERE%SkyBuild/cmake" ^
  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL ^
  "-DCMAKE_CXX_FLAGS=/DWIN32_LEAN_AND_MEAN /DNOMINMAX /DUNICODE /D_UNICODE /EHsc" ^
  -DDEPLOY_TO_MODS=OFF >> "%LOG%" 2>&1 || goto :fail
echo Building...
echo [%date% %time%] build>> "%LOG%"
cmake --build build/ninja --config Release >> "%LOG%" 2>&1 || goto :fail
set "DLL="
for /r "build\ninja" %%f in (SkyBuild.dll) do if exist "%%f" set "DLL=%%f"
if not defined DLL ( echo ERROR: SkyBuild.dll was not produced.>> "%LOG%" & goto :fail )
copy /y "%DLL%" "%HERE%SkyBuild.dll" >> "%LOG%" 2>&1
echo [%date% %time%] RESULT: OK - %HERE%SkyBuild.dll>> "%LOG%"
echo Done: %HERE%SkyBuild.dll
exit /b 0

:fail
echo [%date% %time%] RESULT: FAILED>> "%LOG%"
echo Build failed - see %LOG%
exit /b 1
