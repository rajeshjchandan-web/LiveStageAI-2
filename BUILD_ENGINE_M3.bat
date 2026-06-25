@echo off
cd /d %~dp0

echo Building LiveStage AI Engine...
rmdir /s /q Builds 2>nul

cmake -S . -B Builds -G "Visual Studio 18 2026" -A x64
if errorlevel 1 pause & exit /b 1

cmake --build Builds --config Release --target ArrangerCoreProof
if errorlevel 1 pause & exit /b 1

echo Copying runtime files...
copy /Y RuntimeDLLs\*.dll Builds\Release\ >nul

if not exist Builds\Release\SoundFonts mkdir Builds\Release\SoundFonts
copy /Y SoundFonts\FluidR3_GM.sf2 Builds\Release\SoundFonts\FluidR3_GM.sf2 >nul

cd Builds\Release
ArrangerCoreProof.exe
pause
