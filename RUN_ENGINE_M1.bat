@echo off
cd /d %~dp0
copy /Y libfluidsynth-3.dll Builds\Release\libfluidsynth-3.dll >nul
if not exist Builds\Release\SoundFonts mkdir Builds\Release\SoundFonts
copy /Y SoundFonts\FluidR3_GM.sf2 Builds\Release\SoundFonts\FluidR3_GM.sf2 >nul
cd Builds\Release
ArrangerCoreProof.exe
pause
