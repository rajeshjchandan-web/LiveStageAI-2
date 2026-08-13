@echo off
cd /d %~dp0

echo ==============================================
echo LiveStage AI 2.0 - Native Windows Sprint S1.1
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
echo NATIVE BUILD SUCCESSFUL
echo ==============================================
echo.

if exist "BuildsNative\LiveStageAI_artefacts\Release\LiveStage AI.exe" (
    start "" "BuildsNative\LiveStageAI_artefacts\Release\LiveStage AI.exe"
    exit /b 0
)

if exist "BuildsNative\LiveStageAI_artefacts\Release\LiveStageAI.exe" (
    start "" "BuildsNative\LiveStageAI_artefacts\Release\LiveStageAI.exe"
    exit /b 0
)

echo Build succeeded. Searching for executable...
for /r BuildsNative %%F in ("LiveStage AI.exe") do (
    start "" "%%F"
    exit /b 0
)

for /r BuildsNative %%F in ("LiveStageAI.exe") do (
    start "" "%%F"
    exit /b 0
)

echo Build succeeded but the EXE was not found automatically.
pause
exit /b 0

:error
echo.
echo ==============================================
echo NATIVE BUILD FAILED
echo ==============================================
pause
exit /b 1
