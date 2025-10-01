#include "IPCManager.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>

namespace PrismCore {
namespace IPC {

IPCManager::IPCManager() {
}

IPCManager::~IPCManager() {
    shutdown();
}

bool IPCManager::initialize() {
    if (m_initialized.load()) {
        return true;
    }
    
    m_shutdown.store(false);
    m_connected.store(true);
    
    // Start processing threads
    m_requestThread = std::thread(&IPCManager::processRequests, this);
    m_responseThread = std::thread(&IPCManager::processResponses, this);
    
    m_initialized.store(true);
    return true;
}

void IPCManager::shutdown() {
    if (!m_initialized.load()) {
        return;
    }
    
    m_shutdown.store(true);
    m_connected.store(false);
    
    // Notify threads to wake up
    m_requestCondition.notify_all();
    m_responseCondition.notify_all();
    
    // Wait for threads to finish
    if (m_requestThread.joinable()) {
        m_requestThread.join();
    }
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

bool IPCManager::isConnected() const {
    return m_connected.load();
}

IPCResponse IPCManager::sendRequest(const IPCRequest& request) {
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
    
    // Add to request queue
    {
        std::lock_guard<std::mutex> lock(m_requestMutex);
        m_requestQueue.push(std::make_unique<IPCRequest>(std::move(requestCopy)));
    }
    m_requestCondition.notify_one();
    
    // Wait for response
    try {
        return future.get();
    } catch (const std::exception& e) {
        return IPCResponse(request.method, Status::ERROR, requestId);
    }
}

IPCResponse IPCManager::sendRequest(ServiceMethod method, const std::map<std::string, std::string>& params) {
    IPCRequest request(method);
    request.parameters = params;
    return sendRequest(request);
}

std::future<IPCResponse> IPCManager::sendRequestAsync(const IPCRequest& request) {
    return std::async(std::launch::async, [this, request]() {
        return sendRequest(request);
    });
}

std::future<IPCResponse> IPCManager::sendRequestAsync(ServiceMethod method, const std::map<std::string, std::string>& params) {
    return std::async(std::launch::async, [this, method, params]() {
        return sendRequest(method, params);
    });
}

void IPCManager::setNotificationHandler(std::function<void(const IPCMessage&)> handler) {
    m_notificationHandler = handler;
}

void IPCManager::sendNotification(ServiceMethod method, const std::map<std::string, std::string>& data) {
    IPCMessage notification(MessageType::NOTIFICATION, method);
    notification.payload = serializeMessage(notification);
    
    if (m_notificationHandler) {
        m_notificationHandler(notification);
    }
}

void IPCManager::setEventHandler(std::function<void(const std::string&, const std::string&)> handler) {
    m_eventHandler = handler;
}

void IPCManager::processRequests() {
    while (!m_shutdown.load()) {
        std::unique_ptr<IPCRequest> request;
        
        {
            std::unique_lock<std::mutex> lock(m_requestMutex);
            m_requestCondition.wait(lock, [this] { 
                return !m_requestQueue.empty() || m_shutdown.load(); 
            });
            
            if (m_shutdown.load()) {
                break;
            }
            
            if (!m_requestQueue.empty()) {
                request = std::move(m_requestQueue.front());
                m_requestQueue.pop();
            }
        }
        
        if (request) {
            // Process request and generate response
            IPCResponse response = handleInstanceRequest(*request);
            if (response.status == Status::SUCCESS) {
                response = handleAccountRequest(*request);
            }
            if (response.status == Status::SUCCESS) {
                response = handleJavaRequest(*request);
            }
            if (response.status == Status::SUCCESS) {
                response = handleLaunchRequest(*request);
            }
            if (response.status == Status::SUCCESS) {
                response = handleSettingsRequest(*request);
            }
            if (response.status == Status::SUCCESS) {
                response = handleModRequest(*request);
            }
            if (response.status == Status::SUCCESS) {
                response = handleSystemRequest(*request);
            }
            if (response.status == Status::SUCCESS) {
                response = handleFileRequest(*request);
            }
            
            // Set request ID
            response.requestId = request->requestId;
            
            // Add to response queue
            {
                std::lock_guard<std::mutex> lock(m_responseMutex);
                m_responseQueue.push(std::make_unique<IPCResponse>(std::move(response)));
            }
            m_responseCondition.notify_one();
        }
    }
}

void IPCManager::processResponses() {
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
            // Find and fulfill promise
            std::lock_guard<std::mutex> lock(m_pendingMutex);
            auto it = m_pendingRequests.find(response->requestId);
            if (it != m_pendingRequests.end()) {
                it->second.set_value(std::move(*response));
                m_pendingRequests.erase(it);
            }
        }
    }
}

void IPCManager::handleNotification(const IPCMessage& message) {
    if (m_notificationHandler) {
        m_notificationHandler(message);
    }
}

std::string IPCManager::generateRequestId() {
    auto now = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex << time << "-" << dis(gen) << dis(gen) << dis(gen) << dis(gen);
    return ss.str();
}

std::string IPCManager::serializeMessage(const IPCMessage& message) {
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

std::unique_ptr<IPCMessage> IPCManager::deserializeMessage(const std::string& data) {
    // Simple deserialization - in real implementation, use proper JSON parser
    // This is a placeholder implementation
    return std::make_unique<IPCMessage>();
}

// Service method handlers
IPCResponse IPCManager::handleInstanceRequest(const IPCRequest& request) {
    switch (request.method) {
        case ServiceMethod::CREATE_INSTANCE:
            // Implementation for creating instance
            break;
        case ServiceMethod::DELETE_INSTANCE:
            // Implementation for deleting instance
            break;
        case ServiceMethod::LIST_INSTANCES:
            // Implementation for listing instances
            break;
        default:
            return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCManager::handleAccountRequest(const IPCRequest& request) {
    switch (request.method) {
        case ServiceMethod::LIST_ACCOUNTS:
            // Implementation for listing accounts
            break;
        case ServiceMethod::ADD_ACCOUNT:
            // Implementation for adding account
            break;
        default:
            return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCManager::handleJavaRequest(const IPCRequest& request) {
    switch (request.method) {
        case ServiceMethod::LIST_JAVA_INSTALLS:
            // Implementation for listing Java installs
            break;
        case ServiceMethod::GET_JAVA_INFO:
            // Implementation for getting Java info
            break;
        default:
            return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCManager::handleLaunchRequest(const IPCRequest& request) {
    switch (request.method) {
        case ServiceMethod::LAUNCH_INSTANCE:
            // Implementation for launching instance
            break;
        case ServiceMethod::STOP_INSTANCE:
            // Implementation for stopping instance
            break;
        default:
            return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCManager::handleSettingsRequest(const IPCRequest& request) {
    switch (request.method) {
        case ServiceMethod::GET_SETTINGS:
            // Implementation for getting settings
            break;
        case ServiceMethod::SET_SETTINGS:
            // Implementation for setting settings
            break;
        default:
            return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCManager::handleModRequest(const IPCRequest& request) {
    switch (request.method) {
        case ServiceMethod::SEARCH_MODS:
            // Implementation for searching mods
            break;
        case ServiceMethod::INSTALL_MOD:
            // Implementation for installing mod
            break;
        default:
            return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCManager::handleSystemRequest(const IPCRequest& request) {
    switch (request.method) {
        case ServiceMethod::GET_SYSTEM_INFO:
            // Implementation for getting system info
            break;
        case ServiceMethod::GET_LAUNCHER_INFO:
            // Implementation for getting launcher info
            break;
        default:
            return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

IPCResponse IPCManager::handleFileRequest(const IPCRequest& request) {
    switch (request.method) {
        case ServiceMethod::OPEN_FILE:
            // Implementation for opening file
            break;
        case ServiceMethod::OPEN_FOLDER:
            // Implementation for opening folder
            break;
        default:
            return IPCResponse(request.method, Status::INVALID_REQUEST, request.requestId);
    }
    return IPCResponse(request.method, Status::SUCCESS, request.requestId);
}

// Factory function implementation
std::unique_ptr<IPCInterface> createIPCInterface(const std::string& type) {
    if (type == "default" || type == "manager") {
        return std::make_unique<IPCManager>();
    }
    return nullptr;
}

} // namespace IPC
} // namespace PrismCore