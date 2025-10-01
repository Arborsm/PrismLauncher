// SPDX-License-Identifier: GPL-3.0-only
/*
 * IPC Client for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "Client.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

namespace IPC {

Client::Client() = default;

Client::~Client() = default;

void Client::registerCallback(const std::string& method, std::function<void(const Message&)> callback) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    m_callbacks[method] = callback;
}

void Client::unregisterCallback(const std::string& method) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    m_callbacks.erase(method);
}

void Client::handleMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    
    auto it = m_callbacks.find(message.method());
    if (it != m_callbacks.end()) {
        it->second(message);
    }
}

// UnixSocketClient 实现
UnixSocketClient::UnixSocketClient() = default;

UnixSocketClient::~UnixSocketClient() {
    disconnect();
}

bool UnixSocketClient::connect(const std::string& socketPath) {
    m_socketPath = socketPath;
    
    m_socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        return false;
    }
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
    
    if (::connect(m_socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }
    
    m_connected = true;
    m_messageThread = std::thread(&UnixSocketClient::messageLoop, this);
    
    return true;
}

void UnixSocketClient::disconnect() {
    m_connected = false;
    
    if (m_messageThread.joinable()) {
        m_messageThread.join();
    }
    
    if (m_socketFd >= 0) {
        close(m_socketFd);
        m_socketFd = -1;
    }
}

bool UnixSocketClient::isConnected() const {
    return m_connected;
}

Message UnixSocketClient::sendRequest(const Message& request) {
    auto future = sendRequestAsync(request);
    return future.get();
}

std::future<Message> UnixSocketClient::sendRequestAsync(const Message& request) {
    std::promise<Message> promise;
    auto future = promise.get_future();
    
    {
        std::lock_guard<std::mutex> lock(m_requestMutex);
        m_pendingRequests[request.id()] = std::move(promise);
    }
    
    if (!sendMessage(request)) {
        std::lock_guard<std::mutex> lock(m_requestMutex);
        auto it = m_pendingRequests.find(request.id());
        if (it != m_pendingRequests.end()) {
            Message errorResponse(MessageType::RESPONSE, request.method());
            errorResponse.setId(request.id());
            errorResponse.setError("Failed to send request");
            it->second.set_value(errorResponse);
            m_pendingRequests.erase(it);
        }
    }
    
    return future;
}

bool UnixSocketClient::sendNotification(const Message& notification) {
    return sendMessage(notification);
}

void UnixSocketClient::messageLoop() {
    while (m_connected) {
        try {
            Message message = receiveMessage();
            if (message.type() == MessageType::RESPONSE) {
                std::lock_guard<std::mutex> lock(m_requestMutex);
                auto it = m_pendingRequests.find(message.id());
                if (it != m_pendingRequests.end()) {
                    it->second.set_value(message);
                    m_pendingRequests.erase(it);
                }
            } else {
                handleMessage(message);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in message loop: " << e.what() << std::endl;
            break;
        }
    }
}

bool UnixSocketClient::sendMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_socketMutex);
    
    if (!m_connected || m_socketFd < 0) {
        return false;
    }
    
    std::string json = message.toJson();
    ssize_t sent = send(m_socketFd, json.c_str(), json.length(), 0);
    
    return sent == static_cast<ssize_t>(json.length());
}

Message UnixSocketClient::receiveMessage() {
    std::lock_guard<std::mutex> lock(m_socketMutex);
    
    if (!m_connected || m_socketFd < 0) {
        throw std::runtime_error("Not connected");
    }
    
    // 简化的消息接收实现
    char buffer[4096];
    ssize_t received = recv(m_socketFd, buffer, sizeof(buffer) - 1, 0);
    
    if (received <= 0) {
        throw std::runtime_error("Connection closed");
    }
    
    buffer[received] = '\0';
    return Message::fromJson(std::string(buffer));
}

// TcpClient 实现
TcpClient::TcpClient() = default;

TcpClient::~TcpClient() {
    disconnect();
}

bool TcpClient::connect(const std::string& host, int port) {
    m_host = host;
    m_port = port;
    return connect(host + ":" + std::to_string(port));
}

bool TcpClient::connect(const std::string& address) override {
    // 解析地址格式 "host:port"
    size_t pos = address.find(':');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string host = address.substr(0, pos);
    int port = std::stoi(address.substr(pos + 1));
    
    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        return false;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    
    if (::connect(m_socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }
    
    m_connected = true;
    m_messageThread = std::thread(&TcpClient::messageLoop, this);
    
    return true;
}

void TcpClient::disconnect() {
    m_connected = false;
    
    if (m_messageThread.joinable()) {
        m_messageThread.join();
    }
    
    if (m_socketFd >= 0) {
        close(m_socketFd);
        m_socketFd = -1;
    }
}

bool TcpClient::isConnected() const {
    return m_connected;
}

Message TcpClient::sendRequest(const Message& request) {
    auto future = sendRequestAsync(request);
    return future.get();
}

std::future<Message> TcpClient::sendRequestAsync(const Message& request) {
    std::promise<Message> promise;
    auto future = promise.get_future();
    
    {
        std::lock_guard<std::mutex> lock(m_requestMutex);
        m_pendingRequests[request.id()] = std::move(promise);
    }
    
    if (!sendMessage(request)) {
        std::lock_guard<std::mutex> lock(m_requestMutex);
        auto it = m_pendingRequests.find(request.id());
        if (it != m_pendingRequests.end()) {
            Message errorResponse(MessageType::RESPONSE, request.method());
            errorResponse.setId(request.id());
            errorResponse.setError("Failed to send request");
            it->second.set_value(errorResponse);
            m_pendingRequests.erase(it);
        }
    }
    
    return future;
}

bool TcpClient::sendNotification(const Message& notification) {
    return sendMessage(notification);
}

void TcpClient::messageLoop() {
    while (m_connected) {
        try {
            Message message = receiveMessage();
            if (message.type() == MessageType::RESPONSE) {
                std::lock_guard<std::mutex> lock(m_requestMutex);
                auto it = m_pendingRequests.find(message.id());
                if (it != m_pendingRequests.end()) {
                    it->second.set_value(message);
                    m_pendingRequests.erase(it);
                }
            } else {
                handleMessage(message);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in message loop: " << e.what() << std::endl;
            break;
        }
    }
}

bool TcpClient::sendMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_socketMutex);
    
    if (!m_connected || m_socketFd < 0) {
        return false;
    }
    
    std::string json = message.toJson();
    ssize_t sent = send(m_socketFd, json.c_str(), json.length(), 0);
    
    return sent == static_cast<ssize_t>(json.length());
}

Message TcpClient::receiveMessage() {
    std::lock_guard<std::mutex> lock(m_socketMutex);
    
    if (!m_connected || m_socketFd < 0) {
        throw std::runtime_error("Not connected");
    }
    
    // 简化的消息接收实现
    char buffer[4096];
    ssize_t received = recv(m_socketFd, buffer, sizeof(buffer) - 1, 0);
    
    if (received <= 0) {
        throw std::runtime_error("Connection closed");
    }
    
    buffer[received] = '\0';
    return Message::fromJson(std::string(buffer));
}

// NamedPipeClient 实现
NamedPipeClient::NamedPipeClient() = default;

NamedPipeClient::~NamedPipeClient() {
    disconnect();
}

bool NamedPipeClient::connect(const std::string& pipeName) {
    m_pipeName = pipeName;
    
    m_pipeFd = open(pipeName.c_str(), O_RDWR);
    if (m_pipeFd < 0) {
        return false;
    }
    
    m_connected = true;
    m_messageThread = std::thread(&NamedPipeClient::messageLoop, this);
    
    return true;
}

void NamedPipeClient::disconnect() {
    m_connected = false;
    
    if (m_messageThread.joinable()) {
        m_messageThread.join();
    }
    
    if (m_pipeFd >= 0) {
        close(m_pipeFd);
        m_pipeFd = -1;
    }
}

bool NamedPipeClient::isConnected() const {
    return m_connected;
}

Message NamedPipeClient::sendRequest(const Message& request) {
    auto future = sendRequestAsync(request);
    return future.get();
}

std::future<Message> NamedPipeClient::sendRequestAsync(const Message& request) {
    std::promise<Message> promise;
    auto future = promise.get_future();
    
    {
        std::lock_guard<std::mutex> lock(m_requestMutex);
        m_pendingRequests[request.id()] = std::move(promise);
    }
    
    if (!sendMessage(request)) {
        std::lock_guard<std::mutex> lock(m_requestMutex);
        auto it = m_pendingRequests.find(request.id());
        if (it != m_pendingRequests.end()) {
            Message errorResponse(MessageType::RESPONSE, request.method());
            errorResponse.setId(request.id());
            errorResponse.setError("Failed to send request");
            it->second.set_value(errorResponse);
            m_pendingRequests.erase(it);
        }
    }
    
    return future;
}

bool NamedPipeClient::sendNotification(const Message& notification) {
    return sendMessage(notification);
}

void NamedPipeClient::messageLoop() {
    while (m_connected) {
        try {
            Message message = receiveMessage();
            if (message.type() == MessageType::RESPONSE) {
                std::lock_guard<std::mutex> lock(m_requestMutex);
                auto it = m_pendingRequests.find(message.id());
                if (it != m_pendingRequests.end()) {
                    it->second.set_value(message);
                    m_pendingRequests.erase(it);
                }
            } else {
                handleMessage(message);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in message loop: " << e.what() << std::endl;
            break;
        }
    }
}

bool NamedPipeClient::sendMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_pipeMutex);
    
    if (!m_connected || m_pipeFd < 0) {
        return false;
    }
    
    std::string json = message.toJson();
    ssize_t sent = write(m_pipeFd, json.c_str(), json.length());
    
    return sent == static_cast<ssize_t>(json.length());
}

Message NamedPipeClient::receiveMessage() {
    std::lock_guard<std::mutex> lock(m_pipeMutex);
    
    if (!m_connected || m_pipeFd < 0) {
        throw std::runtime_error("Not connected");
    }
    
    // 简化的消息接收实现
    char buffer[4096];
    ssize_t received = read(m_pipeFd, buffer, sizeof(buffer) - 1);
    
    if (received <= 0) {
        throw std::runtime_error("Connection closed");
    }
    
    buffer[received] = '\0';
    return Message::fromJson(std::string(buffer));
}

} // namespace IPC