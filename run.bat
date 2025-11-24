@echo off
setlocal enabledelayedexpansion

REM Move to script directory
cd /d %~dp0

REM 1) Download headers if missing
if not exist "include\stb_image.h" (
  echo Downloading stb headers...
  if exist "download_headers.ps1" (
    powershell -ExecutionPolicy Bypass -NoProfile -File ".\download_headers.ps1"
    if errorlevel 1 (
      echo Failed to download headers.
      pause
      exit /b 1
    )
  ) else (
    echo download_headers.ps1 not found. Please add stb headers to .\include\ or provide the script.
    pause
    exit /b 1
  )
)

REM 2) Configure and build with CMake
if not exist build mkdir build
cd build

cmake .. 
if errorlevel 1 (
  echo CMake configuration failed.
  pause
  exit /b 1
)

cmake --build . --config Release
if errorlevel 1 (
  echo Build failed.
  pause
  exit /b 1
)

REM 3) Find the executable
cd ..
set "EXE="
for /f "delims=" %%F in ('dir /b /s thumbnail_gen.exe 2^>nul') do (
  set "EXE=%%~fF"
  goto :FOUND_EXE
)
echo thumbnail_gen.exe not found. Build may have failed.
pause
exit /b 1

:FOUND_EXE
echo Found executable: %EXE%

REM 4) Ensure input/output folders
if not exist "data" (
  echo Input folder "data" not found. Create a "data" folder and add images.
  pause
  exit /b 1
)
if not exist "output\thumbnails" mkdir "output\thumbnails"

REM 5) Optional: accept threads as first argument (run.bat 4)
set "NUM_THREADS=0"
if not "%~1"=="" (
  set "NUM_THREADS=%~1"
  set "OMP_NUM_THREADS=%NUM_THREADS%"
)

REM 6) Run the program
echo Running thumbnail_gen...
if "%NUM_THREADS%"=="0" (
  "%EXE%" -i "%cd%\data" -o "%cd%\output\thumbnails"
) else (
  "%EXE%" -i "%cd%\data" -o "%cd%\output\thumbnails" -n %NUM_THREADS%
)

if errorlevel 1 (
  echo Program exited with error code %errorlevel%.
  pause
  exit /b %errorlevel%
)

echo Done. Thumbnails saved to: %cd%\output\thumbnails
pause
endlocal
exit /b 0