# PrismLauncher IPC兼容层

这是一个将PrismLauncher从Qt依赖转换为IPC通信架构的兼容层实现。

## 架构设计

### 核心组件

1. **IPC通信层** (`ipc/`)
   - `MessageBus` - 消息总线，处理进程间通信
   - `Message` - 消息格式定义
   - `Client` - IPC客户端
   - `Server` - IPC服务端

2. **兼容层** (`compat/`)
   - `String` - 替代QString
   - `List` - 替代QList/QStringList
   - `Map` - 替代QMap
   - `Set` - 替代QSet
   - `Process` - 替代QProcess
   - `Network` - 替代QNetworkAccessManager
   - `Model` - 替代QAbstractListModel
   - `FileWatcher` - 替代QFileSystemWatcher
   - `DateTime` - 替代QDateTime
   - `Settings` - 替代QSettings
   - `Thread` - 替代QThread/QFuture

3. **核心业务逻辑** (`core/`)
   - `Application` - 主应用程序类
   - `Instance` - 实例管理
   - `Launcher` - 启动器逻辑
   - `Minecraft` - Minecraft相关功能
   - `ModPlatform` - 模组平台集成

4. **启动器逻辑** (`launcher/`)
   - 移植的核心启动器功能
   - 去除Qt依赖的业务逻辑

## 通信协议

使用JSON格式的消息进行进程间通信：

```json
{
  "type": "request|response|notification",
  "id": "unique_message_id",
  "method": "method_name",
  "params": {...},
  "result": {...},
  "error": {...}
}
```

## 构建系统

使用CMake构建，不依赖Qt：

```cmake
cmake_minimum_required(VERSION 3.16)
project(PrismLauncherIPC)

# 核心库
add_library(ipc_compat STATIC
  ipc/MessageBus.cpp
  ipc/Client.cpp
  ipc/Server.cpp
  compat/String.cpp
  compat/Process.cpp
  compat/Network.cpp
  # ... 其他源文件
)

# 主程序
add_executable(prismlauncher_ipc
  main.cpp
  core/Application.cpp
  launcher/LauncherCore.cpp
)
```

## 使用方式

1. 启动IPC服务端
2. 客户端通过IPC与服务端通信
3. 服务端处理所有业务逻辑
4. 客户端负责UI展示

## 优势

- 完全去除Qt依赖
- 支持多客户端连接
- 更好的进程隔离
- 易于扩展和维护