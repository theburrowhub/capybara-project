@echo off
REM Build script for static linking (Windows with MinGW)

echo ===================================
echo Capybara Shoot'em Up - Static Build
echo ===================================
echo.

REM Check if CMake is installed
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake is not installed.
    echo Please install CMake from https://cmake.org/download/
    exit /b 1
)

REM Clean previous build
if exist build (
    echo Cleaning previous build...
    rmdir /s /q build
)

mkdir build
cd build

echo Configuring CMake...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DBUILD_STATIC=ON ^
    -DBUILD_GAME=ON ^
    -DBUILD_TOOLS=ON ^
    -DBUILD_DEMOS=ON

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    exit /b 1
)

echo.
echo Building...
mingw32-make -j%NUMBER_OF_PROCESSORS%

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo ===================================
echo Build complete!
echo ===================================
echo.
echo Binaries are in: build\
echo.
echo Main game: build\shootemup.exe
echo Enemy showcase: build\enemy_showcase.exe
echo.
echo To run the game:
echo   build\shootemup.exe
echo.

cd ..

