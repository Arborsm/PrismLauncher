#include "IPCClient.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>
#include <iostream>
#include <fstream>

namespace PrismCore {
namespace IPC {

IPCClient::IPCClient() : m_port(8080) {
}

IPCClient::~IPCClient() {
    shutdown();
}

bool IPCClient::initialize() {
    if (m_initialized.load()) {
        return true;
    }
    
    m_shutdown.store(false);
    
    // Start response processing thread
    m_responseThread = std::thread(&IPCClient::processResponses, this);
    
    m_initialized.store(true);
    return true;
}

void IPCClient::shutdown() {
    if (!m_initialized.load()) {
        return;
    }
    
    m_shutdown.store(true);
    m_connected.store(false);
    
    // Notify thread to wake up
    m_responseCondition.notify_all();
    
    // Wait for thread to finish
    if (m_responseThread.joinable()) {
        m_responseThread.join();
    }
    
    // Clear pending requests
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        for (auto& pair : m_pendingRequests) {
            pair.second.set_value(IPCResponse(ServiceMethod::GET_SYSTEM_INFO, Status::ERROR, pair.first));
        }
        m_pendingRequests.clear();
    }
    
    m_initialized.store(false);
}

bool IPCClient::isConnected() const {
    return m_connected.load();
}

IPCResponse IPCClient::sendRequest(const IPCRequest& request) {
    if (!m_connected.load()) {
        return IPCResponse(request.method, Status::ERROR, request.requestId);
    }
    
    // Generate request ID if not provided
    std::string requestId = request.requestId.empty() ? generateRequestId() : request.requestId;
    
    // Create promise for response
    std::promise<IPCResponse> promise;
    auto future = promise.get_future();
    
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        m_pendingRequests[requestId] = std::move(promise);
    }
    
    // Create request copy with ID
    IPCRequest requestCopy = request;
    requestCopy.requestId = requestId;
    
    // Send HTTP request (simulated)
    IPCResponse response = sendHttpRequest(requestCopy);
    
    // Fulfill promise
    {
        std::lock_guard<std::mutex> lock(m_pendingMutex);
        auto it = m_pendingRequests.find(requestId);
        if (it != m_pendingRequests.end()) {
            it->second.set_value(response);
            m_pendingRequests.erase(it);
        }
    }
    
    return response;
}

IPCResponse IPCClient::sendRequest(ServiceMethod method, const std::map<std::string, std::string>& params) {
    IPCRequest request(method);
    request.parameters = params;
    return sendRequest(request);
}

std::future<IPCResponse> IPCClient::sendRequestAsync(const IPCRequest& request) {
    return std::async(std::launch::async, [this, request]() {
        return sendRequest(request);
    });
}

std::future<IPCResponse> IPCClient::sendRequestAsync(ServiceMethod method, const std::map<std::string, std::string>& params) {
    return std::async(std::launch::async, [this, method, params]() {
        return sendRequest(method, params);
    });
}

void IPCClient::setNotificationHandler(std::function<void(const IPCMessage&)> handler) {
    m_notificationHandler = handler;
}

void IPCClient::sendNotification(ServiceMethod method, const std::map<std::string, std::string>& data) {
    IPCMessage notification(MessageType::NOTIFICATION, method);
    notification.payload = serializeMessage(notification);
    
    if (m_notificationHandler) {
        m_notificationHandler(notification);
    }
}

void IPCClient::setEventHandler(std::function<void(const std::string&, const std::string&)> handler) {
    m_eventHandler = handler;
}

bool IPCClient::connectToServer(const std::string& serverAddress, int port) {
    m_serverAddress = serverAddress;
    m_port = port;
    
    // Simulate connection
    m_connected.store(true);
    
    // Test connection with a simple request
    IPCResponse response = sendRequest(ServiceMethod::GET_SYSTEM_INFO);
    return response.status == Status::SUCCESS;
}

void IPCClient::disconnectFromServer() {
    m_connected.store(false);
}

