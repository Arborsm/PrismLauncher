// SPDX-License-Identifier: GPL-3.0-only
/*
 * IPC Client for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#pragma once

#include "Message.h"
#include <memory>
#include <string>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>

namespace IPC {

// IPC客户端类
class Client {
public:
    Client();
    virtual ~Client();
    
    // 连接管理
    virtual bool connect(const std::string& address) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // 消息发送
    virtual Message sendRequest(const Message& request) = 0;
    virtual std::future<Message> sendRequestAsync(const Message& request) = 0;
    virtual bool sendNotification(const Message& notification) = 0;
    
    // 回调注册
    void registerCallback(const std::string& method, std::function<void(const Message&)> callback);
    void unregisterCallback(const std::string& method);
    
    // 超时设置
    void setTimeout(int timeoutMs) { m_timeout = timeoutMs; }
    int timeout() const { return m_timeout; }

protected:
    void handleMessage(const Message& message);
    
    std::map<std::string, std::function<void(const Message&)>> m_callbacks;
    std::mutex m_callbackMutex;
    int m_timeout = 5000; // 5秒默认超时
};

// Unix域套接字客户端
class UnixSocketClient : public Client {
public:
    UnixSocketClient();
    ~UnixSocketClient() override;
    
    bool connect(const std::string& socketPath) override;
    void disconnect() override;
    bool isConnected() const override;
    
    Message sendRequest(const Message& request) override;
    std::future<Message> sendRequestAsync(const Message& request) override;
    bool sendNotification(const Message& notification) override;

private:
    void messageLoop();
    bool sendMessage(const Message& message);
    Message receiveMessage();
    
    std::string m_socketPath;
    int m_socketFd = -1;
    std::atomic<bool> m_connected{false};
    std::thread m_messageThread;
    std::mutex m_socketMutex;
    
    // 待响应的请求
    std::map<std::string, std::promise<Message>> m_pendingRequests;
    std::mutex m_requestMutex;
};

// TCP客户端
class TcpClient : public Client {
public:
    TcpClient();
    ~TcpClient() override;
    
    bool connect(const std::string& host, int port);
    bool connect(const std::string& address) override;
    void disconnect() override;
    bool isConnected() const override;
    
    Message sendRequest(const Message& request) override;
    std::future<Message> sendRequestAsync(const Message& request) override;
    bool sendNotification(const Message& notification) override;

private:
    void messageLoop();
    bool sendMessage(const Message& message);
    Message receiveMessage();
    
    std::string m_host;
    int m_port = 0;
    int m_socketFd = -1;
    std::atomic<bool> m_connected{false};
    std::thread m_messageThread;
    std::mutex m_socketMutex;
    
    // 待响应的请求
    std::map<std::string, std::promise<Message>> m_pendingRequests;
    std::mutex m_requestMutex;
};

// 命名管道客户端
class NamedPipeClient : public Client {
public:
    NamedPipeClient();
    ~NamedPipeClient() override;
    
    bool connect(const std::string& pipeName) override;
    void disconnect() override;
    bool isConnected() const override;
    
    Message sendRequest(const Message& request) override;
    std::future<Message> sendRequestAsync(const Message& request) override;
    bool sendNotification(const Message& notification) override;

private:
    void messageLoop();
    bool sendMessage(const Message& message);
    Message receiveMessage();
    
    std::string m_pipeName;
    int m_pipeFd = -1;
    std::atomic<bool> m_connected{false};
    std::thread m_messageThread;
    std::mutex m_pipeMutex;
    
    // 待响应的请求
    std::map<std::string, std::promise<Message>> m_pendingRequests;
    std::mutex m_requestMutex;
};

} // namespace IPC