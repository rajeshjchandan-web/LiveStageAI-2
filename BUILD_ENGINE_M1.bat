@echo off
cd /d %~dp0
rmdir /s /q Builds 2>nul
cmake -S . -B Builds -G "Visual Studio 18 2026" -A x64
cmake --build Builds --config Release --target ArrangerCoreProof
pause
