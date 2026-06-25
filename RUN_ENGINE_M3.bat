@echo off
cd /d %~dp0
if not exist Builds\Release mkdir Builds\Release
copy /Y *.dll Builds\Release\ >nul
if not exist Builds\Release\SoundFonts mkdir Builds\Release\SoundFonts
copy /Y SoundFonts\FluidR3_GM.sf2 Builds\Release\SoundFonts\FluidR3_GM.sf2 >nul
cd Builds\Release
ArrangerCoreProof.exe
pause
