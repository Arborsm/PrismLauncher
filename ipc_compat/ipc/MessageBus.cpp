// SPDX-License-Identifier: GPL-3.0-only
/*
 * IPC Message Bus for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "MessageBus.h"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

namespace IPC {

MessageBus::MessageBus() = default;

MessageBus::~MessageBus() {
    stop();
}

void MessageBus::registerHandler(const std::string& method, std::shared_ptr<MessageHandler> handler) {
    std::lock_guard<std::mutex> lock(m_handlerMutex);
    m_handlers[method] = handler;
}

void MessageBus::unregisterHandler(const std::string& method) {
    std::lock_guard<std::mutex> lock(m_handlerMutex);
    m_handlers.erase(method);
}

void MessageBus::registerCallback(const std::string& method, MessageCallback callback) {
    std::lock_guard<std::mutex> lock(m_handlerMutex);
    m_callbacks[method] = callback;
}

void MessageBus::unregisterCallback(const std::string& method) {
    std::lock_guard<std::mutex> lock(m_handlerMutex);
    m_callbacks.erase(method);
}

Message MessageBus::processMessage(const Message& request) {
    Message response(MessageType::RESPONSE, request.method());
    response.setId(request.id());
    
    try {
        std::lock_guard<std::mutex> lock(m_handlerMutex);
        
        // 查找处理器
        auto handlerIt = m_handlers.find(request.method());
        if (handlerIt != m_handlers.end()) {
            response = handlerIt->second->handle(request);
            return response;
        }
        
        // 查找回调
        auto callbackIt = m_callbacks.find(request.method());
        if (callbackIt != m_callbacks.end()) {
            callbackIt->second(request);
            response.setResult(true);
            return response;
        }
        
        // 未找到处理器
        response.setError("Method not found: " + request.method());
        
    } catch (const std::exception& e) {
        response.setError("Exception: " + std::string(e.what()));
    }
    
    return response;
}

bool MessageBus::waitForMessage(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    return m_queueCondition.wait_for(lock, timeout, [this] { return !m_messageQueue.empty(); });
}

std::vector<Message> MessageBus::getPendingMessages() {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    std::vector<Message> messages;
    
    while (!m_messageQueue.empty()) {
        messages.push_back(m_messageQueue.front());
        m_messageQueue.pop();
    }
    
    return messages;
}

void MessageBus::clearMessages() {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    std::queue<Message> empty;
    m_messageQueue.swap(empty);
}

void MessageBus::messageLoop() {
    while (m_running) {
        if (waitForMessage(std::chrono::milliseconds(100))) {
            auto messages = getPendingMessages();
            for (const auto& message : messages) {
                handleMessage(message);
            }
        }
    }
}

void MessageBus::handleMessage(const Message& message) {
    Message response = processMessage(message);
    if (message.type() == MessageType::REQUEST) {
        sendMessage(response);
    }
}

// Unix域套接字实现
UnixSocketMessageBus::UnixSocketMessageBus(const std::string& socketPath)
    : m_socketPath(socketPath) {
}

UnixSocketMessageBus::~UnixSocketMessageBus() {
    stop();
}

bool UnixSocketMessageBus::start() {
    // 创建Unix域套接字
    m_socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        return false;
    }
    
    // 绑定到套接字文件
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, m_socketPath.c_str(), sizeof(addr.sun_path) - 1);
    
    unlink(m_socketPath.c_str()); // 删除已存在的套接字文件
    
    if (bind(m_socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }
    
    if (listen(m_socketFd, 5) < 0) {
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }
    
    m_running = true;
    m_messageThread = std::thread(&MessageBus::messageLoop, this);
    
    return true;
}

void UnixSocketMessageBus::stop() {
    m_running = false;
    
    if (m_messageThread.joinable()) {
        m_messageThread.join();
    }
    
    if (m_socketFd >= 0) {
        close(m_socketFd);
        m_socketFd = -1;
    }
    
    unlink(m_socketPath.c_str());
}

bool UnixSocketMessageBus::sendMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_socketMutex);
    
    if (m_socketFd < 0) {
        return false;
    }
    
    std::string json = message.toJson();
    ssize_t sent = send(m_socketFd, json.c_str(), json.length(), 0);
    
    return sent == static_cast<ssize_t>(json.length());
}

// TCP实现
TcpMessageBus::TcpMessageBus(const std::string& host, int port)
    : m_host(host), m_port(port) {
}

TcpMessageBus::~TcpMessageBus() {
    stop();
}

bool TcpMessageBus::start() {
    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        return false;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    inet_pton(AF_INET, m_host.c_str(), &addr.sin_addr);
    
    if (bind(m_socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }
    
    if (listen(m_socketFd, 5) < 0) {
        close(m_socketFd);
        m_socketFd = -1;
        return false;
    }
    
    m_running = true;
    m_messageThread = std::thread(&MessageBus::messageLoop, this);
    
    return true;
}

void TcpMessageBus::stop() {
    m_running = false;
    
    if (m_messageThread.joinable()) {
        m_messageThread.join();
    }
    
    if (m_socketFd >= 0) {
        close(m_socketFd);
        m_socketFd = -1;
    }
}

bool TcpMessageBus::sendMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_socketMutex);
    
    if (m_socketFd < 0) {
        return false;
    }
    
    std::string json = message.toJson();
    ssize_t sent = send(m_socketFd, json.c_str(), json.length(), 0);
    
    return sent == static_cast<ssize_t>(json.length());
}

// 命名管道实现
NamedPipeMessageBus::NamedPipeMessageBus(const std::string& pipeName)
    : m_pipeName(pipeName) {
}

NamedPipeMessageBus::~NamedPipeMessageBus() {
    stop();
}

bool NamedPipeMessageBus::start() {
    if (mkfifo(m_pipeName.c_str(), 0666) < 0 && errno != EEXIST) {
        return false;
    }
    
    m_pipeFd = open(m_pipeName.c_str(), O_RDWR);
    if (m_pipeFd < 0) {
        return false;
    }
    
    m_running = true;
    m_messageThread = std::thread(&MessageBus::messageLoop, this);
    
    return true;
}

void NamedPipeMessageBus::stop() {
    m_running = false;
    
    if (m_messageThread.joinable()) {
        m_messageThread.join();
    }
    
    if (m_pipeFd >= 0) {
        close(m_pipeFd);
        m_pipeFd = -1;
    }
    
    unlink(m_pipeName.c_str());
}

bool NamedPipeMessageBus::sendMessage(const Message& message) {
    std::lock_guard<std::mutex> lock(m_pipeMutex);
    
    if (m_pipeFd < 0) {
        return false;
    }
    
    std::string json = message.toJson();
    ssize_t sent = write(m_pipeFd, json.c_str(), json.length());
    
    return sent == static_cast<ssize_t>(json.length());
}

} // namespace IPC