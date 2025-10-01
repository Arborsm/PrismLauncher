// SPDX-License-Identifier: GPL-3.0-only
/*
 * Launcher Core Logic for PrismLauncher IPC
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "LauncherCore.h"
#include "../core/Application.h"
#include "../compat/String.h"
#include "../compat/Process.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace Launcher {

LauncherCore::LauncherCore() {
    m_app = std::make_unique<Core::Application>(0, nullptr);
}

LauncherCore::~LauncherCore() {
    shutdown();
}

bool LauncherCore::initialize() {
    if (!m_app->initialize()) {
        return false;
    }
    
    m_dataPath = m_app->dataPath();
    m_instancesPath = m_dataPath + "/instances";
    m_versionsPath = m_dataPath + "/versions";
    m_modsPath = m_dataPath + "/mods";
    m_accountsPath = m_dataPath + "/accounts";
    
    if (!setupDirectories()) {
        return false;
    }
    
    if (!loadConfiguration()) {
        return false;
    }
    
    return true;
}

void LauncherCore::shutdown() {
    // 停止所有运行中的实例
    for (const auto& [instanceId, _] : m_runningInstances) {
        killInstance(instanceId);
    }
    
    saveConfiguration();
}

StringList LauncherCore::getInstances() {
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

String LauncherCore::getInstanceInfo(const String& instanceId) {
    String instancePath = m_instancesPath + "/" + instanceId;
    String configFile = instancePath + "/instance.cfg";
    
    std::ifstream file(configFile.toStdString());
    if (!file.is_open()) {
        return "Instance not found";
    }
    
    String info = "Instance: " + instanceId + "\n";
    String line;
    while (std::getline(file, line.toStdString())) {
        info += line + "\n";
    }
    
    return info;
}

bool LauncherCore::createInstance(const String& name, InstanceType type, const String& version) {
    String instancePath = m_instancesPath + "/" + name;
    
    try {
        std::filesystem::create_directories(instancePath.toStdString());
        
        // 创建实例对象
        auto instance = std::make_unique<MinecraftInstance>(name, name, type);
        instance->setConfig("version", version);
        
        // 创建配置文件
        String configFile = instancePath + "/instance.cfg";
        std::ofstream file(configFile.toStdString());
        if (file.is_open()) {
            file << "[General]\n";
            file << "name=" << name.toStdString() << "\n";
            file << "type=" << static_cast<int>(type) << "\n";
            file << "version=" << version.toStdString() << "\n";
            file.close();
            
            m_instanceStatus[name] = InstanceStatus::Created;
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating instance: " << e.what() << std::endl;
    }
    
    return false;
}

bool LauncherCore::deleteInstance(const String& instanceId) {
    String instancePath = m_instancesPath + "/" + instanceId;
    
    try {
        std::filesystem::remove_all(instancePath.toStdString());
        m_instanceStatus.erase(instanceId);
        m_runningInstances.erase(instanceId);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error deleting instance: " << e.what() << std::endl;
        return false;
    }
}

bool LauncherCore::copyInstance(const String& sourceId, const String& newName) {
    String sourcePath = m_instancesPath + "/" + sourceId;
    String destPath = m_instancesPath + "/" + newName;
    
    try {
        std::filesystem::copy(sourcePath.toStdString(), destPath.toStdString());
        m_instanceStatus[newName] = InstanceStatus::Created;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error copying instance: " << e.what() << std::endl;
        return false;
    }
}

bool LauncherCore::launchInstance(const String& instanceId, const String& accountId) {
    String instancePath = m_instancesPath + "/" + instanceId;
    String javaPath = m_defaultJavaPath.isEmpty() ? "/usr/bin/java" : m_defaultJavaPath;
    
    try {
        auto launchTask = std::make_unique<LaunchTask>(instanceId, javaPath, accountId);
        
        launchTask->onStarted([this, instanceId]() {
            m_runningInstances[instanceId] = "running";
            m_instanceStatus[instanceId] = InstanceStatus::Running;
        });
        
        launchTask->onFinished([this, instanceId](int exitCode) {
            m_runningInstances.erase(instanceId);
            m_instanceStatus[instanceId] = InstanceStatus::Stopped;
        });
        
        if (launchTask->start()) {
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error launching instance: " << e.what() << std::endl;
    }
    
    return false;
}

bool LauncherCore::killInstance(const String& instanceId) {
    auto it = m_runningInstances.find(instanceId);
    if (it != m_runningInstances.end()) {
        // 简化实现，实际应该发送终止信号
        m_runningInstances.erase(it);
        m_instanceStatus[instanceId] = InstanceStatus::Stopped;
        return true;
    }
    return false;
}

bool LauncherCore::isInstanceRunning(const String& instanceId) {
    return m_runningInstances.find(instanceId) != m_runningInstances.end();
}

StringList LauncherCore::getJavaInstallations() {
    StringList installations;
    
    // 检测常见的Java安装路径
    StringList commonPaths = {
        "/usr/bin/java",
        "/usr/lib/jvm/default-java/bin/java",
        "/opt/java/bin/java",
        "/usr/local/bin/java"
    };
    
    for (const auto& path : commonPaths) {
        if (std::filesystem::exists(path.toStdString())) {
            installations.append(path);
        }
    }
    
    return installations;
}

String LauncherCore::getDefaultJavaPath() {
    return m_defaultJavaPath;
}

bool LauncherCore::setDefaultJava(const String& javaPath) {
    if (validateJava(javaPath)) {
        m_defaultJavaPath = javaPath;
        return true;
    }
    return false;
}

bool LauncherCore::validateJava(const String& javaPath) {
    try {
        Compat::Process process;
        process.start(javaPath, {"-version"});
        
        if (process.waitForFinished(5000)) {
            String output = process.readAllStandardOutput();
            String error = process.readAllStandardError();
            return !output.isEmpty() || !error.isEmpty();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error validating Java: " << e.what() << std::endl;
    }
    
    return false;
}

StringList LauncherCore::getAvailableVersions() {
    StringList versions;
    
    // 简化实现，实际应该从Mojang API获取
    versions.append("1.20.1");
    versions.append("1.19.4");
    versions.append("1.18.2");
    
    return versions;
}

StringList LauncherCore::getAvailableVersions(const String& type) {
    StringList versions;
    
    if (type == "vanilla") {
        versions = getAvailableVersions();
    } else if (type == "forge") {
        versions.append("1.20.1-47.2.0");
        versions.append("1.19.4-45.2.0");
    } else if (type == "fabric") {
        versions.append("1.20.1");
        versions.append("1.19.4");
    }
    
    return versions;
}

bool LauncherCore::installVersion(const String& version) {
    // 简化实现，实际应该下载和安装版本
    return true;
}

StringList LauncherCore::getModPlatforms() {
    StringList platforms;
    platforms.append("curseforge");
    platforms.append("modrinth");
    platforms.append("github");
    return platforms;
}

StringList LauncherCore::searchMods(const String& platform, const String& query) {
    // 简化实现，实际应该调用平台API
    StringList results;
    results.append("mod1");
    results.append("mod2");
    return results;
}

bool LauncherCore::installMod(const String& platform, const String& modId, const String& instanceId) {
    // 简化实现，实际应该下载和安装模组
    return true;
}

StringList LauncherCore::getInstanceMods(const String& instanceId) {
    StringList mods;
    String modsPath = m_instancesPath + "/" + instanceId + "/mods";
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(modsPath.toStdString())) {
            if (entry.is_regular_file() && entry.path().extension() == ".jar") {
                mods.append(String(entry.path().filename().string()));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading mods: " << e.what() << std::endl;
    }
    
    return mods;
}

bool LauncherCore::removeMod(const String& instanceId, const String& modId) {
    String modPath = m_instancesPath + "/" + instanceId + "/mods/" + modId;
    
    try {
        std::filesystem::remove(modPath.toStdString());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error removing mod: " << e.what() << std::endl;
        return false;
    }
}

StringList LauncherCore::getAccounts() {
    StringList accounts;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_accountsPath.toStdString())) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                accounts.append(String(entry.path().stem().string()));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading accounts: " << e.what() << std::endl;
    }
    
    return accounts;
}

String LauncherCore::getAccountInfo(const String& accountId) {
    String accountFile = m_accountsPath + "/" + accountId + ".json";
    
    std::ifstream file(accountFile.toStdString());
    if (!file.is_open()) {
        return "Account not found";
    }
    
    String info;
    String line;
    while (std::getline(file, line.toStdString())) {
        info += line + "\n";
    }
    
    return info;
}

bool LauncherCore::addAccount(const String& username, const String& password) {
    // 简化实现，实际应该处理认证
    String accountId = username;
    String accountFile = m_accountsPath + "/" + accountId + ".json";
    
    std::ofstream file(accountFile.toStdString());
    if (file.is_open()) {
        file << "{\n";
        file << "  \"username\": \"" << username.toStdString() << "\",\n";
        file << "  \"type\": \"microsoft\"\n";
        file << "}\n";
        file.close();
        return true;
    }
    
    return false;
}

bool LauncherCore::removeAccount(const String& accountId) {
    String accountFile = m_accountsPath + "/" + accountId + ".json";
    
    try {
        std::filesystem::remove(accountFile.toStdString());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error removing account: " << e.what() << std::endl;
        return false;
    }
}

String LauncherCore::getSetting(const String& key, const String& defaultValue) {
    return m_app->settingsManager()->getSetting(key, defaultValue);
}

void LauncherCore::setSetting(const String& key, const String& value) {
    m_app->settingsManager()->setSetting(key, value);
}

StringList LauncherCore::getLogs(const String& instanceId, int maxLines) {
    return m_app->logManager()->getLogs(maxLines);
}

void LauncherCore::clearLogs(const String& instanceId) {
    m_app->logManager()->clearLogs();
}

bool LauncherCore::setupDirectories() {
    try {
        std::filesystem::create_directories(m_instancesPath.toStdString());
        std::filesystem::create_directories(m_versionsPath.toStdString());
        std::filesystem::create_directories(m_modsPath.toStdString());
        std::filesystem::create_directories(m_accountsPath.toStdString());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error setting up directories: " << e.what() << std::endl;
        return false;
    }
}

bool LauncherCore::loadConfiguration() {
    // 加载默认Java路径
    m_defaultJavaPath = getSetting("default_java", "/usr/bin/java");
    
    // 加载实例状态
    StringList instances = getInstances();
    for (const auto& instance : instances) {
        m_instanceStatus[instance] = InstanceStatus::Created;
    }
    
    return true;
}

bool LauncherCore::saveConfiguration() {
    setSetting("default_java", m_defaultJavaPath);
    return m_app->settingsManager()->saveSettings();
}

// MinecraftInstance 实现
MinecraftInstance::MinecraftInstance(const String& id, const String& name, InstanceType type)
    : m_id(id), m_name(name), m_type(type) {
    setupPaths();
}

MinecraftInstance::~MinecraftInstance() = default;

String MinecraftInstance::getConfig(const String& key, const String& defaultValue) {
    auto it = m_config.find(key);
    return it != m_config.end() ? it->second : defaultValue;
}

void MinecraftInstance::setConfig(const String& key, const String& value) {
    m_config[key] = value;
}

bool MinecraftInstance::saveConfig() {
    String configFile = m_configPath + "/instance.cfg";
    
    std::ofstream file(configFile.toStdString());
    if (!file.is_open()) {
        return false;
    }
    
    file << "[General]\n";
    for (const auto& [key, value] : m_config) {
        file << key.toStdString() << "=" << value.toStdString() << "\n";
    }
    
    file.close();
    return true;
}

bool MinecraftInstance::loadConfig() {
    String configFile = m_configPath + "/instance.cfg";
    
    std::ifstream file(configFile.toStdString());
    if (!file.is_open()) {
        return false;
    }
    
    String line;
    while (std::getline(file, line.toStdString())) {
        if (line.startsWith("[") && line.endsWith("]")) {
            continue; // 跳过节标题
        }
        
        int pos = line.indexOf('=');
        if (pos > 0) {
            String key = line.left(pos);
            String value = line.mid(pos + 1);
            m_config[key] = value;
        }
    }
    
    file.close();
    return true;
}

bool MinecraftInstance::canLaunch() const {
    return m_status == InstanceStatus::Created && !m_version.isEmpty();
}

StringList MinecraftInstance::getLaunchArguments(const String& javaPath, const String& accountId) {
    StringList args;
    
    args.append(javaPath);
    args.append("-Xmx2G");
    args.append("-Xms1G");
    args.append("-Djava.library.path=" + m_gamePath + "/natives");
    args.append("-cp");
    args.append(m_gamePath + "/libraries/*:" + m_gamePath + "/minecraft.jar");
    args.append("net.minecraft.client.main.Main");
    args.append("--username");
    args.append(accountId);
    args.append("--version");
    args.append(m_version);
    args.append("--gameDir");
    args.append(m_gamePath);
    args.append("--assetsDir");
    args.append(m_gamePath + "/assets");
    args.append("--assetIndex");
    args.append("1.20.1");
    args.append("--uuid");
    args.append("00000000-0000-0000-0000-000000000000");
    args.append("--accessToken");
    args.append("null");
    args.append("--userType");
    args.append("legacy");
    args.append("--versionType");
    args.append("release");
    
    return args;
}

StringList MinecraftInstance::getJvmArguments() {
    StringList args;
    args.append("-Xmx2G");
    args.append("-Xms1G");
    return args;
}

StringList MinecraftInstance::getGameArguments() {
    StringList args;
    args.append("--username");
    args.append("Player");
    args.append("--version");
    args.append(m_version);
    return args;
}

StringList MinecraftInstance::getMods() {
    StringList mods;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_modsPath.toStdString())) {
            if (entry.is_regular_file() && entry.path().extension() == ".jar") {
                mods.append(String(entry.path().filename().string()));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading mods: " << e.what() << std::endl;
    }
    
    return mods;
}

bool MinecraftInstance::addMod(const String& modPath) {
    String fileName = String(std::filesystem::path(modPath.toStdString()).filename().string());
    String destPath = m_modsPath + "/" + fileName;
    
    try {
        std::filesystem::copy(modPath.toStdString(), destPath.toStdString());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error adding mod: " << e.what() << std::endl;
        return false;
    }
}

bool MinecraftInstance::removeMod(const String& modName) {
    String modPath = m_modsPath + "/" + modName;
    
    try {
        std::filesystem::remove(modPath.toStdString());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error removing mod: " << e.what() << std::endl;
        return false;
    }
}

bool MinecraftInstance::hasMod(const String& modName) {
    String modPath = m_modsPath + "/" + modName;
    return std::filesystem::exists(modPath.toStdString());
}

StringList MinecraftInstance::getResourcePacks() {
    StringList packs;
    String packsPath = m_gamePath + "/resourcepacks";
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(packsPath.toStdString())) {
            if (entry.is_directory()) {
                packs.append(String(entry.path().filename().string()));
            }
        }
    } catch (const std::exception& e) {
        // 目录可能不存在
    }
    
    return packs;
}

StringList MinecraftInstance::getDataPacks() {
    StringList packs;
    String packsPath = m_gamePath + "/datapacks";
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(packsPath.toStdString())) {
            if (entry.is_directory()) {
                packs.append(String(entry.path().filename().string()));
            }
        }
    } catch (const std::exception& e) {
        // 目录可能不存在
    }
    
    return packs;
}

StringList MinecraftInstance::getShaderPacks() {
    StringList packs;
    String packsPath = m_gamePath + "/shaderpacks";
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(packsPath.toStdString())) {
            if (entry.is_directory()) {
                packs.append(String(entry.path().filename().string()));
            }
        }
    } catch (const std::exception& e) {
        // 目录可能不存在
    }
    
    return packs;
}

StringList MinecraftInstance::getWorlds() {
    StringList worlds;
    String worldsPath = m_gamePath + "/saves";
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(worldsPath.toStdString())) {
            if (entry.is_directory()) {
                worlds.append(String(entry.path().filename().string()));
            }
        }
    } catch (const std::exception& e) {
        // 目录可能不存在
    }
    
    return worlds;
}

String MinecraftInstance::getWorldPath(const String& worldName) {
    return m_gamePath + "/saves/" + worldName;
}

void MinecraftInstance::setupPaths() {
    m_rootPath = "/tmp/prismlauncher/instances/" + m_id;
    m_gamePath = m_rootPath + "/minecraft";
    m_modsPath = m_gamePath + "/mods";
    m_configPath = m_rootPath;
    
    createDirectories();
}

bool MinecraftInstance::createDirectories() {
    try {
        std::filesystem::create_directories(m_rootPath.toStdString());
        std::filesystem::create_directories(m_gamePath.toStdString());
        std::filesystem::create_directories(m_modsPath.toStdString());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating directories: " << e.what() << std::endl;
        return false;
    }
}

// LaunchTask 实现
LaunchTask::LaunchTask(const String& instanceId, const String& javaPath, const String& accountId)
    : m_instanceId(instanceId), m_javaPath(javaPath), m_accountId(accountId) {
}

LaunchTask::~LaunchTask() {
    if (m_running) {
        stop();
    }
}

bool LaunchTask::start() {
    if (m_running) {
        return false;
    }
    
    m_running = true;
    m_status = "Starting";
    
    launchProcess();
    
    if (m_onStarted) {
        m_onStarted();
    }
    
    return true;
}

bool LaunchTask::stop() {
    if (!m_running) {
        return false;
    }
    
    if (m_process) {
        m_process->kill();
        m_process->waitForFinished(5000);
    }
    
    m_running = false;
    m_status = "Stopped";
    
    return true;
}

void LaunchTask::launchProcess() {
    m_process = std::make_unique<Compat::Process>();
    
    // 设置启动参数
    StringList args = {"-version"}; // 简化实现
    m_process->start(m_javaPath, args);
    
    // 设置回调
    m_process->onFinished([this](int exitCode, Compat::ExitStatus status) {
        m_exitCode = exitCode;
        m_running = false;
        m_status = "Finished";
        
        if (m_onFinished) {
            m_onFinished(exitCode);
        }
    });
    
    m_process->onReadyReadStandardOutput([this]() {
        String output = m_process->readAllStandardOutput();
        m_logs.append(output);
        
        if (m_onLogOutput) {
            m_onLogOutput(output);
        }
    });
    
    m_process->onReadyReadStandardError([this]() {
        String error = m_process->readAllStandardError();
        m_logs.append("[ERROR] " + error);
        
        if (m_onLogOutput) {
            m_onLogOutput("[ERROR] " + error);
        }
    });
}

void LaunchTask::monitorProcess() {
    // 监控线程实现
}

void LaunchTask::handleProcessOutput() {
    // 处理进程输出
}

void LaunchTask::handleProcessError() {
    // 处理进程错误
}

} // namespace Launcher