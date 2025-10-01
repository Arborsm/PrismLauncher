#!/bin/bash

# Prism Launcher Modern Architecture Build Script
# 构建脚本用于编译Core后端和前端

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查依赖
check_dependencies() {
    log_info "检查构建依赖..."
    
    # 检查CMake
    if ! command -v cmake &> /dev/null; then
        log_error "CMake未安装，请先安装CMake"
        exit 1
    fi
    
    # 检查Node.js
    if ! command -v node &> /dev/null; then
        log_error "Node.js未安装，请先安装Node.js"
        exit 1
    fi
    
    # 检查npm
    if ! command -v npm &> /dev/null; then
        log_error "npm未安装，请先安装npm"
        exit 1
    fi
    
    log_success "所有依赖检查通过"
}

# 构建Core后端
build_core() {
    log_info "开始构建Core后端..."
    
    # 创建构建目录
    mkdir -p build/core
    cd build/core
    
    # 配置CMake
    log_info "配置CMake..."
    cmake ../../core -DCMAKE_BUILD_TYPE=Release
    
    # 编译
    log_info "编译Core后端..."
    make -j$(nproc)
    
    cd ../..
    log_success "Core后端构建完成"
}

# 构建前端
build_frontend() {
    log_info "开始构建前端..."
    
    cd frontend
    
    # 安装依赖
    log_info "安装前端依赖..."
    npm install
    
    # 构建前端
    log_info "构建前端应用..."
    npm run build
    
    cd ..
    log_success "前端构建完成"
}

# 构建Electron应用
build_electron() {
    log_info "开始构建Electron应用..."
    
    cd frontend
    
    # 构建Electron应用
    log_info "构建Electron应用..."
    npm run electron-build
    
    cd ..
    log_success "Electron应用构建完成"
}

# 清理构建文件
clean() {
    log_info "清理构建文件..."
    
    # 清理Core构建
    if [ -d "build" ]; then
        rm -rf build
        log_info "已清理Core构建文件"
    fi
    
    # 清理前端构建
    if [ -d "frontend/dist" ]; then
        rm -rf frontend/dist
        log_info "已清理前端构建文件"
    fi
    
    if [ -d "frontend/dist-electron" ]; then
        rm -rf frontend/dist-electron
        log_info "已清理Electron构建文件"
    fi
    
    # 清理node_modules
    if [ -d "frontend/node_modules" ]; then
        rm -rf frontend/node_modules
        log_info "已清理node_modules"
    fi
    
    log_success "清理完成"
}

# 运行测试
run_tests() {
    log_info "运行测试..."
    
    # 运行前端测试
    cd frontend
    if [ -f "package.json" ] && grep -q '"test"' package.json; then
        npm test
    else
        log_warning "前端没有配置测试"
    fi
    cd ..
    
    log_success "测试完成"
}

# 显示帮助信息
show_help() {
    echo "Prism Launcher Modern Architecture Build Script"
    echo ""
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  core        构建Core后端"
    echo "  frontend    构建前端"
    echo "  electron    构建Electron应用"
    echo "  all         构建所有组件（默认）"
    echo "  clean       清理构建文件"
    echo "  test        运行测试"
    echo "  help        显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0              # 构建所有组件"
    echo "  $0 core         # 只构建Core后端"
    echo "  $0 frontend     # 只构建前端"
    echo "  $0 clean        # 清理构建文件"
}

# 主函数
main() {
    # 检查参数
    if [ $# -eq 0 ]; then
        BUILD_TARGET="all"
    else
        BUILD_TARGET=$1
    fi
    
    # 显示构建信息
    log_info "Prism Launcher Modern Architecture Build Script"
    log_info "构建目标: $BUILD_TARGET"
    log_info "开始时间: $(date)"
    
    # 检查依赖
    check_dependencies
    
    # 根据目标执行构建
    case $BUILD_TARGET in
        "core")
            build_core
            ;;
        "frontend")
            build_frontend
            ;;
        "electron")
            build_electron
            ;;
        "all")
            build_core
            build_frontend
            build_electron
            ;;
        "clean")
            clean
            ;;
        "test")
            run_tests
            ;;
        "help")
            show_help
            ;;
        *)
            log_error "未知的构建目标: $BUILD_TARGET"
            show_help
            exit 1
            ;;
    esac
    
    # 显示完成信息
    log_success "构建完成!"
    log_info "结束时间: $(date)"
}

# 执行主函数
main "$@"