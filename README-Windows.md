# Prism Launcher Windows 环境指南

这是专门为Windows环境准备的Prism Launcher现代化架构项目指南。

## 🚀 快速开始

### 1. 环境设置

**自动设置（推荐）**：
```cmd
# 以管理员身份运行命令提示符
setup-windows.bat
```

**手动设置**：
1. 安装 [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) 或 [Build Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)
2. 安装 [Node.js](https://nodejs.org/) (推荐 LTS 版本)
3. 安装 [CMake](https://cmake.org/download/)
4. 安装 [Git](https://git-scm.com/download/win)

### 2. 构建项目

**使用批处理脚本**：
```cmd
# 构建所有组件
build.bat all

# 只构建Core后端
build.bat core

# 只构建前端
build.bat frontend

# 只构建Electron应用
build.bat electron

# 清理构建文件
build.bat clean
```

**使用PowerShell脚本**：
```powershell
# 构建所有组件
.\build.ps1 all

# 只构建Core后端
.\build.ps1 core

# 只构建前端
.\build.ps1 frontend

# 只构建Electron应用
.\build.ps1 electron

# 清理构建文件
.\build.ps1 clean
```

### 3. 开发模式

**启动前端开发服务器**：
```cmd
run-dev.bat
```

**启动Electron应用**：
```cmd
run-electron.bat
```

## 📋 系统要求

### 最低要求
- Windows 10 (版本 1903 或更高)
- 4GB RAM
- 2GB 可用磁盘空间
- 网络连接

### 推荐配置
- Windows 11
- 8GB RAM
- 5GB 可用磁盘空间
- SSD 存储

## 🛠️ 开发环境

### Visual Studio 设置

1. **安装组件**：
   - MSVC v143 - VS 2022 C++ x64/x86 构建工具
   - Windows 10/11 SDK
   - CMake 工具

2. **开发者命令提示符**：
   ```cmd
   # 打开 "Developer Command Prompt for VS 2022"
   # 或 "x64 Native Tools Command Prompt for VS 2022"
   ```

### Node.js 环境

1. **版本要求**：
   - Node.js 18.x 或更高
   - npm 8.x 或更高

2. **验证安装**：
   ```cmd
   node --version
   npm --version
   ```

### CMake 设置

1. **版本要求**：
   - CMake 3.22 或更高

2. **验证安装**：
   ```cmd
   cmake --version
   ```

## 🔧 构建说明

### Core后端构建

```cmd
# 在Developer Command Prompt中运行
cd build\core
cmake ..\..\core -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release --parallel
```

### 前端构建

```cmd
cd frontend
npm install
npm run build
```

### Electron应用构建

```cmd
cd frontend
npm run electron-build
```

## 🐛 常见问题

### 1. CMake配置失败

**问题**：CMake找不到Visual Studio
**解决方案**：
```cmd
# 确保在Developer Command Prompt中运行
# 或手动指定生成器
cmake -G "Visual Studio 17 2022" -A x64
```

### 2. Node.js模块安装失败

**问题**：npm install失败
**解决方案**：
```cmd
# 清理npm缓存
npm cache clean --force

# 删除node_modules重新安装
rmdir /s /q node_modules
npm install
```

### 3. Electron构建失败

**问题**：Electron下载失败
**解决方案**：
```cmd
# 设置Electron镜像
npm config set electron_mirror https://npmmirror.com/mirrors/electron/

# 或使用cnpm
npm install -g cnpm --registry=https://registry.npmmirror.com
cnpm install
```

### 4. 权限问题

**问题**：文件访问被拒绝
**解决方案**：
- 以管理员身份运行命令提示符
- 检查防病毒软件设置
- 确保项目路径不包含特殊字符

## 📁 项目结构

```
├── build.bat              # Windows批处理构建脚本
├── build.ps1              # PowerShell构建脚本
├── setup-windows.bat      # Windows环境设置脚本
├── run-dev.bat            # 开发服务器启动脚本
├── run-electron.bat       # Electron应用启动脚本
├── core/                  # Core后端源码
├── frontend/              # 前端源码
└── README-Windows.md      # Windows环境指南
```

## 🚀 部署选项

### 1. 本地部署

```cmd
# 构建所有组件
build.bat all

# 运行Electron应用
run-electron.bat
```

### 2. 开发服务器

```cmd
# 启动前端开发服务器
run-dev.bat

# 在浏览器中访问 http://localhost:5173
```

### 3. 生产构建

```cmd
# 构建生产版本
build.bat electron

# 输出文件在 frontend\dist-electron\ 目录
```

## 📞 技术支持

如果遇到问题，请：

1. 检查系统要求是否满足
2. 确保所有依赖都已正确安装
3. 查看错误日志和输出信息
4. 尝试清理构建文件重新构建

## 📝 注意事项

1. **管理员权限**：某些操作需要管理员权限
2. **防病毒软件**：可能需要将项目目录添加到白名单
3. **网络连接**：构建过程需要下载依赖包
4. **磁盘空间**：确保有足够的磁盘空间用于构建
5. **路径长度**：避免使用过长的文件路径

## 🎯 下一步

构建完成后，您可以：

1. 运行 `run-electron.bat` 启动桌面应用
2. 运行 `run-dev.bat` 启动开发服务器
3. 查看 `README.md` 了解项目架构
4. 开始自定义和扩展功能