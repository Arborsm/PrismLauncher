# Prism Launcher Modern Architecture Build Script for Windows PowerShell
# PowerShell构建脚本，支持更高级的功能

param(
    [Parameter(Position=0)]
    [ValidateSet("tauri", "frontend", "app", "all", "clean", "test", "help")]
    [string]$Target = "all"
)

# 颜色定义
$Colors = @{
    Red = "Red"
    Green = "Green"
    Yellow = "Yellow"
    Blue = "Blue"
    White = "White"
}

# 日志函数
function Write-LogInfo {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor $Colors.Blue
}

function Write-LogSuccess {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor $Colors.Green
}

function Write-LogWarning {
    param([string]$Message)
    Write-Host "[WARNING] $Message" -ForegroundColor $Colors.Yellow
}

function Write-LogError {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor $Colors.Red
}

# 检查依赖
function Test-Dependencies {
    Write-LogInfo "检查构建依赖..."
    
    # 检查Rust
    try {
        $rustVersion = & rustc --version 2>$null
        if ($LASTEXITCODE -ne 0) {
            throw "Rust not found"
        }
        Write-LogInfo "Rust: $rustVersion"
    }
    catch {
        Write-LogError "Rust未安装，请先安装Rust"
        Write-LogInfo "访问 https://rustup.rs/ 安装Rust"
        exit 1
    }
    
    # 检查Cargo
    try {
        $cargoVersion = & cargo --version 2>$null
        if ($LASTEXITCODE -ne 0) {
            throw "Cargo not found"
        }
        Write-LogInfo "Cargo: $cargoVersion"
    }
    catch {
        Write-LogError "Cargo未安装，请先安装Cargo"
        exit 1
    }
    
    # 检查Node.js
    try {
        $nodeVersion = & node --version 2>$null
        if ($LASTEXITCODE -ne 0) {
            throw "Node.js not found"
        }
        Write-LogInfo "Node.js: $nodeVersion"
    }
    catch {
        Write-LogError "Node.js未安装，请先安装Node.js"
        exit 1
    }
    
    # 检查npm
    try {
        $npmVersion = & npm --version 2>$null
        if ($LASTEXITCODE -ne 0) {
            throw "npm not found"
        }
        Write-LogInfo "npm: $npmVersion"
    }
    catch {
        Write-LogError "npm未安装，请先安装npm"
        exit 1
    }
    
    # 检查Tauri CLI
    try {
        $tauriVersion = & npx tauri --version 2>$null
        if ($LASTEXITCODE -ne 0) {
            Write-LogWarning "Tauri CLI未安装，将自动安装"
        } else {
            Write-LogInfo "Tauri CLI: $tauriVersion"
        }
    }
    catch {
        Write-LogWarning "Tauri CLI未安装，将自动安装"
    }
    
    Write-LogSuccess "所有依赖检查通过"
}

# 构建Tauri后端
function Build-Tauri {
    Write-LogInfo "开始构建Tauri后端..."
    
    Push-Location "tauri"
    
    try {
        # 安装Tauri CLI（如果需要）
        Write-LogInfo "检查Tauri CLI..."
        $tauriCheck = & npx tauri --version 2>$null
        if ($LASTEXITCODE -ne 0) {
            Write-LogInfo "安装Tauri CLI..."
            & npm install -g @tauri-apps/cli
            if ($LASTEXITCODE -ne 0) {
                throw "Tauri CLI安装失败"
            }
        }
        
        # 构建Tauri应用
        Write-LogInfo "构建Tauri应用..."
        & npx tauri build
        if ($LASTEXITCODE -ne 0) {
            throw "Tauri构建失败"
        }
        
        Write-LogSuccess "Tauri后端构建完成"
    }
    catch {
        Write-LogError "Tauri后端构建失败: $_"
        exit 1
    }
    finally {
        Pop-Location
    }
}

# 构建前端
function Build-Frontend {
    Write-LogInfo "开始构建前端..."
    
    Push-Location "frontend"
    
    try {
        # 安装依赖
        Write-LogInfo "安装前端依赖..."
        & npm install
        if ($LASTEXITCODE -ne 0) {
            throw "npm install失败"
        }
        
        # 构建前端
        Write-LogInfo "构建前端应用..."
        & npm run build
        if ($LASTEXITCODE -ne 0) {
            throw "前端构建失败"
        }
        
        Write-LogSuccess "前端构建完成"
    }
    catch {
        Write-LogError "前端构建失败: $_"
        exit 1
    }
    finally {
        Pop-Location
    }
}