void IPCClient::onServerEvent(const std::string& eventType, const std::string& data) {
    if (m_eventHandler) {
        m_eventHandler(eventType, data);
    }
}

// Convenience methods
std::vector<InstanceInfo> IPCClient::getInstances() {
    IPCResponse response = sendRequest(ServiceMethod::LIST_INSTANCES);
    if (response.status == Status::SUCCESS) {
        return parseInstances(response.data);
    }
    return {};
}

InstanceInfo IPCClient::getInstance(const std::string& id) {
    IPCResponse response = sendRequest(ServiceMethod::GET_INSTANCE_INFO, {{"id", id}});
    if (response.status == Status::SUCCESS) {
        return parseInstance(response.data);
    }
    return {};
}

std::string IPCClient::createInstance(const std::string& name, const std::string& version) {
    IPCResponse response = sendRequest(ServiceMethod::CREATE_INSTANCE, {{"name", name}, {"version", version}});
    if (response.status == Status::SUCCESS) {
        return response.data;
    }
    return "";
}

bool IPCClient::deleteInstance(const std::string& id) {
    IPCResponse response = sendRequest(ServiceMethod::DELETE_INSTANCE, {{"id", id}});
    return response.status == Status::SUCCESS;
}

bool IPCClient::launchInstance(const std::string& id) {
    IPCResponse response = sendRequest(ServiceMethod::LAUNCH_INSTANCE, {{"id", id}});
    return response.status == Status::SUCCESS;
}

bool IPCClient::stopInstance(const std::string& id) {
    IPCResponse response = sendRequest(ServiceMethod::STOP_INSTANCE, {{"id", id}});
    return response.status == Status::SUCCESS;
}

std::vector<AccountInfo> IPCClient::getAccounts() {
    IPCResponse response = sendRequest(ServiceMethod::LIST_ACCOUNTS);
    if (response.status == Status::SUCCESS) {
        return parseAccounts(response.data);
    }
    return {};
}

std::string IPCClient::addAccount(const std::string& username, const std::string& type) {
    IPCResponse response = sendRequest(ServiceMethod::ADD_ACCOUNT, {{"username", username}, {"type", type}});
    if (response.status == Status::SUCCESS) {
        return response.data;
    }
    return "";
}

bool IPCClient::removeAccount(const std::string& id) {
    IPCResponse response = sendRequest(ServiceMethod::REMOVE_ACCOUNT, {{"id", id}});
    return response.status == Status::SUCCESS;
}

std::vector<JavaInfo> IPCClient::getJavaInstalls() {
    IPCResponse response = sendRequest(ServiceMethod::LIST_JAVA_INSTALLS);
    if (response.status == Status::SUCCESS) {
        return parseJavaInstalls(response.data);
    }
    return {};
}

SystemInfo IPCClient::getSystemInfo() {
    IPCResponse response = sendRequest(ServiceMethod::GET_SYSTEM_INFO);
    if (response.status == Status::SUCCESS) {
        return parseSystemInfo(response.data);
    }
    return {};
}

std::map<std::string, std::string> IPCClient::getSettings() {
    IPCResponse response = sendRequest(ServiceMethod::GET_SETTINGS);
    if (response.status == Status::SUCCESS) {
        return parseSettings(response.data);
    }
    return {};
}

void IPCClient::setSettings(const std::map<std::string, std::string>& settings) {
    sendRequest(ServiceMethod::SET_SETTINGS, settings);
}

std::vector<ModInfo> IPCClient::searchMods(const std::string& query, const std::string& platform) {
    IPCResponse response = sendRequest(ServiceMethod::SEARCH_MODS, {{"query", query}, {"platform", platform}});
    if (response.status == Status::SUCCESS) {
        return parseMods(response.data);
    }
    return {};
}

