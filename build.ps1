# Prism Launcher Modern Architecture Build Script for Windows PowerShell
# PowerShell构建脚本，支持更高级的功能

param(
    [Parameter(Position=0)]
    [ValidateSet("core", "frontend", "electron", "all", "clean", "test", "help")]
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
    
    # 检查CMake
    try {
        $cmakeVersion = & cmake --version 2>$null | Select-Object -First 1
        if ($LASTEXITCODE -ne 0) {
            throw "CMake not found"
        }
        Write-LogInfo "CMake: $cmakeVersion"
    }
    catch {
        Write-LogError "CMake未安装，请先安装CMake"
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
    
    # 检查Visual Studio Build Tools
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $vsInstall = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 2>$null
        if ($vsInstall) {
            Write-LogInfo "Visual Studio Build Tools: 已安装"
        } else {
            Write-LogWarning "Visual Studio Build Tools未找到，请确保已安装"
        }
    } else {
        Write-LogWarning "Visual Studio Installer未找到"
    }
    
    Write-LogSuccess "所有依赖检查通过"
}

# 构建Core后端
function Build-Core {
    Write-LogInfo "开始构建Core后端..."
    
    # 创建构建目录
    $buildDir = "build\core"
    if (!(Test-Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
    }
    
    Push-Location $buildDir
    
    try {
        # 配置CMake
        Write-LogInfo "配置CMake..."
        & cmake ..\..\core -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64
        if ($LASTEXITCODE -ne 0) {
            throw "CMake配置失败"
        }
        
        # 编译
        Write-LogInfo "编译Core后端..."
        & cmake --build . --config Release --parallel
        if ($LASTEXITCODE -ne 0) {
            throw "编译失败"
        }
        
        Write-LogSuccess "Core后端构建完成"
    }
    catch {
        Write-LogError "Core后端构建失败: $_"
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

# 构建Electron应用
function Build-Electron {
    Write-LogInfo "开始构建Electron应用..."
    
    Push-Location "frontend"
    
    try {
        # 构建Electron应用
        Write-LogInfo "构建Electron应用..."
        & npm run electron-build
        if ($LASTEXITCODE -ne 0) {
            throw "Electron构建失败"
        }
        
        Write-LogSuccess "Electron应用构建完成"
    }
    catch {
        Write-LogError "Electron构建失败: $_"
        exit 1
    }
    finally {
        Pop-Location
    }
}

# 清理构建文件
function Clear-Build {
    Write-LogInfo "清理构建文件..."
    
    # 清理Core构建
    if (Test-Path "build") {
        Remove-Item -Path "build" -Recurse -Force
        Write-LogInfo "已清理Core构建文件"
    }
    
    # 清理前端构建
    if (Test-Path "frontend\dist") {
        Remove-Item -Path "frontend\dist" -Recurse -Force
        Write-LogInfo "已清理前端构建文件"
    }
    
    if (Test-Path "frontend\dist-electron") {
        Remove-Item -Path "frontend\dist-electron" -Recurse -Force
        Write-LogInfo "已清理Electron构建文件"
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
    Write-Host "  core        构建Core后端" -ForegroundColor $Colors.White
    Write-Host "  frontend    构建前端" -ForegroundColor $Colors.White
    Write-Host "  electron    构建Electron应用" -ForegroundColor $Colors.White
    Write-Host "  all         构建所有组件（默认）" -ForegroundColor $Colors.White
    Write-Host "  clean       清理构建文件" -ForegroundColor $Colors.White
    Write-Host "  test        运行测试" -ForegroundColor $Colors.White
    Write-Host "  help        显示此帮助信息" -ForegroundColor $Colors.White
    Write-Host ""
    Write-Host "示例:" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1              # 构建所有组件" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1 core         # 只构建Core后端" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1 frontend    # 只构建前端" -ForegroundColor $Colors.White
    Write-Host "  .\build.ps1 clean       # 清理构建文件" -ForegroundColor $Colors.White
    Write-Host ""
    Write-Host "注意: 请确保在PowerShell中运行此脚本" -ForegroundColor $Colors.Yellow
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
        "core" {
            Build-Core
        }
        "frontend" {
            Build-Frontend
        }
        "electron" {
            Build-Electron
        }
        "all" {
            Build-Core
            Build-Frontend
            Build-Electron
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