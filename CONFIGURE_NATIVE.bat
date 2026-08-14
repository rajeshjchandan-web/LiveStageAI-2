@echo off
cd /d %~dp0

echo ==============================================
echo LiveStage AI - CONFIGURE NATIVE PROJECT
echo ==============================================

if not exist JUCE\CMakeLists.txt (
    echo ERROR: Local JUCE source missing.
    pause
    exit /b 1
)

taskkill /IM "LiveStage AI.exe" /F >nul 2>&1
taskkill /IM LiveStageAI.exe /F >nul 2>&1

cmake -S . -B BuildsNative -G "Visual Studio 18 2026" -A x64
if errorlevel 1 goto :error

echo CONFIGURE SUCCESSFUL
echo Now run BUILD_NATIVE_FAST.bat
exit /b 0

:error
echo CONFIGURE FAILED
pause
exit /b 1

