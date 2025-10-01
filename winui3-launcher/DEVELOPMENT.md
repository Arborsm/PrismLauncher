# Prism Launcher WinUI 3 开发指南

## 开发环境设置

### 必需软件

1. **Visual Studio 2022**
   - 安装"使用C++的桌面开发"工作负载
   - 安装"通用Windows平台开发"工作负载
   - 确保包含Windows 11 SDK

2. **Windows App SDK**
   - 版本 1.4 或更高
   - 从Microsoft Store或官网下载

3. **CMake**
   - 版本 3.20 或更高
   - 确保在PATH环境变量中

4. **Git**
   - 用于版本控制

### 环境配置

1. **克隆仓库**
```bash
git clone <repository-url>
cd winui3-launcher
```

2. **配置CMake**
```bash
cmake -B build -S . -G "Visual Studio 17 2022" -A x64
```

3. **构建项目**
```bash
cmake --build build --config Debug
```

## 项目结构详解

### 源代码组织

```
src/
├── App.xaml                    # 应用程序XAML定义
├── App.xaml.cpp/.h             # 应用程序C++实现
├── App.idl                     # WinRT接口定义
├── MainWindow.xaml             # 主窗口XAML定义
├── MainWindow.xaml.cpp/.h      # 主窗口C++实现
├── MainWindow.idl              # 主窗口WinRT接口
├── main.cpp                    # 程序入口点
├── pch.h                       # 预编译头文件
├── bridge/                     # 桥接层
│   ├── PrismLauncherBridge.h/.cpp
│   ├── InstanceWrapper.h/.cpp
│   └── InstanceListWrapper.h/.cpp
└── core/                       # 核心包装
    └── PrismCore.h/.cpp
```

### 关键文件说明

#### App.xaml
- 定义应用程序级别的资源
- 设置全局样式和主题
- 配置应用程序属性

#### MainWindow.xaml
- 主窗口的UI布局
- 定义控件和绑定
- 设置窗口属性

#### PrismLauncherBridge
- 主要的桥接类
- 协调UI和核心功能
- 管理事件和状态

## 开发工作流

### 1. 添加新功能

#### 步骤1: 设计接口
```cpp
// 在PrismLauncherBridge.h中添加新方法
bool CreateNewFeature(const std::wstring& parameter);
```

#### 步骤2: 实现功能
```cpp
// 在PrismLauncherBridge.cpp中实现
bool PrismLauncherBridge::CreateNewFeature(const std::wstring& parameter)
{
    // 实现逻辑
    return true;
}
```

#### 步骤3: 添加UI
```xml
<!-- 在MainWindow.xaml中添加控件 -->
<Button x:Name="NewFeatureButton" Content="新功能" Click="NewFeature_Click"/>
```

#### 步骤4: 实现事件处理
```cpp
// 在MainWindow.xaml.cpp中实现
void MainWindow::NewFeature_Click(IInspectable const&, RoutedEventArgs const&)
{
    // 调用桥接方法
    m_bridge->CreateNewFeature(L"参数");
}
```

### 2. 修改现有功能

#### 步骤1: 定位代码
- 使用Visual Studio的"转到定义"功能
- 搜索相关关键字
- 查看调用堆栈

#### 步骤2: 理解逻辑
- 阅读相关文档
- 查看注释和代码
- 理解数据流

#### 步骤3: 进行修改
- 遵循现有代码风格
- 添加适当的注释
- 保持向后兼容性

#### 步骤4: 测试验证
- 运行单元测试
- 进行集成测试
- 验证功能正确性

### 3. 调试技巧

#### 使用Visual Studio调试器
1. 设置断点
2. 启动调试会话
3. 单步执行代码
4. 检查变量值

#### 日志输出
```cpp
// 使用OutputDebugString输出日志
OutputDebugStringA("Debug message\n");
```

#### 异常处理
```cpp
try
{
    // 可能抛出异常的代码
}
catch (const std::exception& e)
{
    // 处理异常
    OutputDebugStringA(e.what());
}
```

## 代码规范

### 命名约定

#### 类名
- 使用PascalCase
- 描述性强
- 避免缩写

```cpp
class InstanceManager
class LaunchController
class SettingsDialog
```

#### 方法名
- 使用PascalCase
- 动词开头
- 描述功能

