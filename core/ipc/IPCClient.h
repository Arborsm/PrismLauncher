#pragma once

#include "IPCInterface.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <memory>
#include <functional>

namespace PrismCore {
namespace IPC {

// IPC Client for frontend applications
class IPCClient : public IPCInterface {
public:
    IPCClient();
    virtual ~IPCClient();
    
    // IPCInterface implementation
    bool initialize() override;
    void shutdown() override;
    bool isConnected() const override;
    
    IPCResponse sendRequest(const IPCRequest& request) override;
    IPCResponse sendRequest(ServiceMethod method, const std::map<std::string, std::string>& params = {}) override;
    
    std::future<IPCResponse> sendRequestAsync(const IPCRequest& request) override;
    std::future<IPCResponse> sendRequestAsync(ServiceMethod method, const std::map<std::string, std::string>& params = {}) override;
    
    void setNotificationHandler(std::function<void(const IPCMessage&)> handler) override;
    void sendNotification(ServiceMethod method, const std::map<std::string, std::string>& data = {}) override;
    
    void setEventHandler(std::function<void(const std::string&, const std::string&)> handler) override;
    
    // Connection management
    bool connectToServer(const std::string& serverAddress = "localhost", int port = 8080);
    void disconnectFromServer();
    
    // Event handling
    void onServerEvent(const std::string& eventType, const std::string& data);
    
    // Convenience methods for common operations
    std::vector<InstanceInfo> getInstances();
    InstanceInfo getInstance(const std::string& id);
    std::string createInstance(const std::string& name, const std::string& version = "latest");
    bool deleteInstance(const std::string& id);
    bool launchInstance(const std::string& id);
    bool stopInstance(const std::string& id);
    
    std::vector<AccountInfo> getAccounts();
    std::string addAccount(const std::string& username, const std::string& type = "offline");
    bool removeAccount(const std::string& id);
    
    std::vector<JavaInfo> getJavaInstalls();
    SystemInfo getSystemInfo();
    
    std::map<std::string, std::string> getSettings();
    void setSettings(const std::map<std::string, std::string>& settings);
    
    std::vector<ModInfo> searchMods(const std::string& query, const std::string& platform = "curseforge");

private:
    // Internal methods
    void processResponses();
    void handleNotification(const IPCMessage& message);
    std::string generateRequestId();
    
    // Message serialization
    std::string serializeMessage(const IPCMessage& message);
    std::unique_ptr<IPCMessage> deserializeMessage(const std::string& data);
    
    // HTTP client simulation (in real implementation, use proper HTTP client)
    IPCResponse sendHttpRequest(const IPCRequest& request);
    
    // Data parsing
    std::vector<InstanceInfo> parseInstances(const std::string& jsonData);
    InstanceInfo parseInstance(const std::string& jsonData);
    std::vector<AccountInfo> parseAccounts(const std::string& jsonData);
    AccountInfo parseAccount(const std::string& jsonData);
    std::vector<JavaInfo> parseJavaInstalls(const std::string& jsonData);
    JavaInfo parseJavaInfo(const std::string& jsonData);
    SystemInfo parseSystemInfo(const std::string& jsonData);
    std::map<std::string, std::string> parseSettings(const std::string& jsonData);
    std::vector<ModInfo> parseMods(const std::string& jsonData);
    ModInfo parseModInfo(const std::string& jsonData);

private:
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_shutdown{false};
    std::atomic<bool> m_connected{false};
    
    // Connection info
    std::string m_serverAddress;
    int m_port;
    
    // Threading
    std::thread m_responseThread;
    std::mutex m_responseMutex;
    std::condition_variable m_responseCondition;
    std::queue<std::unique_ptr<IPCResponse>> m_responseQueue;
    
    // Pending requests
    std::mutex m_pendingMutex;
    std::unordered_map<std::string, std::promise<IPCResponse>> m_pendingRequests;
    
    // Handlers
    std::function<void(const IPCMessage&)> m_notificationHandler;
    std::function<void(const std::string&, const std::string&)> m_eventHandler;
    
    // Request ID counter
    std::atomic<uint64_t> m_requestIdCounter{0};
};

} // namespace IPC
} // namespace PrismCore