// Internal methods
void IPCClient::processResponses() {
    while (!m_shutdown.load()) {
        std::unique_ptr<IPCResponse> response;
        
        {
            std::unique_lock<std::mutex> lock(m_responseMutex);
            m_responseCondition.wait(lock, [this] { 
                return !m_responseQueue.empty() || m_shutdown.load(); 
            });
            
            if (m_shutdown.load()) {
                break;
            }
            
            if (!m_responseQueue.empty()) {
                response = std::move(m_responseQueue.front());
                m_responseQueue.pop();
            }
        }
        
        if (response) {
            // Handle response
            std::lock_guard<std::mutex> lock(m_pendingMutex);
            auto it = m_pendingRequests.find(response->requestId);
            if (it != m_pendingRequests.end()) {
                it->second.set_value(std::move(*response));
                m_pendingRequests.erase(it);
            }
        }
    }
}

void IPCClient::handleNotification(const IPCMessage& message) {
    if (m_notificationHandler) {
        m_notificationHandler(message);
    }
}

std::string IPCClient::generateRequestId() {
    auto now = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex << time << "-" << dis(gen) << dis(gen) << dis(gen) << dis(gen);
    return ss.str();
}

std::string IPCClient::serializeMessage(const IPCMessage& message) {
    // Simple JSON-like serialization
    std::stringstream ss;
    ss << "{";
    ss << "\"type\":" << static_cast<int>(message.type) << ",";
    ss << "\"method\":" << static_cast<int>(message.method) << ",";
    ss << "\"requestId\":\"" << message.requestId << "\",";
    ss << "\"payload\":\"" << message.payload << "\"";
    ss << "}";
    return ss.str();
}

std::unique_ptr<IPCMessage> IPCClient::deserializeMessage(const std::string& data) {
    // Simple deserialization - in real implementation, use proper JSON parser
    return std::make_unique<IPCMessage>();
}

IPCResponse IPCClient::sendHttpRequest(const IPCRequest& request) {
    // Simulate HTTP request to server
    // In real implementation, use proper HTTP client library
    
    // For now, simulate a successful response
    IPCResponse response(request.method, Status::SUCCESS, request.requestId);
    
    // Simulate different responses based on method
    switch (request.method) {
        case ServiceMethod::LIST_INSTANCES:
            response.data = R"([
                {"id":"instance1","name":"Test Instance 1","version":"1.20.1","iconPath":"","instancePath":"./instances/test1","isRunning":false},
                {"id":"instance2","name":"Test Instance 2","version":"1.19.4","iconPath":"","instancePath":"./instances/test2","isRunning":false}
            ])";
            break;
        case ServiceMethod::GET_SYSTEM_INFO:
            response.data = R"({
                "osName":"Linux",
                "osVersion":"6.12.8+",
                "architecture":"x86_64",
                "javaVersion":"17.0.2",
                "launcherVersion":"10.0.0"
            })";
            break;
        case ServiceMethod::LIST_ACCOUNTS:
            response.data = R"([
                {"id":"account1","username":"TestUser","uuid":"12345678-1234-1234-1234-123456789012","type":"offline","isValid":true}
            ])";
            break;
        case ServiceMethod::LIST_JAVA_INSTALLS:
            response.data = R"([
                {"id":"java1","version":"17.0.2","path":"/usr/lib/jvm/java-17-openjdk","architecture":"x86_64","isValid":true}
            ])";
            break;
        case ServiceMethod::GET_SETTINGS:
            response.data = R"({
                "theme":"dark",
                "language":"en",
                "autoUpdate":"true"
            })";
            break;
        default:
            response.data = "{}";
            break;
    }
    
    return response;
}