```cpp
void LaunchInstance()
bool CreateNewInstance()
std::wstring GetInstanceName()
```

#### 变量名
- 使用camelCase
- 描述性强
- 避免单字母变量

```cpp
std::wstring instanceName;
bool isInstanceRunning;
int totalInstanceCount;
```

#### 常量名
- 使用UPPER_CASE
- 下划线分隔
- 描述性强

```cpp
const int MAX_INSTANCE_COUNT = 100;
const std::wstring DEFAULT_INSTANCE_NAME = L"New Instance";
```

### 代码格式

#### 缩进
- 使用4个空格
- 不使用制表符
- 保持一致性

#### 大括号
- 使用Allman风格
- 大括号独占一行
- 保持对齐

```cpp
if (condition)
{
    // 代码
}
else
{
    // 代码
}
```

#### 注释
- 使用//进行单行注释
- 使用/* */进行多行注释
- 注释要清晰有用

```cpp
// 启动指定的实例
bool LaunchInstance(const std::wstring& instanceId)
{
    /* 
     * 这是一个多行注释
     * 用于解释复杂的逻辑
     */
    return true;
}
```

### 错误处理

#### 返回值检查
```cpp
if (!m_bridge)
{
    // 处理错误情况
    return false;
}
```

#### 异常安全
```cpp
try
{
    // 可能失败的操作
    auto result = riskyOperation();
    return result;
}
catch (const std::exception& e)
{
    // 记录错误并返回安全值
    LogError(e.what());
    return DefaultValue();
}
```

## 测试策略

### 单元测试

#### 测试框架
- 使用Google Test框架
- 创建测试项目
- 编写测试用例

#### 测试示例
```cpp
TEST(PrismLauncherBridge, CreateInstance)
{
    PrismLauncherBridge bridge;
    ASSERT_TRUE(bridge.Initialize());
    
    bool result = bridge.CreateInstance(L"Test Instance", L"1.19.2");
    EXPECT_TRUE(result);
}
```

### 集成测试

#### 测试场景
- UI与核心功能集成
- 数据流测试
- 错误处理测试

#### 测试工具
- 使用UI自动化工具
- 模拟用户操作
- 验证结果

### 性能测试

#### 基准测试
- 测量启动时间
- 监控内存使用
- 测试响应时间

#### 压力测试
- 大量实例操作
- 长时间运行
- 资源限制测试

## 构建和部署

### 构建配置

#### Debug配置
- 包含调试信息
- 启用断言
- 优化关闭

#### Release配置
- 优化代码
- 移除调试信息
- 启用所有优化

### 打包部署

#### MSIX包
- 使用Windows App Packaging Project
- 配置应用程序清单
- 设置依赖项

#### 安装程序
- 使用WiX工具集
- 创建安装向导
- 处理依赖项

## 贡献指南

### 提交代码

#### 提交信息格式
```
类型(范围): 简短描述

详细描述

相关Issue: #123
```

#### 类型说明
- feat: 新功能
- fix: 错误修复
- docs: 文档更新
- style: 代码格式
- refactor: 重构
- test: 测试相关
- chore: 构建/工具

### 代码审查

#### 审查要点
- 代码正确性
- 性能影响
- 安全性
- 可维护性

#### 审查流程
1. 创建Pull Request
2. 等待审查
3. 根据反馈修改
4. 合并代码

### 问题报告

#### 报告格式
- 问题描述
- 复现步骤
- 预期行为
- 实际行为
- 环境信息

#### 优先级
- Critical: 崩溃或数据丢失
- High: 主要功能不可用
- Medium: 功能受限
- Low: 轻微问题

## 资源链接

### 官方文档
- [WinUI 3 文档](https://docs.microsoft.com/en-us/windows/apps/winui/winui3/)
- [C++/WinRT 文档](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/)
- [Windows App SDK 文档](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/)

### 开发工具
- [Visual Studio](https://visualstudio.microsoft.com/)
- [CMake](https://cmake.org/)
- [Git](https://git-scm.com/)

### 社区资源
- [Prism Launcher GitHub](https://github.com/PrismLauncher/PrismLauncher)
- [Discord 社区](https://prismlauncher.org/discord)
- [Matrix 空间](https://prismlauncher.org/matrix)