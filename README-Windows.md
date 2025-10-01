# Prism Launcher Windows 环境指南 (Tauri版本)

这是专门为Windows环境准备的Prism Launcher现代化架构项目指南，使用Tauri框架构建。

## 🚀 快速开始

### 1. 环境设置

**自动设置（推荐）**：
```cmd
# 以管理员身份运行命令提示符
setup-windows.bat
```

**手动设置**：
1. 安装 [Rust](https://rustup.rs/) (最新稳定版)
2. 安装 [Node.js](https://nodejs.org/) (推荐 LTS 版本)
3. 安装 [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) 或 [Build Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) (Rust需要)
4. 安装 [Git](https://git-scm.com/download/win)

### 2. 构建项目

**使用批处理脚本**：
```cmd
# 构建所有组件
build.bat all

# 只构建Tauri后端
build.bat tauri

# 只构建前端
build.bat frontend

# 构建Tauri应用
build.bat app

# 清理构建文件
build.bat clean
```

**使用PowerShell脚本**：
```powershell
# 构建所有组件
.\build.ps1 all

# 只构建Tauri后端
.\build.ps1 tauri

# 只构建前端
.\build.ps1 frontend

# 构建Tauri应用
.\build.ps1 app

# 清理构建文件
.\build.ps1 clean
```

### 3. 开发模式

**启动Tauri开发服务器**：
```cmd
run-dev.bat
```

**启动Tauri应用**：
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

### Rust 环境

1. **安装Rust**：
   - 访问 [rustup.rs](https://rustup.rs/) 安装Rust
   - 或使用Chocolatey: `choco install rust`

2. **验证安装**：
   ```cmd
   rustc --version
   cargo --version
   ```

### Visual Studio 设置

1. **安装组件**：
   - MSVC v143 - VS 2022 C++ x64/x86 构建工具
   - Windows 10/11 SDK

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

## 🔧 构建说明

### Tauri后端构建

```cmd
cd tauri
npx tauri build
```

### 前端构建

```cmd
cd frontend
npm install
npm run build
```

### Tauri应用构建

```cmd
cd frontend
npm run tauri:build
```

## 🐛 常见问题

### 1. Rust编译失败

**问题**：Rust找不到Visual Studio Build Tools
**解决方案**：
```cmd
# 确保在Developer Command Prompt中运行
# 或安装Visual Studio Build Tools
choco install visualstudio2022buildtools
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

### 3. Tauri构建失败

**问题**：Tauri CLI未安装
**解决方案**：
```cmd
# 安装Tauri CLI
npm install -g @tauri-apps/cli

# 或使用npx
npx tauri build
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
├── run-dev.bat            # Tauri开发服务器启动脚本
├── run-electron.bat       # Tauri应用启动脚本
├── tauri/                 # Tauri后端源码
├── frontend/              # 前端源码
└── README-Windows.md      # Windows环境指南
```

## 🚀 部署选项

### 1. 本地部署

```cmd
# 构建所有组件
build.bat all

# 运行Tauri应用
run-electron.bat
```

### 2. 开发服务器

```cmd
# 启动Tauri开发服务器
run-dev.bat

# 在浏览器中访问 http://localhost:1420
```

### 3. 生产构建

```cmd
# 构建生产版本
build.bat app

# 输出文件在 tauri\target\release\ 目录
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
6. **Rust工具链**：首次构建可能需要下载Rust工具链

## 🎯 下一步

构建完成后，您可以：

1. 运行 `run-electron.bat` 启动Tauri桌面应用
2. 运行 `run-dev.bat` 启动Tauri开发服务器
3. 查看 `README.md` 了解项目架构
4. 开始自定义和扩展功能

## 🔗 Tauri优势

- **轻量级**：比Electron更小的应用体积
- **性能更好**：Rust后端提供更好的性能
- **安全性**：更好的安全模型
- **跨平台**：支持Windows、macOS、Linux
- **现代化**：使用最新的Web技术