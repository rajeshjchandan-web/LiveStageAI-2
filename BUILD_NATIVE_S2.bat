@echo off
cd /d %~dp0

echo ==============================================
echo LiveStage AI 2.0 - Native Windows Sprint S2
echo Realtime Arranger Control Surface
echo ==============================================

if not exist JUCE\CMakeLists.txt (
    echo ERROR: Local JUCE source is missing.
    pause
    exit /b 1
)

rmdir /s /q BuildsNative 2>nul

cmake -S . -B BuildsNative -G "Visual Studio 18 2026" -A x64
if errorlevel 1 goto :error

cmake --build BuildsNative --config Release --target LiveStageAI
if errorlevel 1 goto :error

echo.
echo ==============================================
echo NATIVE S2 BUILD SUCCESSFUL
echo ==============================================
echo.

if exist "BuildsNative\LiveStageAI_artefacts\Release\LiveStage AI.exe" (
    start "" "BuildsNative\LiveStageAI_artefacts\Release\LiveStage AI.exe"
    exit /b 0
)

echo Build succeeded but LiveStage AI.exe was not found automatically.
pause
exit /b 0

:error
echo.
echo ==============================================
echo NATIVE S2 BUILD FAILED
echo ==============================================
pause
exit /b 1
