#pragma once

#include "IPCInterface.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <atomic>

namespace PrismCore {
namespace IPC {

// IPC Manager implementation
class IPCManager : public IPCInterface {
public:
    IPCManager();
    virtual ~IPCManager();
    
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

private:
    // Internal methods
    void processRequests();
    void processResponses();
    void handleNotification(const IPCMessage& message);
    std::string generateRequestId();
    
    // Message serialization
    std::string serializeMessage(const IPCMessage& message);
    std::unique_ptr<IPCMessage> deserializeMessage(const std::string& data);
    
    // Service method handlers
    IPCResponse handleInstanceRequest(const IPCRequest& request);
    IPCResponse handleAccountRequest(const IPCRequest& request);
    IPCResponse handleJavaRequest(const IPCRequest& request);
    IPCResponse handleLaunchRequest(const IPCRequest& request);
    IPCResponse handleSettingsRequest(const IPCRequest& request);
    IPCResponse handleModRequest(const IPCRequest& request);
    IPCResponse handleSystemRequest(const IPCRequest& request);
    IPCResponse handleFileRequest(const IPCRequest& request);

private:
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_shutdown{false};
    std::atomic<bool> m_connected{false};
    
    // Threading
    std::thread m_requestThread;
    std::thread m_responseThread;
    std::mutex m_requestMutex;
    std::mutex m_responseMutex;
    std::condition_variable m_requestCondition;
    std::condition_variable m_responseCondition;
    
    // Message queues
    std::queue<std::unique_ptr<IPCRequest>> m_requestQueue;
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

// IPC Service implementation
class IPCService {
public:
    IPCService();
    virtual ~IPCService();
    
    bool initialize();
    void shutdown();
    bool isRunning() const;
    
    // Service methods
    IPCResponse processRequest(const IPCRequest& request);
    
    // Event broadcasting
    void broadcastEvent(const std::string& eventType, const std::string& data);
    
    // Service registration
    void registerService(std::shared_ptr<IPCInterface> service);

private:
    std::atomic<bool> m_running{false};
    std::vector<std::shared_ptr<IPCInterface>> m_services;
    std::mutex m_servicesMutex;
};

} // namespace IPC
} // namespace PrismCore