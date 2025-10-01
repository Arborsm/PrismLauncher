@echo off
echo Building Prism Launcher WinUI 3...

REM 检查CMake是否安装
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: CMake is not installed or not in PATH
    pause
    exit /b 1
)

REM 检查Visual Studio环境
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 2>nul
if %ERRORLEVEL% neq 0 (
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" 2>nul
    if %ERRORLEVEL% neq 0 (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" 2>nul
        if %ERRORLEVEL% neq 0 (
            echo Error: Visual Studio 2022 not found
            pause
            exit /b 1
        )
    )
)

REM 创建构建目录
if not exist build mkdir build

REM 配置项目
echo Configuring project...
cmake -B build -S . -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed
    pause
    exit /b 1
)

REM 构建项目
echo Building project...
cmake --build build --config Release
if %ERRORLEVEL% neq 0 (
    echo Error: Build failed
    pause
    exit /b 1
)

echo Build completed successfully!
echo Executable: build\bin\Release\PrismLauncherWinUI3.exe

pause