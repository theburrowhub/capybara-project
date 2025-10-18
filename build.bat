@echo off
REM Convenience build script for CMake on Windows

setlocal enabledelayedexpansion

set BUILD_TYPE=Release
set BUILD_DIR=build
set CLEAN=0
set STATIC=ON
set BUILD_GAME=ON
set BUILD_TOOLS=ON
set BUILD_DEMOS=ON
set RUN_GAME=0
set INSTALL=0
set PACKAGE=0

REM Parse arguments
:parse_args
if "%~1"=="" goto end_parse
if /i "%~1"=="-d" set BUILD_TYPE=Debug& shift & goto parse_args
if /i "%~1"=="--debug" set BUILD_TYPE=Debug& shift & goto parse_args
if /i "%~1"=="-c" set CLEAN=1& shift & goto parse_args
if /i "%~1"=="--clean" set CLEAN=1& shift & goto parse_args
if /i "%~1"=="-s" set STATIC=ON& shift & goto parse_args
if /i "%~1"=="--static" set STATIC=ON& shift & goto parse_args
if /i "%~1"=="--game-only" set BUILD_TOOLS=OFF& set BUILD_DEMOS=OFF& shift & goto parse_args
if /i "%~1"=="-r" set RUN_GAME=1& shift & goto parse_args
if /i "%~1"=="--run" set RUN_GAME=1& shift & goto parse_args
if /i "%~1"=="-i" set INSTALL=1& shift & goto parse_args
if /i "%~1"=="--install" set INSTALL=1& shift & goto parse_args
if /i "%~1"=="-p" set PACKAGE=1& shift & goto parse_args
if /i "%~1"=="--package" set PACKAGE=1& shift & goto parse_args
if /i "%~1"=="-h" goto show_help
if /i "%~1"=="--help" goto show_help
echo Unknown option: %~1
echo Use -h or --help for usage information
exit /b 1

:show_help
echo Usage: %0 [options]
echo.
echo Options:
echo   -d, --debug      Build in debug mode (default: Release)
echo   -c, --clean      Clean build directory before building
echo   -s, --static     Build static binaries (default on Windows)
echo   --game-only      Build only the main game (no tools/demos)
echo   -r, --run        Run the game after building
echo   -i, --install    Install files to build/install
echo   -p, --package    Create distribution package
echo   -h, --help       Show this help message
echo.
echo Examples:
echo   %0                    # Build in release mode
echo   %0 -d                 # Build in debug mode
echo   %0 -c -r              # Clean build and run
echo   %0 --static -p        # Build static and create package
exit /b 0

:end_parse

echo ===============================================
echo Capybara Shoot'em Up - Build Script
echo ===============================================

REM Clean if requested
if %CLEAN%==1 (
    echo Cleaning build directory...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
)

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

REM Configure CMake
echo Configuring CMake...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DBUILD_STATIC=%STATIC% ^
    -DBUILD_GAME=%BUILD_GAME% ^
    -DBUILD_TOOLS=%BUILD_TOOLS% ^
    -DBUILD_DEMOS=%BUILD_DEMOS%

if errorlevel 1 (
    echo Error: CMake configuration failed
    exit /b 1
)

REM Build
echo Building...
cmake --build . -j%NUMBER_OF_PROCESSORS%

if errorlevel 1 (
    echo Error: Build failed
    exit /b 1
)

echo Build completed successfully!

REM Install if requested
if %INSTALL%==1 (
    echo Installing...
    cmake --install . --prefix install_temp
    echo Installation completed in build\install_temp
)

REM Package if requested
if %PACKAGE%==1 (
    echo Creating package...
    cpack
    echo Package created successfully!
    dir *.zip
)

REM Run if requested
if %RUN_GAME%==1 (
    if exist "shootemup.exe" (
        echo Starting game...
        cd ..
        build\shootemup.exe
    ) else (
        echo Error: shootemup.exe not found
        exit /b 1
    )
)

echo ===============================================
echo Build process complete!
echo ===============================================

if %RUN_GAME%==0 (
    echo.
    echo To run the game:
    echo   build\shootemup.exe
    echo.
)

cd ..
exit /b 0

