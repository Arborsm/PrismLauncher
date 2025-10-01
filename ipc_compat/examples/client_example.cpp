// SPDX-License-Identifier: GPL-3.0-only
/*
 * IPC Client Example for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "../ipc/UnixSocketClient.h"
#include "../ipc/Message.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "PrismLauncher IPC Client Example" << std::endl;
    
    try {
        // 创建客户端
        auto client = std::make_unique<IPC::UnixSocketClient>();
        
        // 连接到服务器
        if (!client->connect("/tmp/prismlauncher_ipc.sock")) {
            std::cerr << "Failed to connect to server" << std::endl;
            return 1;
        }
        
        std::cout << "Connected to server successfully" << std::endl;
        
        // 注册回调
        client->registerCallback("instance_launched", [](const IPC::Message& message) {
            std::cout << "Instance launched: " << message.toJson() << std::endl;
        });
        
        // 发送请求获取实例列表
        IPC::Message request(IPC::MessageType::REQUEST, "get_instance_list");
        auto response = client->sendRequest(request);
        
        std::cout << "Response: " << response.toJson() << std::endl;
        
        // 发送请求启动实例
        IPC::Message launchRequest(IPC::MessageType::REQUEST, "launch_instance");
        launchRequest.setParam("instance_id", std::string("test_instance"));
        launchRequest.setParam("account_id", std::string("test_account"));
        
        auto launchResponse = client->sendRequest(launchRequest);
        std::cout << "Launch response: " << launchResponse.toJson() << std::endl;
        
        // 发送通知
        IPC::Message notification(IPC::MessageType::NOTIFICATION, "client_ready");
        client->sendNotification(notification);
        
        // 等待一段时间
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // 断开连接
        client->disconnect();
        std::cout << "Disconnected from server" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}