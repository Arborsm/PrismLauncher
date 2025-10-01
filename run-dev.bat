@echo off
REM Windows开发环境启动脚本
REM 启动前端开发服务器和后端服务

setlocal enabledelayedexpansion

echo ========================================
echo Prism Launcher 开发环境启动
echo ========================================
echo.

REM 检查依赖
echo [INFO] 检查开发环境...

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

echo [SUCCESS] 开发环境检查通过

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

REM 启动开发服务器
echo [INFO] 启动开发服务器...
echo [INFO] 前端将在 http://localhost:5173 启动
echo [INFO] 按 Ctrl+C 停止服务器
echo.

cd frontend
call npm run dev

pause