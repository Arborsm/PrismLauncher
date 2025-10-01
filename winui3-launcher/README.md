# Prism Launcher WinUI 3

这是Prism Launcher的现代Windows UI实现，使用WinUI 3 (Windows App SDK)框架构建，通过native调用访问核心功能。

## 特性

- 🎨 现代Windows 11风格的用户界面
- ⚡ 高性能的native调用桥接
- 🔧 完整的实例管理功能
- 🚀 快速启动和响应
- 📱 响应式设计，支持不同屏幕尺寸
- 🌙 支持深色和浅色主题

## 架构

### 核心组件

1. **WinUI 3 UI层** - 现代Windows用户界面
2. **C++/WinRT桥接层** - 连接UI和核心功能
3. **Prism核心库** - 原有的Qt/C++核心功能

### 技术栈

- **UI框架**: WinUI 3 (Windows App SDK)
- **编程语言**: C++/WinRT, C++
- **构建系统**: CMake
- **核心库**: Prism Launcher (Qt/C++)

## 构建要求

- Windows 10 版本 1903 或更高版本
- Visual Studio 2022 或更高版本
- Windows 11 SDK (版本 10.0.19041.0 或更高)
- CMake 3.20 或更高版本
- Windows App SDK 1.4 或更高版本

## 构建步骤

1. 克隆仓库：
```bash
git clone <repository-url>
cd winui3-launcher
```

2. 配置项目：
```bash
cmake -B build -S .
```

3. 构建项目：
```bash
cmake --build build --config Release
```

4. 运行应用：
```bash
build\\bin\\Release\\PrismLauncherWinUI3.exe
```

## 项目结构

```
winui3-launcher/
├── src/
│   ├── App.xaml                 # 应用程序定义
│   ├── App.xaml.cpp/.h          # 应用程序逻辑
│   ├── MainWindow.xaml          # 主窗口UI
│   ├── MainWindow.xaml.cpp/.h   # 主窗口逻辑
│   ├── bridge/                  # Native调用桥接层
│   │   ├── PrismLauncherBridge.h/.cpp
│   │   ├── InstanceWrapper.h/.cpp
│   │   └── InstanceListWrapper.h/.cpp
│   ├── core/                    # 核心功能包装
│   │   ├── PrismCore.h/.cpp
│   │   └── ...
│   └── main.cpp                 # 程序入口点
├── CMakeLists.txt               # CMake构建配置
├── package.json                 # 项目元数据
└── README.md                    # 项目说明
```

## 功能特性

### 实例管理
- 查看所有Minecraft实例
- 创建新实例
- 编辑实例设置
- 复制和删除实例
- 导出实例

### 启动控制
- 启动/停止实例
- 在线/离线模式
- 服务器连接
- 世界选择

### 用户界面
- 现代化设计
- 响应式布局
- 深色主题支持
- 搜索和筛选
- 分组管理

## 开发说明

### 添加新功能

1. 在`bridge/`目录中添加新的桥接类
2. 在`MainWindow.xaml`中添加UI元素
3. 在`MainWindow.xaml.cpp`中实现事件处理
4. 更新CMakeLists.txt以包含新文件

### 调试

使用Visual Studio调试器：
1. 打开项目文件夹
2. 选择"调试"配置
3. 按F5开始调试

## 贡献

欢迎贡献代码！请确保：

1. 遵循现有的代码风格
2. 添加适当的注释
3. 测试新功能
4. 更新文档

## 许可证

本项目使用GPL-3.0-only许可证，与Prism Launcher保持一致。

## 致谢

- Prism Launcher团队 - 原始核心功能
- Microsoft - WinUI 3框架
- 所有贡献者和社区成员