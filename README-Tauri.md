# Prism Launcher - Tauri现代化架构

这是一个基于Tauri框架的现代化Minecraft启动器项目，使用Rust后端和React前端构建。

## 🏗️ 架构概述

### 技术栈
- **后端**: Rust + Tauri
- **前端**: React + TypeScript + FluentUI
- **状态管理**: Zustand
- **构建工具**: Vite + Cargo
- **包管理**: npm + Cargo

### 项目结构
```
├── tauri/                 # Tauri后端源码
│   ├── src/              # Rust源码
│   │   ├── main.rs       # 主入口
│   │   ├── ipc.rs        # IPC通信
│   │   ├── instance.rs   # 实例管理
│   │   ├── account.rs    # 账户管理
│   │   ├── java.rs       # Java管理
│   │   ├── system.rs     # 系统信息
│   │   ├── settings.rs   # 设置管理
│   │   └── mod_platform.rs # 模组平台
│   ├── Cargo.toml        # Rust依赖配置
│   └── tauri.conf.json   # Tauri配置
├── frontend/             # 前端源码
│   ├── src/              # React源码
│   │   ├── components/   # 组件
│   │   ├── services/     # 服务层
│   │   ├── stores/       # 状态管理
│   │   └── types/        # 类型定义
│   ├── package.json      # Node.js依赖
│   └── vite.config.ts    # Vite配置
├── build.bat             # Windows构建脚本
├── build.ps1             # PowerShell构建脚本
└── README-Tauri.md       # 本文档
```

## 🚀 快速开始

### 1. 环境要求
- Rust 1.70+
- Node.js 18+
- Visual Studio Build Tools (Windows)
- Git

### 2. 安装依赖
```bash
# 安装Rust
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# 安装Node.js依赖
cd frontend
npm install
```

### 3. 开发模式
```bash
# 启动Tauri开发服务器
cd frontend
npm run tauri:dev
```

### 4. 构建应用
```bash
# 构建生产版本
cd frontend
npm run tauri:build
```

## 🔧 核心功能

### 实例管理
- 创建、删除、启动、停止Minecraft实例
- 支持多版本Minecraft
- 实例配置管理

### 账户管理
- Microsoft账户登录
- 离线账户支持
- 账户信息管理

### Java管理
- 自动检测Java安装
- Java版本管理
- 兼容性检查

### 模组平台集成
- CurseForge集成
- Modrinth集成
- 本地模组管理

### 设置管理
- 主题设置（深色/浅色）
- 语言设置
- 性能设置
- 自定义设置

## 📡 IPC通信

### 通信架构
```
Frontend (React) ←→ Tauri IPC ←→ Backend (Rust)
```

### 消息类型
- **Request**: 前端向后端发送请求
- **Response**: 后端向前端返回响应
- **Notification**: 后端向前端发送通知
- **Error**: 错误消息

### 服务方法
- 实例管理: `CreateInstance`, `DeleteInstance`, `LaunchInstance`
- 账户管理: `AddAccount`, `RemoveAccount`, `LoginAccount`
- Java管理: `ListJavaInstalls`, `InstallJava`
- 设置管理: `GetSettings`, `SetSettings`
- 系统信息: `GetSystemInfo`

## 🎨 前端架构

### 组件结构
```
App
├── MainLayout
│   ├── Header
│   ├── Sidebar
│   │   └── Navigation
│   └── Content
│       ├── InstanceList
│       │   └── InstanceCard
│       ├── AccountList
│       ├── JavaList
│       └── Settings
└── Toast (通知)
```

### 状态管理
使用Zustand进行状态管理：
- `appStore`: 应用全局状态
- `instanceStore`: 实例管理状态
- `accountStore`: 账户管理状态
- `javaStore`: Java管理状态
- `settingsStore`: 设置管理状态

### 服务层
- `TauriService`: Tauri IPC服务封装
- `IPCService`: 通用IPC服务接口

## 🔒 安全特性

### Tauri安全模型
- 进程隔离
- 权限控制
- 安全IPC通信
- 代码签名支持

### 数据安全
- 本地数据加密
- 安全存储
- 权限验证

## 🚀 性能优化

### Rust后端优势
- 零成本抽象
- 内存安全
- 高性能
- 并发安全

### 前端优化
- 代码分割
- 懒加载
- 虚拟滚动
- 缓存策略

## 📦 构建和部署

### 开发构建
```bash
# 前端开发服务器
npm run dev

# Tauri开发模式
npm run tauri:dev
```

### 生产构建
```bash
# 构建前端
npm run build

# 构建Tauri应用
npm run tauri:build
```

### 跨平台支持
- Windows: `.exe` 安装包
- macOS: `.dmg` 安装包
- Linux: `.deb`, `.rpm`, `.AppImage`

## 🧪 测试

### 后端测试
```bash
cd tauri
cargo test
```

### 前端测试
```bash
cd frontend
npm test
```

### 集成测试
```bash
npm run test:integration
```

## 📚 开发指南

### 添加新功能
1. 在Rust后端定义新的IPC方法
2. 在前端服务层添加对应方法
3. 创建UI组件
4. 更新状态管理
5. 添加测试

### 代码规范
- Rust: 使用`rustfmt`和`clippy`
- TypeScript: 使用ESLint和Prettier
- 提交信息: 使用Conventional Commits

### 调试
- 使用Tauri DevTools
- 查看控制台日志
- 使用Rust调试器

## 🤝 贡献指南

1. Fork项目
2. 创建功能分支
3. 提交更改
4. 创建Pull Request

## 📄 许可证

本项目使用GPL-3.0许可证。

## 🔗 相关链接

- [Tauri文档](https://tauri.app/)
- [Rust文档](https://doc.rust-lang.org/)
- [React文档](https://react.dev/)
- [FluentUI文档](https://react.fluentui.dev/)