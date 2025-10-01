#include "IPCService.h"
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <filesystem>

namespace PrismCore {
namespace IPC {

IPCService::IPCService() {
    loadSystemInfo();
    loadInstances();
    loadAccounts();
    loadSettings();
}

IPCService::~IPCService() {
    shutdown();
}

bool IPCService::initialize() {
    if (m_running.load()) {
        return true;
    }
    
    m_running.store(true);
    return true;
}

void IPCService::shutdown() {
    if (!m_running.load()) {
        return;
    }
    
    saveInstances();
    saveAccounts();
    saveSettings();
    
    m_running.store(false);
}

bool IPCService::isRunning() const {
    return m_running.load();
}

IPCResponse IPCService::processRequest(const IPCRequest& request) {
    if (!m_running.load()) {
        return IPCResponse(request.method, Status::ERROR, request.requestId);
    }
    
    if (!validateRequest(request)) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    
    try {
        switch (request.method) {
            // Instance management
            case ServiceMethod::CREATE_INSTANCE:
                response = handleCreateInstance(request);
                break;
            case ServiceMethod::DELETE_INSTANCE:
                response = handleDeleteInstance(request);
                break;
            case ServiceMethod::LIST_INSTANCES:
                response = handleListInstances(request);
                break;
            case ServiceMethod::GET_INSTANCE_INFO:
                response = handleGetInstanceInfo(request);
                break;
            case ServiceMethod::UPDATE_INSTANCE:
                response = handleUpdateInstance(request);
                break;
                
            // Account management
            case ServiceMethod::LIST_ACCOUNTS:
                response = handleListAccounts(request);
                break;
            case ServiceMethod::ADD_ACCOUNT:
                response = handleAddAccount(request);
                break;
            case ServiceMethod::REMOVE_ACCOUNT:
                response = handleRemoveAccount(request);
                break;
            case ServiceMethod::LOGIN_ACCOUNT:
                response = handleLoginAccount(request);
                break;
            case ServiceMethod::LOGOUT_ACCOUNT:
                response = handleLogoutAccount(request);
                break;
            case ServiceMethod::GET_ACCOUNT_INFO:
                response = handleGetAccountInfo(request);
                break;
                
            // Java management
            case ServiceMethod::LIST_JAVA_INSTALLS:
                response = handleListJavaInstalls(request);
                break;
            case ServiceMethod::GET_JAVA_INFO:
                response = handleGetJavaInfo(request);
                break;
            case ServiceMethod::INSTALL_JAVA:
                response = handleInstallJava(request);
                break;
            case ServiceMethod::UNINSTALL_JAVA:
                response = handleUninstallJava(request);
                break;
            case ServiceMethod::CHECK_JAVA_COMPATIBILITY:
                response = handleCheckJavaCompatibility(request);
                break;
                
            // Launch control
            case ServiceMethod::LAUNCH_INSTANCE:
                response = handleLaunchInstance(request);
                break;
            case ServiceMethod::STOP_INSTANCE:
                response = handleStopInstance(request);
                break;
            case ServiceMethod::GET_LAUNCH_STATUS:
                response = handleGetLaunchStatus(request);
                break;
            case ServiceMethod::GET_LAUNCH_LOGS:
                response = handleGetLaunchLogs(request);
                break;
                
            // Settings
            case ServiceMethod::GET_SETTINGS:
                response = handleGetSettings(request);
                break;
            case ServiceMethod::SET_SETTINGS:
                response = handleSetSettings(request);
                break;
            case ServiceMethod::RESET_SETTINGS:
                response = handleResetSettings(request);
                break;
                
            // Mod platform
            case ServiceMethod::SEARCH_MODS:
                response = handleSearchMods(request);
                break;
            case ServiceMethod::GET_MOD_INFO:
                response = handleGetModInfo(request);
                break;
            case ServiceMethod::INSTALL_MOD:
                response = handleInstallMod(request);
                break;
            case ServiceMethod::UNINSTALL_MOD:
                response = handleUninstallMod(request);
                break;
            case ServiceMethod::UPDATE_MOD:
                response = handleUpdateMod(request);
                break;
                
            // System info
            case ServiceMethod::GET_SYSTEM_INFO:
                response = handleGetSystemInfo(request);
                break;
            case ServiceMethod::GET_LAUNCHER_INFO:
                response = handleGetLauncherInfo(request);
                break;
                
            // File operations
            case ServiceMethod::OPEN_FILE:
                response = handleOpenFile(request);
                break;
            case ServiceMethod::OPEN_FOLDER:
                response = handleOpenFolder(request);
                break;
            case ServiceMethod::COPY_FILE:
                response = handleCopyFile(request);
                break;
            case ServiceMethod::DELETE_FILE:
                response = handleDeleteFile(request);
                break;
                
            default:
                response.status = Status::INVALID_REQUEST;
                response.errorMessage = "Unknown service method";
                break;
        }
    } catch (const std::exception& e) {
        response.status = Status::ERROR;
        response.errorMessage = e.what();
    }
    
    return response;
}

void IPCService::broadcastEvent(const std::string& eventType, const std::string& data) {
    // Broadcast event to all registered services
    std::lock_guard<std::mutex> lock(m_servicesMutex);
    for (auto& service : m_services) {
        if (service) {
            // Send notification
            service->sendNotification(ServiceMethod::GET_SYSTEM_INFO, {{"eventType", eventType}, {"data", data}});
        }
    }
}

void IPCService::registerService(std::shared_ptr<IPCInterface> service) {
    std::lock_guard<std::mutex> lock(m_servicesMutex);
    m_services.push_back(service);
}

// Instance management implementations
IPCResponse IPCService::handleCreateInstance(const IPCRequest& request) {
    auto nameIt = request.parameters.find("name");
    if (nameIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    InstanceInfo instance;
    instance.id = generateId();
    instance.name = nameIt->second;
    instance.version = request.parameters.count("version") ? request.parameters["version"] : "latest";
    instance.iconPath = request.parameters.count("iconPath") ? request.parameters["iconPath"] : "";
    instance.instancePath = "./instances/" + instance.name;
    instance.isRunning = false;
    
    m_instances.push_back(instance);
    
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    response.data = serializeData(instance.id);
    return response;
}

IPCResponse IPCService::handleDeleteInstance(const IPCRequest& request) {
    auto idIt = request.parameters.find("id");
    if (idIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    auto it = std::find_if(m_instances.begin(), m_instances.end(),
        [&](const InstanceInfo& instance) { return instance.id == idIt->second; });
    
    if (it != m_instances.end()) {
        m_instances.erase(it);
        return IPCResponse(request.method, Status::SUCCESS, request.requestId);
    }
    
    return IPCResponse(request.method, Status::NOT_FOUND, request.requestId);
}

IPCResponse IPCService::handleListInstances(const IPCRequest& request) {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < m_instances.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "{";
        ss << "\"id\":\"" << m_instances[i].id << "\",";
        ss << "\"name\":\"" << m_instances[i].name << "\",";
        ss << "\"version\":\"" << m_instances[i].version << "\",";
        ss << "\"iconPath\":\"" << m_instances[i].iconPath << "\",";
        ss << "\"instancePath\":\"" << m_instances[i].instancePath << "\",";
        ss << "\"isRunning\":" << (m_instances[i].isRunning ? "true" : "false");
        ss << "}";
    }
    ss << "]";
    
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    response.data = ss.str();
    return response;
}

IPCResponse IPCService::handleGetInstanceInfo(const IPCRequest& request) {
    auto idIt = request.parameters.find("id");
    if (idIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    auto it = std::find_if(m_instances.begin(), m_instances.end(),
        [&](const InstanceInfo& instance) { return instance.id == idIt->second; });
    
    if (it != m_instances.end()) {
        std::stringstream ss;
        ss << "{";
        ss << "\"id\":\"" << it->id << "\",";
        ss << "\"name\":\"" << it->name << "\",";
        ss << "\"version\":\"" << it->version << "\",";
        ss << "\"iconPath\":\"" << it->iconPath << "\",";
        ss << "\"instancePath\":\"" << it->instancePath << "\",";
        ss << "\"isRunning\":" << (it->isRunning ? "true" : "false");
        ss << "}";
        
        IPCResponse response(request.method, Status::SUCCESS, request.requestId);
        response.data = ss.str();
        return response;
    }
    
    return IPCResponse(request.method, Status::NOT_FOUND, request.requestId);
}

IPCResponse IPCService::handleUpdateInstance(const IPCRequest& request) {
    auto idIt = request.parameters.find("id");
    if (idIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    auto it = std::find_if(m_instances.begin(), m_instances.end(),
        [&](const InstanceInfo& instance) { return instance.id == idIt->second; });
    
    if (it != m_instances.end()) {
        if (request.parameters.count("name")) {
            it->name = request.parameters["name"];
        }
        if (request.parameters.count("version")) {
            it->version = request.parameters["version"];
        }
        if (request.parameters.count("iconPath")) {
            it->iconPath = request.parameters["iconPath"];
        }
        
        return IPCResponse(request.method, Status::SUCCESS, request.requestId);
    }
    
    return IPCResponse(request.method, Status::NOT_FOUND, request.requestId);
}

// Account management implementations
IPCResponse IPCService::handleListAccounts(const IPCRequest& request) {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < m_accounts.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "{";
        ss << "\"id\":\"" << m_accounts[i].id << "\",";
        ss << "\"username\":\"" << m_accounts[i].username << "\",";
        ss << "\"uuid\":\"" << m_accounts[i].uuid << "\",";
        ss << "\"type\":\"" << m_accounts[i].type << "\",";
        ss << "\"isValid\":" << (m_accounts[i].isValid ? "true" : "false");
        ss << "}";
    }
    ss << "]";
    
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    response.data = ss.str();
    return response;
}

IPCResponse IPCService::handleAddAccount(const IPCRequest& request) {
    auto usernameIt = request.parameters.find("username");
    if (usernameIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    AccountInfo account;
    account.id = generateId();
    account.username = usernameIt->second;
    account.uuid = request.parameters.count("uuid") ? request.parameters["uuid"] : "";
    account.type = request.parameters.count("type") ? request.parameters["type"] : "offline";
    account.isValid = true;
    
    m_accounts.push_back(account);
    
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    response.data = serializeData(account.id);
    return response;
}

IPCResponse IPCService::handleRemoveAccount(const IPCRequest& request) {
    auto idIt = request.parameters.find("id");
    if (idIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    auto it = std::find_if(m_accounts.begin(), m_accounts.end(),
        [&](const AccountInfo& account) { return account.id == idIt->second; });
    
    if (it != m_accounts.end()) {
        m_accounts.erase(it);
        return IPCResponse(request.method, Status::SUCCESS, request.requestId);
    }
    
    return IPCResponse(request.method, Status::NOT_FOUND, request.requestId);
}

IPCResponse IPCService::handleLoginAccount(const IPCRequest& request) {
    // Implementation for account login
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleLogoutAccount(const IPCRequest& request) {
    // Implementation for account logout
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleGetAccountInfo(const IPCRequest& request) {
    auto idIt = request.parameters.find("id");
    if (idIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    auto it = std::find_if(m_accounts.begin(), m_accounts.end(),
        [&](const AccountInfo& account) { return account.id == idIt->second; });
    
    if (it != m_accounts.end()) {
        std::stringstream ss;
        ss << "{";
        ss << "\"id\":\"" << it->id << "\",";
        ss << "\"username\":\"" << it->username << "\",";
        ss << "\"uuid\":\"" << it->uuid << "\",";
        ss << "\"type\":\"" << it->type << "\",";
        ss << "\"isValid\":" << (it->isValid ? "true" : "false");
        ss << "}";
        
        IPCResponse response(request.method, Status::SUCCESS, request.requestId);
        response.data = ss.str();
        return response;
    }
    
    return IPCResponse(request.method, Status::NOT_FOUND, request.requestId);
}

// Java management implementations
IPCResponse IPCService::handleListJavaInstalls(const IPCRequest& request) {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < m_javaInstalls.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "{";
        ss << "\"id\":\"" << m_javaInstalls[i].id << "\",";
        ss << "\"version\":\"" << m_javaInstalls[i].version << "\",";
        ss << "\"path\":\"" << m_javaInstalls[i].path << "\",";
        ss << "\"architecture\":\"" << m_javaInstalls[i].architecture << "\",";
        ss << "\"isValid\":" << (m_javaInstalls[i].isValid ? "true" : "false");
        ss << "}";
    }
    ss << "]";
    
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    response.data = ss.str();
    return response;
}

IPCResponse IPCService::handleGetJavaInfo(const IPCRequest& request) {
    // Implementation for getting Java info
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleInstallJava(const IPCRequest& request) {
    // Implementation for installing Java
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleUninstallJava(const IPCRequest& request) {
    // Implementation for uninstalling Java
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleCheckJavaCompatibility(const IPCRequest& request) {
    // Implementation for checking Java compatibility
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

// Launch control implementations
IPCResponse IPCService::handleLaunchInstance(const IPCRequest& request) {
    auto idIt = request.parameters.find("id");
    if (idIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    // Find instance
    auto it = std::find_if(m_instances.begin(), m_instances.end(),
        [&](const InstanceInfo& instance) { return instance.id == idIt->second; });
    
    if (it != m_instances.end()) {
        it->isRunning = true;
        
        // Create launch status
        LaunchStatus status;
        status.instanceId = it->id;
        status.isRunning = true;
        status.processId = 12345; // Simulate process ID
        status.status = "running";
        status.errorMessage = "";
        
        m_launchStatuses[it->id] = status;
        
        return IPCResponse(request.method, Status::SUCCESS, request.requestId);
    }
    
    return IPCResponse(request.method, Status::NOT_FOUND, request.requestId);
}

IPCResponse IPCService::handleStopInstance(const IPCRequest& request) {
    auto idIt = request.parameters.find("id");
    if (idIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    // Find instance
    auto it = std::find_if(m_instances.begin(), m_instances.end(),
        [&](const InstanceInfo& instance) { return instance.id == idIt->second; });
    
    if (it != m_instances.end()) {
        it->isRunning = false;
        
        // Update launch status
        auto statusIt = m_launchStatuses.find(it->id);
        if (statusIt != m_launchStatuses.end()) {
            statusIt->second.isRunning = false;
            statusIt->second.status = "stopped";
        }
        
        return IPCResponse(request.method, Status::SUCCESS, request.requestId);
    }
    
    return IPCResponse(request.method, Status::NOT_FOUND, request.requestId);
}

IPCResponse IPCService::handleGetLaunchStatus(const IPCRequest& request) {
    auto idIt = request.parameters.find("id");
    if (idIt == request.parameters.end()) {
        return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    auto it = m_launchStatuses.find(idIt->second);
    if (it != m_launchStatuses.end()) {
        std::stringstream ss;
        ss << "{";
        ss << "\"instanceId\":\"" << it->second.instanceId << "\",";
        ss << "\"isRunning\":" << (it->second.isRunning ? "true" : "false") << ",";
        ss << "\"processId\":" << it->second.processId << ",";
        ss << "\"status\":\"" << it->second.status << "\",";
        ss << "\"errorMessage\":\"" << it->second.errorMessage << "\"";
        ss << "}";
        
        IPCResponse response(request.method, Status::SUCCESS, request.requestId);
        response.data = ss.str();
        return response;
    }
    
    return IPCResponse(request.method, Status::NOT_FOUND, request.requestId);
}

IPCResponse IPCService::handleGetLaunchLogs(const IPCRequest& request) {
    // Implementation for getting launch logs
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

// Settings implementations
IPCResponse IPCService::handleGetSettings(const IPCRequest& request) {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& pair : m_settings) {
        if (!first) ss << ",";
        ss << "\"" << pair.first << "\":\"" << pair.second << "\"";
        first = false;
    }
    ss << "}";
    
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    response.data = ss.str();
    return response;
}

IPCResponse IPCService::handleSetSettings(const IPCRequest& request) {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    for (const auto& pair : request.parameters) {
        m_settings[pair.first] = pair.second;
    }
    
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleResetSettings(const IPCRequest& request) {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    m_settings.clear();
    
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

// Mod platform implementations
IPCResponse IPCService::handleSearchMods(const IPCRequest& request) {
    // Implementation for searching mods
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleGetModInfo(const IPCRequest& request) {
    // Implementation for getting mod info
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleInstallMod(const IPCRequest& request) {
    // Implementation for installing mod
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleUninstallMod(const IPCRequest& request) {
    // Implementation for uninstalling mod
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleUpdateMod(const IPCRequest& request) {
    // Implementation for updating mod
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

// System info implementations
IPCResponse IPCService::handleGetSystemInfo(const IPCRequest& request) {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    
    std::stringstream ss;
    ss << "{";
    ss << "\"osName\":\"" << m_systemInfo.osName << "\",";
    ss << "\"osVersion\":\"" << m_systemInfo.osVersion << "\",";
    ss << "\"architecture\":\"" << m_systemInfo.architecture << "\",";
    ss << "\"javaVersion\":\"" << m_systemInfo.javaVersion << "\",";
    ss << "\"launcherVersion\":\"" << m_systemInfo.launcherVersion << "\"";
    ss << "}";
    
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    response.data = ss.str();
    return response;
}

IPCResponse IPCService::handleGetLauncherInfo(const IPCRequest& request) {
    // Implementation for getting launcher info
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

// File operations implementations
IPCResponse IPCService::handleOpenFile(const IPCRequest& request) {
    // Implementation for opening file
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleOpenFolder(const IPCRequest& request) {
    // Implementation for opening folder
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleCopyFile(const IPCRequest& request) {
    // Implementation for copying file
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCService::handleDeleteFile(const IPCRequest& request) {
    // Implementation for deleting file
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

// Helper methods
std::string IPCService::generateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; ++i) {
        ss << dis(gen);
    }
    return ss.str();
}

bool IPCService::validateRequest(const IPCRequest& request) {
    return request.method >= ServiceMethod::CREATE_INSTANCE && 
           request.method <= ServiceMethod::DELETE_FILE;
}

std::string IPCService::serializeData(const std::string& data) {
    return data; // Simple passthrough for now
}

std::string IPCService::deserializeData(const std::string& data) {
    return data; // Simple passthrough for now
}

// Data management
void IPCService::loadInstances() {
    // Load instances from file or database
    // For now, create some sample instances
    InstanceInfo instance1;
    instance1.id = "instance1";
    instance1.name = "Test Instance 1";
    instance1.version = "1.20.1";
    instance1.iconPath = "";
    instance1.instancePath = "./instances/test1";
    instance1.isRunning = false;
    m_instances.push_back(instance1);
    
    InstanceInfo instance2;
    instance2.id = "instance2";
    instance2.name = "Test Instance 2";
    instance2.version = "1.19.4";
    instance2.iconPath = "";
    instance2.instancePath = "./instances/test2";
    instance2.isRunning = false;
    m_instances.push_back(instance2);
}

void IPCService::saveInstances() {
    // Save instances to file or database
}

void IPCService::loadAccounts() {
    // Load accounts from file or database
}

void IPCService::saveAccounts() {
    // Save accounts to file or database
}

void IPCService::loadSettings() {
    // Load settings from file or database
    m_settings["theme"] = "dark";
    m_settings["language"] = "en";
    m_settings["autoUpdate"] = "true";
}

void IPCService::saveSettings() {
    // Save settings to file or database
}

void IPCService::loadSystemInfo() {
    m_systemInfo.osName = "Linux";
    m_systemInfo.osVersion = "6.12.8+";
    m_systemInfo.architecture = "x86_64";
    m_systemInfo.javaVersion = "17.0.2";
    m_systemInfo.launcherVersion = "10.0.0";
}

} // namespace IPC
} // namespace PrismCore