# 构建Tauri应用
function Build-TauriApp {
    Write-LogInfo "开始构建Tauri应用..."
    
    Push-Location "frontend"
    
    try {
        # 安装依赖
        Write-LogInfo "安装前端依赖..."
        & npm install
        if ($LASTEXITCODE -ne 0) {
            throw "前端依赖安装失败"
        }
        
        # 构建前端
        Write-LogInfo "构建前端..."
        & npm run build
        if ($LASTEXITCODE -ne 0) {
            throw "前端构建失败"
        }
        
        Write-LogSuccess "前端构建完成"
    }
    catch {
        Write-LogError "前端构建失败: $_"
        exit 1
    }
    finally {
        Pop-Location
    }
    
    # 构建Tauri应用
    Write-LogInfo "构建Tauri应用..."
    Push-Location "tauri"
    
    try {
        & npx tauri build
        if ($LASTEXITCODE -ne 0) {
            throw "Tauri应用构建失败"
        }
        
        Write-LogSuccess "Tauri应用构建完成"
    }
    catch {
        Write-LogError "Tauri应用构建失败: $_"
        exit 1
    }
    finally {
        Pop-Location
    }
}

# 清理构建文件
function Clear-Build {
    Write-LogInfo "清理构建文件..."
    
    # 清理Tauri构建
    if (Test-Path "tauri\target") {
        Remove-Item -Path "tauri\target" -Recurse -Force
        Write-LogInfo "已清理Tauri构建文件"
    }
    
    # 清理前端构建
    if (Test-Path "frontend\dist") {
        Remove-Item -Path "frontend\dist" -Recurse -Force
        Write-LogInfo "已清理前端构建文件"
    }
    
    # 清理node_modules
    if (Test-Path "frontend\node_modules") {
        Remove-Item -Path "frontend\node_modules" -Recurse -Force
        Write-LogInfo "已清理node_modules"
    }
    
    Write-LogSuccess "清理完成"
}

# 运行测试
function Invoke-Tests {
    Write-LogInfo "运行测试..."
    
    Push-Location "frontend"
    
    try {
        if (Test-Path "package.json") {
            $packageJson = Get-Content "package.json" | ConvertFrom-Json
            if ($packageJson.scripts.test) {
                & npm test
                if ($LASTEXITCODE -ne 0) {
                    throw "测试失败"
                }
            } else {
                Write-LogWarning "前端没有配置测试"
            }
        } else {
            Write-LogWarning "package.json不存在"
        }
        
        Write-LogSuccess "测试完成"
    }
    catch {
        Write-LogError "测试失败: $_"
        exit 1
    }
    finally {
        Pop-Location
    }
}

# 显示帮助信息
function Show-Help {
    Write-Host "Prism Launcher Modern Architecture Build Script for Windows PowerShell" -ForegroundColor $Colors.White
    Write-Host ""
    Write-Host "用法: .\build.ps1 [选项]" -ForegroundColor $Colors.White
    Write-Host ""
    Write-Host "选项:" -ForegroundColor $Colors.White
    Write-Host "  tauri       构建Tauri后端" -ForegroundColor $Colors.White
    Write-Host "  frontend    构建前端" -ForegroundColor $Colors.White
    Write-Host "  app         构建Tauri应用" -ForegroundColor $Colors.White
    Write-Host "  all         构建所有组件（默认）" -ForegroundColor $Colors.White
    Write-Host "  clean       清理构建文件" -ForegroundColor $Colors.White
    Write-Host "  test        运行测试" -ForegroundColor $Colors.White
    Write-Host "  help        显示此帮助信息" -ForegroundColor $Colors.White
    Write-Host ""
    Write-Host "示例:" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1              # 构建所有组件" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1 tauri       # 只构建Tauri后端" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1 frontend   # 只构建前端" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1 app         # 构建Tauri应用" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1 clean       # 清理构建文件" -ForegroundColor $Colors.White
    Write-Host ""
    Write-Host "注意: 请确保已安装Rust和Node.js" -ForegroundColor $Colors.Yellow
}

# 主函数
function Main {
    # 显示构建信息
    Write-LogInfo "Prism Launcher Modern Architecture Build Script for Windows PowerShell"
    Write-LogInfo "构建目标: $Target"
    Write-LogInfo "开始时间: $(Get-Date)"
    
    # 检查依赖
    Test-Dependencies
    
    # 根据目标执行构建
    switch ($Target) {
        "tauri" {
            Build-Tauri
        }
        "frontend" {
            Build-Frontend
        }
        "app" {
            Build-TauriApp
        }
        "all" {
            Build-Frontend
            Build-TauriApp
        }
        "clean" {
            Clear-Build
        }
        "test" {
            Invoke-Tests
        }
        "help" {
            Show-Help
            return
        }
        default {
            Write-LogError "未知的构建目标: $Target"
            Show-Help
            exit 1
        }
    }
    
    # 显示完成信息
    Write-LogSuccess "构建完成!"
    Write-LogInfo "结束时间: $(Get-Date)"
}

# 执行主函数
Main