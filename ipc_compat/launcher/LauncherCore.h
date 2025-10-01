// SPDX-License-Identifier: GPL-3.0-only
/*
 * Launcher Core Logic for PrismLauncher IPC
 * Copyright (C) 2024 IPC Compat Layer
 */

#pragma once

#include "../core/Application.h"
#include "../compat/String.h"
#include "../compat/Process.h"
#include <memory>
#include <map>
#include <vector>

namespace Launcher {

// 实例类型
enum class InstanceType {
    Vanilla,
    Forge,
    Fabric,
    Quilt,
    NeoForge,
    Custom
};

// 实例状态
enum class InstanceStatus {
    NotCreated,
    Created,
    Broken,
    Running,
    Stopped
};

// 启动器核心类
class LauncherCore {
public:
    LauncherCore();
    virtual ~LauncherCore();
    
    // 初始化
    bool initialize();
    void shutdown();
    
    // 实例管理
    StringList getInstances();
    String getInstanceInfo(const String& instanceId);
    bool createInstance(const String& name, InstanceType type, const String& version);
    bool deleteInstance(const String& instanceId);
    bool copyInstance(const String& sourceId, const String& newName);
    
    // 实例启动
    bool launchInstance(const String& instanceId, const String& accountId = String());
    bool killInstance(const String& instanceId);
    bool isInstanceRunning(const String& instanceId);
    
    // Java管理
    StringList getJavaInstallations();
    String getDefaultJavaPath();
    bool setDefaultJava(const String& javaPath);
    bool validateJava(const String& javaPath);
    
    // 版本管理
    StringList getAvailableVersions();
    StringList getAvailableVersions(const String& type);
    bool installVersion(const String& version);
    
    // 模组管理
    StringList getModPlatforms();
    StringList searchMods(const String& platform, const String& query);
    bool installMod(const String& platform, const String& modId, const String& instanceId);
    StringList getInstanceMods(const String& instanceId);
    bool removeMod(const String& instanceId, const String& modId);
    
    // 账户管理
    StringList getAccounts();
    String getAccountInfo(const String& accountId);
    bool addAccount(const String& username, const String& password);
    bool removeAccount(const String& accountId);
    
    // 设置管理
    String getSetting(const String& key, const String& defaultValue = String());
    void setSetting(const String& key, const String& value);
    
    // 日志管理
    StringList getLogs(const String& instanceId, int maxLines = 1000);
    void clearLogs(const String& instanceId);

private:
    bool setupDirectories();
    bool loadConfiguration();
    bool saveConfiguration();
    
    String m_dataPath;
    String m_instancesPath;
    String m_versionsPath;
    String m_modsPath;
    String m_accountsPath;
    
    std::map<String, InstanceStatus> m_instanceStatus;
    std::map<String, String> m_runningInstances;
    String m_defaultJavaPath;
    
    std::unique_ptr<Core::Application> m_app;
};

// Minecraft实例类
class MinecraftInstance {
public:
    MinecraftInstance(const String& id, const String& name, InstanceType type);
    virtual ~MinecraftInstance();
    
    // 基本信息
    String id() const { return m_id; }
    String name() const { return m_name; }
    InstanceType type() const { return m_type; }
    String version() const { return m_version; }
    
    // 状态管理
    InstanceStatus status() const { return m_status; }
    void setStatus(InstanceStatus status) { m_status = status; }
    
    // 路径管理
    String rootPath() const { return m_rootPath; }
    String gamePath() const { return m_gamePath; }
    String modsPath() const { return m_modsPath; }
    String configPath() const { return m_configPath; }
    
    // 配置管理
    String getConfig(const String& key, const String& defaultValue = String());
    void setConfig(const String& key, const String& value);
    bool saveConfig();
    bool loadConfig();
    
    // 启动相关
    bool canLaunch() const;
    StringList getLaunchArguments(const String& javaPath, const String& accountId);
    StringList getJvmArguments();
    StringList getGameArguments();
    
    // 模组管理
    StringList getMods();
    bool addMod(const String& modPath);
    bool removeMod(const String& modName);
    bool hasMod(const String& modName);
    
    // 资源管理
    StringList getResourcePacks();
    StringList getDataPacks();
    StringList getShaderPacks();
    
    // 世界管理
    StringList getWorlds();
    String getWorldPath(const String& worldName);

private:
    void setupPaths();
    bool createDirectories();
    
    String m_id;
    String m_name;
    InstanceType m_type;
    String m_version;
    InstanceStatus m_status = InstanceStatus::NotCreated;
    
    String m_rootPath;
    String m_gamePath;
    String m_modsPath;
    String m_configPath;
    
    std::map<String, String> m_config;
};

// 启动任务类
class LaunchTask {
public:
    LaunchTask(const String& instanceId, const String& javaPath, const String& accountId);
    virtual ~LaunchTask();
    
    // 任务控制
    bool start();
    bool stop();
    bool isRunning() const { return m_running; }
    
    // 状态获取
    String getStatus() const { return m_status; }
    StringList getLogs() const { return m_logs; }
    int getExitCode() const { return m_exitCode; }
    
    // 回调设置
    void onStarted(std::function<void()> callback) { m_onStarted = callback; }
    void onFinished(std::function<void(int)> callback) { m_onFinished = callback; }
    void onLogOutput(std::function<void(const String&)> callback) { m_onLogOutput = callback; }

private:
    void launchProcess();
    void monitorProcess();
    void handleProcessOutput();
    void handleProcessError();
    
    String m_instanceId;
    String m_javaPath;
    String m_accountId;
    
    bool m_running = false;
    String m_status;
    StringList m_logs;
    int m_exitCode = 0;
    
    std::unique_ptr<Compat::Process> m_process;
    std::thread m_monitorThread;
    
    std::function<void()> m_onStarted;
    std::function<void(int)> m_onFinished;
    std::function<void(const String&)> m_onLogOutput;
};

// Java安装类
class JavaInstall {
public:
    JavaInstall(const String& path, const String& version);
    
    String path() const { return m_path; }
    String version() const { return m_version; }
    bool isValid() const { return m_valid; }
    void setValid(bool valid) { m_valid = valid; }
    
    bool validate();
    String getVersionString();

private:
    String m_path;
    String m_version;
    bool m_valid = false;
};

// 模组平台类
class ModPlatform {
public:
    ModPlatform(const String& name, const String& apiUrl);
    
    String name() const { return m_name; }
    String apiUrl() const { return m_apiUrl; }
    
    StringList searchMods(const String& query);
    String getModInfo(const String& modId);
    bool downloadMod(const String& modId, const String& filePath);

private:
    String m_name;
    String m_apiUrl;
};

} // namespace Launcher