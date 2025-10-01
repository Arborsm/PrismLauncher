@echo off
REM Windows环境设置脚本
REM 自动安装必要的依赖和工具

setlocal enabledelayedexpansion

echo ========================================
echo Prism Launcher Windows环境设置
echo ========================================
echo.

REM 检查管理员权限
net session >nul 2>&1
if errorlevel 1 (
    echo [ERROR] 请以管理员身份运行此脚本
    pause
    exit /b 1
)

echo [INFO] 开始设置Windows环境...

REM 1. 安装Chocolatey包管理器
echo [INFO] 检查Chocolatey...
where choco >nul 2>&1
if errorlevel 1 (
    echo [INFO] 安装Chocolatey...
    powershell -Command "Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    if errorlevel 1 (
        echo [ERROR] Chocolatey安装失败
        pause
        exit /b 1
    )
    echo [SUCCESS] Chocolatey安装完成
) else (
    echo [INFO] Chocolatey已安装
)

REM 2. 安装必要的工具
echo [INFO] 安装必要的工具...

REM 安装Git
echo [INFO] 安装Git...
choco install git -y
if errorlevel 1 (
    echo [WARNING] Git安装失败，请手动安装
)

REM 安装CMake
echo [INFO] 安装CMake...
choco install cmake -y
if errorlevel 1 (
    echo [WARNING] CMake安装失败，请手动安装
)

REM 安装Node.js
echo [INFO] 安装Node.js...
choco install nodejs -y
if errorlevel 1 (
    echo [WARNING] Node.js安装失败，请手动安装
)

REM 安装Visual Studio Build Tools
echo [INFO] 安装Visual Studio Build Tools...
choco install visualstudio2022buildtools -y
if errorlevel 1 (
    echo [WARNING] Visual Studio Build Tools安装失败，请手动安装
)

REM 安装Python（用于某些构建工具）
echo [INFO] 安装Python...
choco install python -y
if errorlevel 1 (
    echo [WARNING] Python安装失败，请手动安装
)

REM 3. 刷新环境变量
echo [INFO] 刷新环境变量...
call refreshenv

REM 4. 验证安装
echo [INFO] 验证安装...

echo [INFO] 检查Git...
git --version
if errorlevel 1 (
    echo [ERROR] Git未正确安装
) else (
    echo [SUCCESS] Git安装成功
)

echo [INFO] 检查CMake...
cmake --version
if errorlevel 1 (
    echo [ERROR] CMake未正确安装
) else (
    echo [SUCCESS] CMake安装成功
)

echo [INFO] 检查Node.js...
node --version
if errorlevel 1 (
    echo [ERROR] Node.js未正确安装
) else (
    echo [SUCCESS] Node.js安装成功
)

echo [INFO] 检查npm...
npm --version
if errorlevel 1 (
    echo [ERROR] npm未正确安装
) else (
    echo [SUCCESS] npm安装成功
)

REM 5. 创建开发环境
echo [INFO] 设置开发环境...

REM 创建项目目录结构
if not exist "frontend\node_modules" (
    echo [INFO] 安装前端依赖...
    cd frontend
    call npm install
    cd ..
)

REM 6. 完成设置
echo.
echo ========================================
echo 环境设置完成！
echo ========================================
echo.
echo 下一步：
echo 1. 打开 "Developer Command Prompt for VS 2022"
echo 2. 导航到项目目录
echo 3. 运行: build.bat all
echo.
echo 或者使用PowerShell：
echo 1. 打开PowerShell
echo 2. 导航到项目目录
echo 3. 运行: .\build.ps1 all
echo.

pause