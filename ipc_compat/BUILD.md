# 构建指南

## 系统要求

### 最低要求
- Linux (Ubuntu 20.04+ / CentOS 8+ / Debian 11+)
- CMake 3.16+
- GCC 9+ 或 Clang 10+
- C++17 支持

### 推荐配置
- Linux (Ubuntu 22.04+)
- CMake 3.20+
- GCC 11+ 或 Clang 13+
- 4GB+ RAM
- 1GB+ 磁盘空间

## 依赖项

### 系统依赖
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake git pkg-config

# CentOS/RHEL/Fedora
sudo dnf install gcc-c++ cmake git pkgconfig

# Arch Linux
sudo pacman -S base-devel cmake git pkgconf
```

### 运行时依赖
- POSIX线程库 (pthread)
- 动态链接器 (dl)
- 文件系统支持

## 构建步骤

### 1. 克隆源码
```bash
git clone <repository-url>
cd ipc_compat
```

### 2. 创建构建目录
```bash
mkdir build
cd build
```

### 3. 配置构建
```bash
# 基本配置
cmake ..

# 调试构建
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 发布构建
cmake -DCMAKE_BUILD_TYPE=Release ..

# 自定义安装路径
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

### 4. 编译
```bash
# 并行编译
make -j$(nproc)

# 或者使用ninja
ninja
```

### 5. 安装
```bash
# 安装到系统
sudo make install

# 或者安装到用户目录
make install DESTDIR=$HOME/.local
```

## 构建选项

### CMake选项
```bash
# 构建类型
-DCMAKE_BUILD_TYPE=Release|Debug|RelWithDebInfo|MinSizeRel

# 安装路径
-DCMAKE_INSTALL_PREFIX=/usr/local

# 编译器选项
-DCMAKE_CXX_COMPILER=g++
-DCMAKE_C_COMPILER=gcc

# 链接选项
-DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++"
```

### 功能选项
```bash
# 启用测试
-DBUILD_TESTS=ON

# 启用示例
-DBUILD_EXAMPLES=ON

# 启用文档
-DBUILD_DOCS=ON

# 启用静态链接
-DBUILD_STATIC=ON
```

## 交叉编译

### 目标架构
```bash
# ARM64
cmake -DCMAKE_SYSTEM_NAME=Linux \
      -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
      -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
      -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ ..

# ARM32
cmake -DCMAKE_SYSTEM_NAME=Linux \
      -DCMAKE_SYSTEM_PROCESSOR=arm \
      -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
      -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ ..
```

## 测试

### 运行测试
```bash
# 构建测试
make test_string test_message

# 运行测试
./test_string
./test_message

# 使用CTest
ctest -V
```

### 测试覆盖率
```bash
# 安装gcov
sudo apt install gcov

# 编译时启用覆盖率
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" ..

# 运行测试
make test_string test_message
./test_string
./test_message

# 生成覆盖率报告
gcov test_string.cpp
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## 打包

### 创建包
```bash
# 创建tar包
make package_source

# 创建DEB包
cpack -G DEB

# 创建RPM包
cpack -G RPM

# 创建TGZ包
cpack -G TGZ
```

### 安装包
```bash
# 安装DEB包
sudo dpkg -i prismlauncher-ipc_1.0.0_amd64.deb

# 安装RPM包
sudo rpm -i prismlauncher-ipc-1.0.0.x86_64.rpm
```

## 故障排除

### 常见问题

#### 1. 编译错误
```bash
# 检查编译器版本
gcc --version
g++ --version

# 检查CMake版本
cmake --version

# 清理构建目录
rm -rf build
mkdir build
cd build
cmake ..
```

#### 2. 链接错误
```bash
# 检查库依赖
ldd prismlauncher_ipc

# 安装缺失的库
sudo apt install libc6-dev

# 检查链接器
ld --version
```

#### 3. 运行时错误
```bash
# 检查权限
ls -la prismlauncher_ipc

# 检查依赖
ldd prismlauncher_ipc

# 调试运行
gdb ./prismlauncher_ipc
```

### 调试选项
```bash
# 启用详细输出
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..

# 启用调试信息
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0" ..

# 启用地址消毒器
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address -g" ..
```

## 性能优化

### 编译优化
```bash
# 优化级别
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native" ..

# 链接时优化
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-flto" ..
```

### 运行时优化
```bash
# 设置环境变量
export OMP_NUM_THREADS=4
export MALLOC_ARENA_MAX=2

# 使用性能分析
perf record ./prismlauncher_ipc
perf report
```

## 持续集成

### GitHub Actions
```yaml
name: Build and Test
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    - name: Install dependencies
      run: |
        sudo apt update
        sudo apt install build-essential cmake
    - name: Build
      run: |
        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Release ..
        make -j$(nproc)
    - name: Test
      run: |
        cd build
        ctest -V
```

### Docker构建
```dockerfile
FROM ubuntu:22.04
RUN apt update && apt install -y build-essential cmake git
COPY . /src
WORKDIR /src
RUN mkdir build && cd build && cmake .. && make -j$(nproc)
CMD ["./build/prismlauncher_ipc"]
```