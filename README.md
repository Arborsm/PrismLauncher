# Prism Launcher - Modern Architecture

这是一个基于PrismLauncher项目的现代化架构重构，包含IPC通信兼容层、无Qt依赖的Core构建，以及使用FluentUI的现代桌面前端。

## 项目结构

```
├── core/                    # 核心后端（无Qt依赖）
│   ├── CMakeLists.txt      # Core构建配置
│   ├── BaseInstance.h/cpp  # 实例管理
│   └── ipc/                # IPC通信层
│       ├── IPCInterface.h   # IPC接口定义
│       ├── IPCManager.h/cpp # IPC管理器
│       ├── IPCService.h/cpp # IPC服务实现
│       └── IPCClient.h/cpp  # IPC客户端
├── frontend/               # FluentUI前端
│   ├── src/
│   │   ├── components/     # React组件
│   │   ├── pages/          # 页面组件
│   │   ├── stores/         # Zustand状态管理
│   │   ├── services/       # IPC服务
│   │   └── types/          # TypeScript类型
│   ├── electron/           # Electron主进程
│   └── package.json        # 前端依赖
└── launcher/               # 原始PrismLauncher代码
```

## 核心功能

### IPC通信兼容层

- **统一接口**: 提供统一的IPC通信接口，支持同步和异步调用
- **服务方法**: 涵盖实例管理、账户管理、Java管理、启动控制、设置管理、模组平台等
- **事件系统**: 支持实时事件通知和状态更新
- **错误处理**: 完善的错误处理和状态管理

### Core构建（无Qt依赖）

- **轻量级**: 移除Qt依赖，减少二进制大小
- **跨平台**: 支持Windows、macOS、Linux
- **模块化**: 清晰的模块分离和接口定义
- **高性能**: 优化的数据结构和算法

### FluentUI前端

- **现代UI**: 使用Microsoft FluentUI设计系统
- **响应式**: 支持不同屏幕尺寸和分辨率
- **主题支持**: 支持明暗主题切换
- **实时更新**: 基于IPC的实时状态同步

## 主要特性

### 实例管理
- 创建、删除、复制Minecraft实例
- 版本管理和更新
- 启动状态监控
- 实例配置管理

### 账户管理
- Microsoft账户认证
- 离线账户支持
- 账户状态管理
- 自动登录

### Java管理
- 自动检测Java安装
- 版本兼容性检查
- 自动下载和安装
- 路径管理

### 模组平台集成
- CurseForge支持
- Modrinth支持
- 本地模组管理
- 依赖解析

### 启动控制
- 游戏启动和停止
- 日志实时显示
- 进程监控
- 错误处理

## 技术栈

### 后端
- **C++20**: 现代C++特性
- **CMake**: 构建系统
- **标准库**: 无外部依赖的核心功能
- **IPC**: 进程间通信

### 前端
- **React 18**: 用户界面框架
- **TypeScript**: 类型安全
- **FluentUI**: Microsoft设计系统
- **Electron**: 桌面应用框架
- **Zustand**: 状态管理
- **Vite**: 构建工具

## 构建说明

### 后端构建

```bash
# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
make -j$(nproc)
```

### 前端构建

```bash
# 安装依赖
npm install

# 开发模式
npm run dev

# 构建
npm run build

# Electron开发
npm run electron-dev

# Electron构建
npm run electron-build
```

## 架构设计

### IPC通信流程

```
Frontend (React/Electron)
    ↓ IPC Request
IPC Client
    ↓ HTTP/WebSocket
IPC Service
    ↓ Core API
Core Backend
    ↓ Response
IPC Service
    ↓ HTTP/WebSocket
IPC Client
    ↓ IPC Response
Frontend (React/Electron)
```

### 数据流

1. **用户操作** → 前端组件
2. **状态更新** → Zustand Store
3. **IPC调用** → IPC Client
4. **服务处理** → IPC Service
5. **核心逻辑** → Core Backend
6. **数据返回** → 前端更新

## 开发指南

### 添加新的IPC方法

1. 在`IPCInterface.h`中定义新的`ServiceMethod`
2. 在`IPCService.cpp`中实现处理方法
3. 在`IPCClient.cpp`中添加客户端方法
4. 在前端`IPCService.ts`中添加TypeScript接口
5. 在相应的Store中添加状态管理

### 添加新的前端页面

1. 在`src/pages/`中创建页面组件
2. 在`src/components/`中创建相关组件
3. 在`src/stores/`中添加状态管理
4. 在`App.tsx`中添加路由
5. 在`Sidebar.tsx`中添加导航项

## 部署

### 后端部署
- 编译为静态库或动态库
- 提供IPC服务接口
- 支持多进程架构

### 前端部署
- 打包为Electron应用
- 支持自动更新
- 跨平台分发

## 贡献

1. Fork项目
2. 创建功能分支
3. 提交更改
4. 推送到分支
5. 创建Pull Request

## 许可证

本项目基于GPL-3.0许可证开源。

## 致谢

- PrismLauncher项目团队
- Microsoft FluentUI团队
- Electron团队
- React团队