// SPDX-License-Identifier: GPL-3.0-only
/*
 * IPC Message Bus for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#pragma once

#include "Message.h"
#include <memory>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

namespace IPC {

// 消息总线类
class MessageBus {
public:
    MessageBus();
    virtual ~MessageBus();
    
    // 启动/停止消息总线
    virtual bool start() = 0;
    virtual void stop() = 0;
    
    // 发送消息
    virtual bool sendMessage(const Message& message) = 0;
    
    // 注册消息处理器
    void registerHandler(const std::string& method, std::shared_ptr<MessageHandler> handler);
    void unregisterHandler(const std::string& method);
    
    // 注册消息回调
    void registerCallback(const std::string& method, MessageCallback callback);
    void unregisterCallback(const std::string& method);
    
    // 处理消息
    Message processMessage(const Message& request);
    
    // 等待消息
    bool waitForMessage(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));
    
    // 获取待处理消息
    std::vector<Message> getPendingMessages();
    
    // 清空消息队列
    void clearMessages();

protected:
    // 消息处理线程
    void messageLoop();
    
    // 处理单个消息
    void handleMessage(const Message& message);
    
    // 消息队列
    std::queue<Message> m_messageQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    
    // 消息处理器
    std::map<std::string, std::shared_ptr<MessageHandler>> m_handlers;
    std::map<std::string, MessageCallback> m_callbacks;
    std::mutex m_handlerMutex;
    
    // 线程控制
    std::atomic<bool> m_running{false};
    std::thread m_messageThread;
};

// Unix域套接字消息总线
class UnixSocketMessageBus : public MessageBus {
public:
    UnixSocketMessageBus(const std::string& socketPath);
    ~UnixSocketMessageBus() override;
    
    bool start() override;
    void stop() override;
    bool sendMessage(const Message& message) override;

private:
    std::string m_socketPath;
    int m_socketFd = -1;
    std::mutex m_socketMutex;
};

// TCP消息总线
class TcpMessageBus : public MessageBus {
public:
    TcpMessageBus(const std::string& host, int port);
    ~TcpMessageBus() override;
    
    bool start() override;
    void stop() override;
    bool sendMessage(const Message& message) override;

private:
    std::string m_host;
    int m_port;
    int m_socketFd = -1;
    std::mutex m_socketMutex;
};

// 命名管道消息总线
class NamedPipeMessageBus : public MessageBus {
public:
    NamedPipeMessageBus(const std::string& pipeName);
    ~NamedPipeMessageBus() override;
    
    bool start() override;
    void stop() override;
    bool sendMessage(const Message& message) override;

private:
    std::string m_pipeName;
    int m_pipeFd = -1;
    std::mutex m_pipeMutex;
};

} // namespace IPC