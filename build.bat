@echo off
REM Prism Launcher Modern Architecture Build Script for Windows
REM Windows批处理构建脚本

setlocal enabledelayedexpansion

REM 颜色定义（Windows 10+支持）
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM 日志函数
:log_info
echo %BLUE%[INFO]%NC% %~1
goto :eof

:log_success
echo %GREEN%[SUCCESS]%NC% %~1
goto :eof

:log_warning
echo %YELLOW%[WARNING]%NC% %~1
goto :eof

:log_error
echo %RED%[ERROR]%NC% %~1
goto :eof

REM 检查依赖
:check_dependencies
call :log_info "检查构建依赖..."

REM 检查Rust
rustc --version >nul 2>&1
if errorlevel 1 (
    call :log_error "Rust未安装，请先安装Rust"
    call :log_info "访问 https://rustup.rs/ 安装Rust"
    exit /b 1
)

REM 检查Cargo
cargo --version >nul 2>&1
if errorlevel 1 (
    call :log_error "Cargo未安装，请先安装Cargo"
    exit /b 1
)

REM 检查Node.js
node --version >nul 2>&1
if errorlevel 1 (
    call :log_error "Node.js未安装，请先安装Node.js"
    exit /b 1
)

REM 检查npm
npm --version >nul 2>&1
if errorlevel 1 (
    call :log_error "npm未安装，请先安装npm"
    exit /b 1
)

REM 检查Tauri CLI
npx tauri --version >nul 2>&1
if errorlevel 1 (
    call :log_warning "Tauri CLI未安装，将自动安装"
)

call :log_success "所有依赖检查通过"
goto :eof

REM 构建Tauri后端
:build_tauri
call :log_info "开始构建Tauri后端..."

cd tauri

REM 安装Tauri CLI（如果需要）
call :log_info "检查Tauri CLI..."
npx tauri --version >nul 2>&1
if errorlevel 1 (
    call :log_info "安装Tauri CLI..."
    npm install -g @tauri-apps/cli
)

REM 构建Tauri应用
call :log_info "构建Tauri应用..."
npx tauri build
if errorlevel 1 (
    call :log_error "Tauri构建失败"
    exit /b 1
)

cd ..
call :log_success "Tauri后端构建完成"
goto :eof

REM 构建前端
:build_frontend
call :log_info "开始构建前端..."

cd frontend

REM 安装依赖
call :log_info "安装前端依赖..."
call npm install
if errorlevel 1 (
    call :log_error "npm install失败"
    exit /b 1
)

REM 构建前端
call :log_info "构建前端应用..."
call npm run build
if errorlevel 1 (
    call :log_error "前端构建失败"
    exit /b 1
)

cd ..
call :log_success "前端构建完成"
goto :eof

REM 构建Tauri应用
:build_tauri_app
call :log_info "开始构建Tauri应用..."

cd frontend

REM 安装依赖
call :log_info "安装前端依赖..."
call npm install
if errorlevel 1 (
    call :log_error "前端依赖安装失败"
    exit /b 1
)

REM 构建前端
call :log_info "构建前端..."
call npm run build
if errorlevel 1 (
    call :log_error "前端构建失败"
    exit /b 1
)

cd ..

REM 构建Tauri应用
call :log_info "构建Tauri应用..."
cd tauri
npx tauri build
if errorlevel 1 (
    call :log_error "Tauri应用构建失败"
    exit /b 1
)

cd ..
call :log_success "Tauri应用构建完成"
goto :eof

REM 清理构建文件
:clean
call :log_info "清理构建文件..."

REM 清理Tauri构建
if exist "tauri\target" (
    rmdir /s /q "tauri\target"
    call :log_info "已清理Tauri构建文件"
)

REM 清理前端构建
if exist "frontend\dist" (
    rmdir /s /q "frontend\dist"
    call :log_info "已清理前端构建文件"
)

REM 清理node_modules
if exist "frontend\node_modules" (
    rmdir /s /q "frontend\node_modules"
    call :log_info "已清理node_modules"
)

call :log_success "清理完成"
goto :eof

REM 运行测试
:run_tests
call :log_info "运行测试..."

cd frontend
if exist "package.json" (
    findstr /c:"\"test\"" package.json >nul
    if not errorlevel 1 (
        call npm test
    ) else (
        call :log_warning "前端没有配置测试"
    )
) else (
    call :log_warning "package.json不存在"
)
cd ..

call :log_success "测试完成"
goto :eof

REM 显示帮助信息
:show_help
echo Prism Launcher Modern Architecture Build Script for Windows
echo.
echo 用法: %~nx0 [选项]
echo.
echo 选项:
echo   tauri       构建Tauri后端
echo   frontend    构建前端
echo   app         构建Tauri应用
echo   all         构建所有组件（默认）
echo   clean       清理构建文件
echo   test        运行测试
echo   help        显示此帮助信息
echo.
echo 示例:
echo   %~nx0              # 构建所有组件
echo   %~nx0 tauri        # 只构建Tauri后端
echo   %~nx0 frontend     # 只构建前端
echo   %~nx0 app          # 构建Tauri应用
echo   %~nx0 clean        # 清理构建文件
echo.
echo 注意: 请确保已安装Rust和Node.js
goto :eof

REM 主函数
:main
REM 检查参数
if "%~1"=="" (
    set "BUILD_TARGET=all"
) else (
    set "BUILD_TARGET=%~1"
)

REM 显示构建信息
call :log_info "Prism Launcher Modern Architecture Build Script for Windows"
call :log_info "构建目标: %BUILD_TARGET%"
call :log_info "开始时间: %date% %time%"

REM 检查依赖
call :check_dependencies
if errorlevel 1 exit /b 1

REM 根据目标执行构建
if "%BUILD_TARGET%"=="tauri" (
    call :build_tauri
) else if "%BUILD_TARGET%"=="frontend" (
    call :build_frontend
) else if "%BUILD_TARGET%"=="app" (
    call :build_tauri_app
) else if "%BUILD_TARGET%"=="all" (
    call :build_frontend
    if errorlevel 1 exit /b 1
    call :build_tauri_app
    if errorlevel 1 exit /b 1
) else if "%BUILD_TARGET%"=="clean" (
    call :clean
) else if "%BUILD_TARGET%"=="test" (
    call :run_tests
) else if "%BUILD_TARGET%"=="help" (
    call :show_help
) else (
    call :log_error "未知的构建目标: %BUILD_TARGET%"
    call :show_help
    exit /b 1
)

REM 显示完成信息
call :log_success "构建完成!"
call :log_info "结束时间: %date% %time%"

goto :eof

REM 执行主函数
call :main %*