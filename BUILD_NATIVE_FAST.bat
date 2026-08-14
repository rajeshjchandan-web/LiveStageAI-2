@echo off
cd /d %~dp0

echo ==============================================
echo LiveStage AI - TRUE FAST BUILD
echo ==============================================

if not exist BuildsNative\CMakeCache.txt (
    echo ERROR: Native cache missing.
    echo Run CONFIGURE_NATIVE.bat once.
    pause
    exit /b 1
)

echo Closing any running LiveStage AI instance...
taskkill /IM "LiveStage AI.exe" /F >nul 2>&1
taskkill /IM LiveStageAI.exe /F >nul 2>&1

echo Building only changed files...
cmake --build BuildsNative --config Release --target LiveStageAI --parallel
if errorlevel 1 goto :error

echo.
echo ==============================================
echo FAST BUILD SUCCESSFUL
echo ==============================================
echo.

if exist "BuildsNative\LiveStageAI_artefacts\Release\LiveStage AI.exe" (
    start "" "BuildsNative\LiveStageAI_artefacts\Release\LiveStage AI.exe"
    exit /b 0
)

echo Build succeeded but EXE was not found automatically.
pause
exit /b 0

:error
echo.
echo ==============================================
echo FAST BUILD FAILED
echo ==============================================
pause
exit /b 1

