# Prism Launcher WinUI 3 - 快速开始

## 🚀 5分钟快速体验

### 1. 环境准备

确保您的系统满足以下要求：
- Windows 10 版本 1903 或更高
- Visual Studio 2022 (Community/Professional/Enterprise)
- Windows 11 SDK (版本 10.0.19041.0 或更高)
- CMake 3.20 或更高版本

### 2. 获取代码

```bash
# 克隆项目
git clone <repository-url>
cd winui3-launcher

# 或者下载ZIP文件并解压
```

### 3. 构建项目

#### 方法一：使用构建脚本（推荐）
```bash
# 双击运行
build.bat

# 或者在命令行中运行
.\build.bat
```

#### 方法二：手动构建
```bash
# 配置项目
cmake -B build -S . -G "Visual Studio 17 2022" -A x64

# 构建项目
cmake --build build --config Release
```

### 4. 运行应用

```bash
# 运行应用程序
build\bin\Release\PrismLauncherWinUI3.exe
```

## 🎯 主要功能演示

### 查看实例列表
1. 启动应用程序
2. 在左侧面板查看所有Minecraft实例
3. 使用搜索框快速查找特定实例
4. 使用筛选器按状态或分组查看实例

### 创建新实例
1. 点击"添加实例"按钮
2. 在对话框中输入实例名称
3. 选择Minecraft版本
4. 配置其他选项
5. 点击"创建"完成

### 启动游戏
1. 在实例列表中选择要启动的实例
2. 点击"启动"按钮
3. 等待游戏加载完成
4. 享受游戏！

### 管理实例
- **编辑**: 修改实例设置和配置
- **复制**: 快速创建实例副本
- **删除**: 安全删除不需要的实例
- **导出**: 导出实例为文件

## 🎨 界面特色

### 现代化设计
- **Fluent UI风格**: 符合Windows 11设计规范
- **深色主题**: 护眼的深色界面
- **流畅动画**: 60fps的流畅过渡效果
- **响应式布局**: 适配不同屏幕尺寸

### 直观操作
- **卡片式设计**: 信息层次清晰
- **状态指示器**: 直观显示实例状态
- **快速操作**: 一键启动、编辑、删除
- **实时反馈**: 操作状态实时更新

## 🔧 常见问题

### Q: 构建失败怎么办？
A: 请检查：
1. Visual Studio 2022是否正确安装
2. Windows 11 SDK是否已安装
3. CMake版本是否满足要求
4. 网络连接是否正常（用于下载依赖）

### Q: 应用无法启动？
A: 请检查：
1. 系统版本是否满足要求
2. Windows App SDK是否正确安装
3. 是否有足够的系统权限
4. 防病毒软件是否阻止运行

### Q: 实例无法启动？
A: 请检查：
1. Java是否正确安装
2. 实例配置是否正确
3. 网络连接是否正常
4. 查看错误日志获取详细信息

### Q: 界面显示异常？
A: 请检查：
1. 系统DPI设置
2. 显卡驱动是否最新
3. 是否启用了高对比度模式
4. 尝试重启应用程序

## 📚 更多资源

### 文档
- [完整使用说明](USAGE.md)
- [架构设计文档](ARCHITECTURE.md)
- [开发指南](DEVELOPMENT.md)
- [项目总结](PROJECT_SUMMARY.md)

### 社区
- [GitHub仓库](https://github.com/PrismLauncher/PrismLauncher)
- [Discord社区](https://prismlauncher.org/discord)
- [问题反馈](https://github.com/PrismLauncher/PrismLauncher/issues)
- [官方网站](https://prismlauncher.org)

### 支持
- 查看[常见问题](https://prismlauncher.org/wiki/faq)
- 阅读[故障排除指南](https://prismlauncher.org/wiki/troubleshooting)
- 提交[问题报告](https://github.com/PrismLauncher/PrismLauncher/issues/new)
- 参与[社区讨论](https://prismlauncher.org/discord)

## 🎉 开始使用

现在您已经了解了基本操作，可以开始使用Prism Launcher WinUI 3了！

1. **创建您的第一个实例**
2. **导入现有的Minecraft实例**
3. **安装喜欢的模组**
4. **享受现代化的游戏体验**

---

**提示**: 如果您是第一次使用，建议先阅读[完整使用说明](USAGE.md)了解更多高级功能。

**需要帮助？** 随时访问我们的[社区](https://prismlauncher.org/discord)获取支持！