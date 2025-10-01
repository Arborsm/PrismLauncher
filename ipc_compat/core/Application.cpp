// SPDX-License-Identifier: GPL-3.0-only
/*
 * Core Application for PrismLauncher IPC
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "Application.h"
#include "../ipc/UnixSocketMessageBus.h"
#include "../compat/String.h"
#include <iostream>
#include <filesystem>
#include <chrono>

namespace Core {

Application* Application::s_instance = nullptr;

Application::Application(int argc, char** argv)
    : m_startTime(std::chrono::system_clock::now()) {
    s_instance = this;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        String arg = argv[i];
        if (arg == "--portable") {
            m_portable = true;
        } else if (arg.startsWith("--data-path=")) {
            m_dataPath = arg.mid(12);
        } else if (arg.startsWith("--root-path=")) {
            m_rootPath = arg.mid(12);
        }
    }
    
    setupPaths();
}

Application::~Application() {
    if (m_messageBus) {
        m_messageBus->stop();
    }
    s_instance = nullptr;
}

bool Application::initialize() {
    try {
        // 创建消息总线
        m_messageBus = std::make_shared<IPC::UnixSocketMessageBus>("/tmp/prismlauncher_ipc.sock");
        
        if (!m_messageBus->start()) {
            std::cerr << "Failed to start message bus" << std::endl;
            m_status = ApplicationStatus::Failed;
            return false;
        }
        
        // 创建管理器
        if (!createManagers()) {
            std::cerr << "Failed to create managers" << std::endl;
            m_status = ApplicationStatus::Failed;
            return false;
        }
        
        // 设置消息处理器
        setupMessageHandlers();
        
        // 检测能力
        detectCapabilities();
        
        m_status = ApplicationStatus::Initialized;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        m_status = ApplicationStatus::Failed;
        return false;
    }
}

int Application::exec() {
    if (m_status != ApplicationStatus::Initialized) {
        return 1;
    }
    
    m_status = ApplicationStatus::Running;
    
    // 主事件循环
    while (m_status == ApplicationStatus::Running) {
        if (m_messageBus->waitForMessage(std::chrono::milliseconds(100))) {
            auto messages = m_messageBus->getPendingMessages();
            for (const auto& message : messages) {
                // 消息已在MessageBus中处理
            }
        }
    }
    
    return m_status == ApplicationStatus::Succeeded ? 0 : 1;
}

void Application::quit() {
    m_status = ApplicationStatus::Succeeded;
}

int64_t Application::timeSinceStart() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime);
    return duration.count();
}

void Application::setupMessageHandlers() {
    // 注册消息处理器
    m_messageBus->registerCallback("get_instance_list", [this](const IPC::Message& request) {
        IPC::Message response(IPC::MessageType::RESPONSE, "get_instance_list");
        response.setId(request.id());
        
        try {
            auto instances = m_instanceManager->getInstanceList();
            // 简化实现，实际应该序列化实例列表
            response.setResult("instances");
        } catch (const std::exception& e) {
            response.setError(e.what());
        }
    });
    
    m_messageBus->registerCallback("launch_instance", [this](const IPC::Message& request) {
        IPC::Message response(IPC::MessageType::RESPONSE, "launch_instance");
        response.setId(request.id());
        
        try {
            String instanceId = std::get<String>(request.getParam("instance_id"));
            String accountId = std::get<String>(request.getParam("account_id"));
            
            bool success = m_launcherManager->launchMinecraft(instanceId, accountId);
            response.setResult(success);
        } catch (const std::exception& e) {
            response.setError(e.what());
        }
    });
    
    m_messageBus->registerCallback("get_settings", [this](const IPC::Message& request) {
        IPC::Message response(IPC::MessageType::RESPONSE, "get_settings");
        response.setId(request.id());
        
        try {
            String key = std::get<String>(request.getParam("key"));
            String defaultValue = std::get<String>(request.getParam("default_value"));
            
            String value = m_settingsManager->getSetting(key, defaultValue);
            response.setResult(value);
        } catch (const std::exception& e) {
            response.setError(e.what());
        }
    });
    
    m_messageBus->registerCallback("set_settings", [this](const IPC::Message& request) {
        IPC::Message response(IPC::MessageType::RESPONSE, "set_settings");
        response.setId(request.id());
        
        try {
            String key = std::get<String>(request.getParam("key"));
            String value = std::get<String>(request.getParam("value"));
            
            m_settingsManager->setSetting(key, value);
            response.setResult(true);
        } catch (const std::exception& e) {
            response.setError(e.what());
        }
    });
}

void Application::setupPaths() {
    if (m_rootPath.isEmpty()) {
        // 默认根路径
        char* home = getenv("HOME");
        if (home) {
            m_rootPath = String(home) + "/.prismlauncher";
        } else {
            m_rootPath = "./prismlauncher";
        }
    }
    
    if (m_dataPath.isEmpty()) {
        m_dataPath = m_portable ? m_rootPath : m_rootPath + "/data";
    }
    
    // 创建目录
    std::filesystem::create_directories(m_rootPath.toStdString());
    std::filesystem::create_directories(m_dataPath.toStdString());
}

void Application::detectCapabilities() {
    m_capabilities = 0;
    
    // 检测MSA支持
    m_capabilities |= static_cast<int>(ApplicationCapability::SupportsMSA);
    
    // 检测Flame支持
    m_capabilities |= static_cast<int>(ApplicationCapability::SupportsFlame);
    
    // 检测GameMode支持
    if (std::filesystem::exists("/usr/bin/gamemoderun")) {
        m_capabilities |= static_cast<int>(ApplicationCapability::SupportsGameMode);
    }
    
    // 检测MangoHud支持
    if (std::filesystem::exists("/usr/lib/libMangoHud.so")) {
        m_capabilities |= static_cast<int>(ApplicationCapability::SupportsMangoHud);
    }
}

bool Application::createManagers() {
    try {
        m_instanceManager = std::make_unique<InstanceManager>();
        m_launcherManager = std::make_unique<LauncherManager>();
        m_networkManager = std::make_unique<NetworkManager>();
        m_settingsManager = std::make_unique<SettingsManager>();
        m_logManager = std::make_unique<LogManager>();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create managers: " << e.what() << std::endl;
        return false;
    }
}

// InstanceManager 实现
InstanceManager::InstanceManager() {
    m_instancesPath = Application::instance()->dataPath() + "/instances";
    std::filesystem::create_directories(m_instancesPath.toStdString());
}

InstanceManager::~InstanceManager() = default;

StringList InstanceManager::getInstanceList() {
    StringList instances;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_instancesPath.toStdString())) {
            if (entry.is_directory()) {
                instances.append(String(entry.path().filename().string()));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading instances: " << e.what() << std::endl;
    }
    
    return instances;
}

String InstanceManager::getInstanceInfo(const String& instanceId) {
    String instancePath = m_instancesPath + "/" + instanceId;
    String configFile = instancePath + "/instance.cfg";
    
    // 简化实现，实际应该读取配置文件
    return "Instance: " + instanceId;
}

bool InstanceManager::createInstance(const String& name, const String& type) {
    String instancePath = m_instancesPath + "/" + name;
    
    try {
        std::filesystem::create_directories(instancePath.toStdString());
        
        // 创建基本配置文件
        String configFile = instancePath + "/instance.cfg";
        std::ofstream file(configFile.toStdString());
        if (file.is_open()) {
            file << "[General]\n";
            file << "name=" << name.toStdString() << "\n";
            file << "type=" << type.toStdString() << "\n";
            file.close();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating instance: " << e.what() << std::endl;
    }
    
    return false;
}

bool InstanceManager::deleteInstance(const String& instanceId) {
    String instancePath = m_instancesPath + "/" + instanceId;
    
    try {
        std::filesystem::remove_all(instancePath.toStdString());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting instance: " << e.what() << std::endl;
        return false;
    }
}

bool InstanceManager::copyInstance(const String& sourceId, const String& newName) {
    String sourcePath = m_instancesPath + "/" + sourceId;
    String destPath = m_instancesPath + "/" + newName;
    
    try {
        std::filesystem::copy(sourcePath.toStdString(), destPath.toStdString());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error copying instance: " << e.what() << std::endl;
        return false;
    }
}

bool InstanceManager::launchInstance(const String& instanceId, const String& accountId) {
    return Application::instance()->launcherManager()->launchMinecraft(instanceId, accountId);
}

bool InstanceManager::killInstance(const String& instanceId) {
    return Application::instance()->launcherManager()->killMinecraft(instanceId);
}

bool InstanceManager::isInstanceRunning(const String& instanceId) {
    return Application::instance()->launcherManager()->isMinecraftRunning(instanceId);
}

String InstanceManager::getInstanceStatus(const String& instanceId) {
    return Application::instance()->launcherManager()->getMinecraftStatus(instanceId);
}

// LauncherManager 实现
LauncherManager::LauncherManager() {
    // 检测Java安装
    m_javaPath = "/usr/bin/java";
}

LauncherManager::~LauncherManager() = default;

bool LauncherManager::launchMinecraft(const String& instanceId, const String& accountId) {
    // 简化实现，实际应该启动Minecraft进程
    m_runningGames[instanceId] = "running";
    return true;
}

bool LauncherManager::killMinecraft(const String& instanceId) {
    auto it = m_runningGames.find(instanceId);
    if (it != m_runningGames.end()) {
        m_runningGames.erase(it);
        return true;
    }
    return false;
}

bool LauncherManager::isMinecraftRunning(const String& instanceId) {
    return m_runningGames.find(instanceId) != m_runningGames.end();
}

String LauncherManager::getMinecraftStatus(const String& instanceId) {
    auto it = m_runningGames.find(instanceId);
    return it != m_runningGames.end() ? it->second : "not_running";
}

StringList LauncherManager::getJavaInstallations() {
    StringList installations;
    installations.append("/usr/bin/java");
    return installations;
}

String LauncherManager::getDefaultJavaPath() {
    return m_javaPath;
}

bool LauncherManager::setDefaultJava(const String& javaPath) {
    m_javaPath = javaPath;
    return true;
}

// NetworkManager 实现
NetworkManager::NetworkManager() {
    m_networkAccess = std::make_unique<Compat::NetworkAccessManager>();
    m_downloadManager = std::make_unique<Compat::DownloadManager>();
}

NetworkManager::~NetworkManager() = default;

bool NetworkManager::downloadFile(const String& url, const String& filePath) {
    // 简化实现
    return true;
}

String NetworkManager::downloadString(const String& url) {
    // 简化实现
    return "Downloaded content";
}

StringList NetworkManager::getModPlatforms() {
    StringList platforms;
    platforms.append("curseforge");
    platforms.append("modrinth");
    return platforms;
}

StringList NetworkManager::searchMods(const String& platform, const String& query) {
    // 简化实现
    StringList results;
    results.append("mod1");
    results.append("mod2");
    return results;
}

bool NetworkManager::installMod(const String& platform, const String& modId, const String& instanceId) {
    // 简化实现
    return true;
}

// SettingsManager 实现
SettingsManager::SettingsManager() {
    m_settingsPath = Application::instance()->dataPath() + "/settings.ini";
    loadSettings();
}

SettingsManager::~SettingsManager() {
    saveSettings();
}

String SettingsManager::getSetting(const String& key, const String& defaultValue) {
    auto it = m_settings.find(key);
    return it != m_settings.end() ? it->second : defaultValue;
}

void SettingsManager::setSetting(const String& key, const String& value) {
    m_settings[key] = value;
}

void SettingsManager::removeSetting(const String& key) {
    m_settings.erase(key);
}

StringList SettingsManager::getSettingGroups() {
    StringList groups;
    groups.append("General");
    groups.append("Java");
    groups.append("Network");
    return groups;
}

StringList SettingsManager::getSettingsInGroup(const String& group) {
    // 简化实现
    StringList settings;
    return settings;
}

bool SettingsManager::saveSettings() {
    try {
        std::ofstream file(m_settingsPath.toStdString());
        if (file.is_open()) {
            for (const auto& [key, value] : m_settings) {
                file << key.toStdString() << "=" << value.toStdString() << "\n";
            }
            file.close();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving settings: " << e.what() << std::endl;
    }
    return false;
}

bool SettingsManager::loadSettings() {
    try {
        std::ifstream file(m_settingsPath.toStdString());
        if (file.is_open()) {
            String line;
            while (std::getline(file, line.toStdString())) {
                int pos = line.indexOf('=');
                if (pos > 0) {
                    String key = line.left(pos);
                    String value = line.mid(pos + 1);
                    m_settings[key] = value;
                }
            }
            file.close();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading settings: " << e.what() << std::endl;
    }
    return false;
}

// LogManager 实现
LogManager::LogManager() = default;

LogManager::~LogManager() = default;

void LogManager::log(const String& message, int level) {
    std::lock_guard<std::mutex> lock(m_logMutex);
    
    String timestamp = Compat::String::number(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    
    String logEntry = "[" + timestamp + "] " + message;
    m_logs.append(logEntry);
    
    // 限制日志数量
    if (m_logs.size() > 10000) {
        m_logs.removeFirst();
    }
    
    // 输出到控制台
    std::cout << logEntry.toStdString() << std::endl;
}

void LogManager::logError(const String& message) {
    log("[ERROR] " + message, 1);
}

void LogManager::logWarning(const String& message) {
    log("[WARNING] " + message, 2);
}

void LogManager::logInfo(const String& message) {
    log("[INFO] " + message, 3);
}

void LogManager::logDebug(const String& message) {
    log("[DEBUG] " + message, 4);
}

StringList LogManager::getLogs(int maxLines) {
    std::lock_guard<std::mutex> lock(m_logMutex);
    
    if (maxLines <= 0 || maxLines >= m_logs.size()) {
        return m_logs;
    }
    
    StringList result;
    int start = m_logs.size() - maxLines;
    for (int i = start; i < m_logs.size(); ++i) {
        result.append(m_logs[i]);
    }
    
    return result;
}

void LogManager::clearLogs() {
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_logs.clear();
}

void LogManager::setLogFile(const String& filePath) {
    m_logFile = filePath;
}

} // namespace Core