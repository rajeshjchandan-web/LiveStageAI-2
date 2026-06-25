@echo off
cd /d %~dp0

echo ===============================
echo Building LiveStage AI Engine M3
echo ===============================

rmdir /s /q Builds 2>nul

cmake -S . -B Builds -G "Visual Studio 18 2026" -A x64
if errorlevel 1 goto :error

cmake --build Builds --config Release --target ArrangerCoreProof
if errorlevel 1 goto :error

echo.
echo Copying Runtime DLLs...
copy /Y RuntimeDLLs\*.dll Builds\Release\ >nul

if not exist Builds\Release\SoundFonts mkdir Builds\Release\SoundFonts
copy /Y SoundFonts\FluidR3_GM.sf2 Builds\Release\SoundFonts\FluidR3_GM.sf2 >nul

echo.
echo ===============================
echo BUILD SUCCESSFUL
echo ===============================
echo.

cd Builds\Release
ArrangerCoreProof.exe
goto :end

:error
echo.
echo ===============================
echo BUILD FAILED
echo ===============================
pause

:end
pause