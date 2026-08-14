@echo off
cd /d %~dp0

echo ==============================================
echo LiveStage AI - FULL CLEAN BUILD
echo Use only when the incremental cache is broken
echo ==============================================

rmdir /s /q BuildsNative 2>nul

cmake -S . -B BuildsNative -G "Visual Studio 18 2026" -A x64
if errorlevel 1 goto :error

cmake --build BuildsNative --config Release --target LiveStageAI --parallel
if errorlevel 1 goto :error

echo.
echo FULL CLEAN BUILD SUCCESSFUL
echo.

if exist "BuildsNative\LiveStageAI_artefacts\Release\LiveStage AI.exe" (
    start "" "BuildsNative\LiveStageAI_artefacts\Release\LiveStage AI.exe"
)
exit /b 0

:error
echo FULL CLEAN BUILD FAILED
pause
exit /b 1

