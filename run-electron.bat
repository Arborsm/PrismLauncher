@echo off
REM Windows Electron应用启动脚本

setlocal enabledelayedexpansion

echo ========================================
echo Prism Launcher Electron应用启动
echo ========================================
echo.

REM 检查依赖
echo [INFO] 检查Electron环境...

REM 检查Node.js
node --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Node.js未安装，请先运行setup-windows.bat
    pause
    exit /b 1
)

REM 检查npm
npm --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] npm未安装，请先运行setup-windows.bat
    pause
    exit /b 1
)

echo [SUCCESS] Electron环境检查通过

REM 安装前端依赖（如果需要）
if not exist "frontend\node_modules" (
    echo [INFO] 安装前端依赖...
    cd frontend
    call npm install
    if errorlevel 1 (
        echo [ERROR] 前端依赖安装失败
        pause
        exit /b 1
    )
    cd ..
)

REM 启动Electron应用
echo [INFO] 启动Electron应用...
echo [INFO] 按 Ctrl+C 停止应用
echo.

cd frontend
call npm run electron-dev

pause