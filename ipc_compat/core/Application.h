// SPDX-License-Identifier: GPL-3.0-only
/*
 * Core Application for PrismLauncher IPC
 * Copyright (C) 2024 IPC Compat Layer
 */

#pragma once

#include "../ipc/MessageBus.h"
#include "../compat/String.h"
#include <memory>
#include <map>
#include <vector>
#include <functional>

namespace Core {

// 应用程序状态
enum class ApplicationStatus {
    StartingUp,
    Initialized,
    Running,
    Failed,
    Succeeded
};

// 应用程序能力
enum class ApplicationCapability {
    None = 0,
    SupportsMSA = 1 << 0,
    SupportsFlame = 1 << 1,
    SupportsGameMode = 1 << 2,
    SupportsMangoHud = 1 << 3
};

// 主应用程序类
class Application {
public:
    Application(int argc, char** argv);
    virtual ~Application();
    
    // 应用程序生命周期
    bool initialize();
    int exec();
    void quit();
    
    // 状态管理
    ApplicationStatus status() const { return m_status; }
    void setStatus(ApplicationStatus status) { m_status = status; }
    
    // 能力管理
    int capabilities() const { return m_capabilities; }
    void updateCapabilities();
    
    // 路径管理
    String rootPath() const { return m_rootPath; }
    String dataPath() const { return m_dataPath; }
    void setRootPath(const String& path) { m_rootPath = path; }
    void setDataPath(const String& path) { m_dataPath = path; }
    
    // 便携模式
    bool isPortable() const { return m_portable; }
    void setPortable(bool portable) { m_portable = portable; }
    
    // 时间管理
    int64_t timeSinceStart() const;
    
    // 消息总线
    std::shared_ptr<IPC::MessageBus> messageBus() const { return m_messageBus; }
    
    // 实例管理
    class InstanceManager* instanceManager() const { return m_instanceManager.get(); }
    
    // 启动器管理
    class LauncherManager* launcherManager() const { return m_launcherManager.get(); }
    
    // 网络管理
    class NetworkManager* networkManager() const { return m_networkManager.get(); }
    
    // 设置管理
    class SettingsManager* settingsManager() const { return m_settingsManager.get(); }
    
    // 日志管理
    class LogManager* logManager() const { return m_logManager.get(); }
    
    // 单例访问
    static Application* instance() { return s_instance; }

private:
    void setupMessageHandlers();
    void setupPaths();
    void detectCapabilities();
    bool createManagers();
    
    ApplicationStatus m_status = ApplicationStatus::StartingUp;
    int m_capabilities = 0;
    String m_rootPath;
    String m_dataPath;
    bool m_portable = false;
    std::chrono::system_clock::time_point m_startTime;
    
    std::shared_ptr<IPC::MessageBus> m_messageBus;
    std::unique_ptr<class InstanceManager> m_instanceManager;
    std::unique_ptr<class LauncherManager> m_launcherManager;
    std::unique_ptr<class NetworkManager> m_networkManager;
    std::unique_ptr<class SettingsManager> m_settingsManager;
    std::unique_ptr<class LogManager> m_logManager;
    
    static Application* s_instance;
};

// 实例管理器
class InstanceManager {
public:
    InstanceManager();
    virtual ~InstanceManager();
    
    // 实例操作
    StringList getInstanceList();
    String getInstanceInfo(const String& instanceId);
    bool createInstance(const String& name, const String& type);
    bool deleteInstance(const String& instanceId);
    bool copyInstance(const String& sourceId, const String& newName);
    
    // 实例启动
    bool launchInstance(const String& instanceId, const String& accountId = String());
    bool killInstance(const String& instanceId);
    
    // 实例状态
    bool isInstanceRunning(const String& instanceId);
    String getInstanceStatus(const String& instanceId);

private:
    String m_instancesPath;
    std::map<String, String> m_runningInstances;
};

// 启动器管理器
class LauncherManager {
public:
    LauncherManager();
    virtual ~LauncherManager();
    
    // 启动器操作
    bool launchMinecraft(const String& instanceId, const String& accountId);
    bool killMinecraft(const String& instanceId);
    
    // 游戏状态
    bool isMinecraftRunning(const String& instanceId);
    String getMinecraftStatus(const String& instanceId);
    
    // Java管理
    StringList getJavaInstallations();
    String getDefaultJavaPath();
    bool setDefaultJava(const String& javaPath);

private:
    String m_javaPath;
    std::map<String, String> m_runningGames;
};

// 网络管理器
class NetworkManager {
public:
    NetworkManager();
    virtual ~NetworkManager();
    
    // 下载管理
    bool downloadFile(const String& url, const String& filePath);
    String downloadString(const String& url);
    
    // 模组平台
    StringList getModPlatforms();
    StringList searchMods(const String& platform, const String& query);
    bool installMod(const String& platform, const String& modId, const String& instanceId);

private:
    std::unique_ptr<class Compat::NetworkAccessManager> m_networkAccess;
    std::unique_ptr<class Compat::DownloadManager> m_downloadManager;
};

// 设置管理器
class SettingsManager {
public:
    SettingsManager();
    virtual ~SettingsManager();
    
    // 设置操作
    String getSetting(const String& key, const String& defaultValue = String());
    void setSetting(const String& key, const String& value);
    void removeSetting(const String& key);
    
    // 设置组
    StringList getSettingGroups();
    StringList getSettingsInGroup(const String& group);
    
    // 保存/加载
    bool saveSettings();
    bool loadSettings();

private:
    String m_settingsPath;
    std::map<String, String> m_settings;
};

// 日志管理器
class LogManager {
public:
    LogManager();
    virtual ~LogManager();
    
    // 日志操作
    void log(const String& message, int level = 0);
    void logError(const String& message);
    void logWarning(const String& message);
    void logInfo(const String& message);
    void logDebug(const String& message);
    
    // 日志获取
    StringList getLogs(int maxLines = 1000);
    void clearLogs();
    
    // 日志文件
    void setLogFile(const String& filePath);
    String logFile() const { return m_logFile; }

private:
    String m_logFile;
    StringList m_logs;
    std::mutex m_logMutex;
};

} // namespace Core