// Data parsing methods
std::vector<InstanceInfo> IPCClient::parseInstances(const std::string& jsonData) {
    std::vector<InstanceInfo> instances;
    
    // Simple JSON parsing - in real implementation, use proper JSON parser
    // This is a placeholder implementation
    InstanceInfo instance1;
    instance1.id = "instance1";
    instance1.name = "Test Instance 1";
    instance1.version = "1.20.1";
    instance1.iconPath = "";
    instance1.instancePath = "./instances/test1";
    instance1.isRunning = false;
    instances.push_back(instance1);
    
    InstanceInfo instance2;
    instance2.id = "instance2";
    instance2.name = "Test Instance 2";
    instance2.version = "1.19.4";
    instance2.iconPath = "";
    instance2.instancePath = "./instances/test2";
    instance2.isRunning = false;
    instances.push_back(instance2);
    
    return instances;
}

InstanceInfo IPCClient::parseInstance(const std::string& jsonData) {
    InstanceInfo instance;
    instance.id = "instance1";
    instance.name = "Test Instance 1";
    instance.version = "1.20.1";
    instance.iconPath = "";
    instance.instancePath = "./instances/test1";
    instance.isRunning = false;
    return instance;
}

std::vector<AccountInfo> IPCClient::parseAccounts(const std::string& jsonData) {
    std::vector<AccountInfo> accounts;
    
    AccountInfo account;
    account.id = "account1";
    account.username = "TestUser";
    account.uuid = "12345678-1234-1234-1234-123456789012";
    account.type = "offline";
    account.isValid = true;
    accounts.push_back(account);
    
    return accounts;
}

AccountInfo IPCClient::parseAccount(const std::string& jsonData) {
    AccountInfo account;
    account.id = "account1";
    account.username = "TestUser";
    account.uuid = "12345678-1234-1234-1234-123456789012";
    account.type = "offline";
    account.isValid = true;
    return account;
}

std::vector<JavaInfo> IPCClient::parseJavaInstalls(const std::string& jsonData) {
    std::vector<JavaInfo> javaInstalls;
    
    JavaInfo java;
    java.id = "java1";
    java.version = "17.0.2";
    java.path = "/usr/lib/jvm/java-17-openjdk";
    java.architecture = "x86_64";
    java.isValid = true;
    javaInstalls.push_back(java);
    
    return javaInstalls;
}

JavaInfo IPCClient::parseJavaInfo(const std::string& jsonData) {
    JavaInfo java;
    java.id = "java1";
    java.version = "17.0.2";
    java.path = "/usr/lib/jvm/java-17-openjdk";
    java.architecture = "x86_64";
    java.isValid = true;
    return java;
}

SystemInfo IPCClient::parseSystemInfo(const std::string& jsonData) {
    SystemInfo info;
    info.osName = "Linux";
    info.osVersion = "6.12.8+";
    info.architecture = "x86_64";
    info.javaVersion = "17.0.2";
    info.launcherVersion = "10.0.0";
    return info;
}

std::map<std::string, std::string> IPCClient::parseSettings(const std::string& jsonData) {
    std::map<std::string, std::string> settings;
    settings["theme"] = "dark";
    settings["language"] = "en";
    settings["autoUpdate"] = "true";
    return settings;
}

std::vector<ModInfo> IPCClient::parseMods(const std::string& jsonData) {
    std::vector<ModInfo> mods;
    
    ModInfo mod;
    mod.id = "mod1";
    mod.name = "Test Mod";
    mod.version = "1.0.0";
    mod.description = "A test mod";
    mod.author = "TestAuthor";
    mod.platform = "curseforge";
    mod.downloadUrl = "https://example.com/mod.jar";
    mod.filePath = "./mods/mod.jar";
    mods.push_back(mod);
    
    return mods;
}

ModInfo IPCClient::parseModInfo(const std::string& jsonData) {
    ModInfo mod;
    mod.id = "mod1";
    mod.name = "Test Mod";
    mod.version = "1.0.0";
    mod.description = "A test mod";
    mod.author = "TestAuthor";
    mod.platform = "curseforge";
    mod.downloadUrl = "https://example.com/mod.jar";
    mod.filePath = "./mods/mod.jar";
    return mod;
}

} // namespace IPC
} // namespace PrismCore