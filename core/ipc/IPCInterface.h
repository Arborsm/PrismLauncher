#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <future>

namespace PrismCore {
namespace IPC {

// Forward declarations
class IPCRequest;
class IPCResponse;
class IPCMessage;

// Message types
enum class MessageType {
    REQUEST = 0,
    RESPONSE = 1,
    NOTIFICATION = 2,
    ERROR = 3
};

// Request/Response status
enum class Status {
    SUCCESS = 0,
    ERROR = 1,
    TIMEOUT = 2,
    INVALID_REQUEST = 3,
    NOT_FOUND = 4,
    UNAUTHORIZED = 5
};

// Core service methods
enum class ServiceMethod {
    // Instance management
    CREATE_INSTANCE = 1000,
    DELETE_INSTANCE = 1001,
    COPY_INSTANCE = 1002,
    LIST_INSTANCES = 1003,
    GET_INSTANCE_INFO = 1004,
    UPDATE_INSTANCE = 1005,
    
    // Version management
    LIST_VERSIONS = 2000,
    GET_VERSION_INFO = 2001,
    INSTALL_VERSION = 2002,
    UNINSTALL_VERSION = 2003,
    
    // Account management
    LIST_ACCOUNTS = 3000,
    ADD_ACCOUNT = 3001,
    REMOVE_ACCOUNT = 3002,
    LOGIN_ACCOUNT = 3003,
    LOGOUT_ACCOUNT = 3004,
    GET_ACCOUNT_INFO = 3005,
    
    // Java management
    LIST_JAVA_INSTALLS = 4000,
    GET_JAVA_INFO = 4001,
    INSTALL_JAVA = 4002,
    UNINSTALL_JAVA = 4003,
    CHECK_JAVA_COMPATIBILITY = 4004,
    
    // Launch control
    LAUNCH_INSTANCE = 5000,
    STOP_INSTANCE = 5001,
    GET_LAUNCH_STATUS = 5002,
    GET_LAUNCH_LOGS = 5003,
    
    // Settings
    GET_SETTINGS = 6000,
    SET_SETTINGS = 6001,
    RESET_SETTINGS = 6002,
    
    // Mod platform
    SEARCH_MODS = 7000,
    GET_MOD_INFO = 7001,
    INSTALL_MOD = 7002,
    UNINSTALL_MOD = 7003,
    UPDATE_MOD = 7004,
    
    // System info
    GET_SYSTEM_INFO = 8000,
    GET_LAUNCHER_INFO = 8001,
    
    // File operations
    OPEN_FILE = 9000,
    OPEN_FOLDER = 9001,
    COPY_FILE = 9002,
    DELETE_FILE = 9003
};

// Data structures
struct InstanceInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string iconPath;
    std::string instancePath;
    bool isRunning;
    std::map<std::string, std::string> metadata;
};

struct AccountInfo {
    std::string id;
    std::string username;
    std::string uuid;
    std::string type; // "microsoft", "offline"
    bool isValid;
    std::map<std::string, std::string> properties;
};

struct JavaInfo {
    std::string id;
    std::string version;
    std::string path;
    std::string architecture;
    bool isValid;
    std::map<std::string, std::string> properties;
};

struct LaunchStatus {
    std::string instanceId;
    bool isRunning;
    int processId;
    std::string status; // "starting", "running", "stopped", "error"
    std::string errorMessage;
};

struct ModInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string platform; // "curseforge", "modrinth", "local"
    std::string downloadUrl;
    std::string filePath;
    std::map<std::string, std::string> metadata;
};

struct SystemInfo {
    std::string osName;
    std::string osVersion;
    std::string architecture;
    std::string javaVersion;
    std::string launcherVersion;
    std::map<std::string, std::string> properties;
};

// IPC Message structure
class IPCMessage {
public:
    MessageType type;
    ServiceMethod method;
    std::string requestId;
    std::map<std::string, std::string> headers;
    std::string payload;
    
    IPCMessage() = default;
    IPCMessage(MessageType t, ServiceMethod m, const std::string& id = "")
        : type(t), method(m), requestId(id) {}
    
    virtual ~IPCMessage() = default;
};

class IPCRequest : public IPCMessage {
public:
    std::map<std::string, std::string> parameters;
    
    IPCRequest() : IPCMessage(MessageType::REQUEST, ServiceMethod::GET_SYSTEM_INFO) {}
    IPCRequest(ServiceMethod method, const std::string& id = "")
        : IPCMessage(MessageType::REQUEST, method, id) {}
};

class IPCResponse : public IPCMessage {
public:
    Status status;
    std::string errorMessage;
    std::string data;
    
    IPCResponse() : IPCMessage(MessageType::RESPONSE, ServiceMethod::GET_SYSTEM_INFO), status(Status::SUCCESS) {}
    IPCResponse(ServiceMethod method, Status s, const std::string& id = "")
        : IPCMessage(MessageType::RESPONSE, method, id), status(s) {}
};

// IPC Interface
class IPCInterface {
public:
    virtual ~IPCInterface() = default;
    
    // Core methods
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isConnected() const = 0;
    
    // Synchronous methods
    virtual IPCResponse sendRequest(const IPCRequest& request) = 0;
    virtual IPCResponse sendRequest(ServiceMethod method, const std::map<std::string, std::string>& params = {}) = 0;
    
    // Asynchronous methods
    virtual std::future<IPCResponse> sendRequestAsync(const IPCRequest& request) = 0;
    virtual std::future<IPCResponse> sendRequestAsync(ServiceMethod method, const std::map<std::string, std::string>& params = {}) = 0;
    
    // Notification handling
    virtual void setNotificationHandler(std::function<void(const IPCMessage&)> handler) = 0;
    virtual void sendNotification(ServiceMethod method, const std::map<std::string, std::string>& data = {}) = 0;
    
    // Event handling
    virtual void setEventHandler(std::function<void(const std::string&, const std::string&)> handler) = 0;
};

// Factory function
std::unique_ptr<IPCInterface> createIPCInterface(const std::string& type = "default");

} // namespace IPC
} // namespace PrismCore