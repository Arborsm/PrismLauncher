// SPDX-License-Identifier: GPL-3.0-only
/*
 * IPC Message Tests
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "../ipc/Message.h"
#include <iostream>
#include <cassert>

void testMessageCreation() {
    std::cout << "Testing message creation..." << std::endl;
    
    IPC::Message request(IPC::MessageType::REQUEST, "test_method");
    
    assert(request.type() == IPC::MessageType::REQUEST);
    assert(request.method() == "test_method");
    assert(!request.id().empty());
    
    std::cout << "Message creation: PASSED" << std::endl;
}

void testMessageParameters() {
    std::cout << "Testing message parameters..." << std::endl;
    
    IPC::Message message(IPC::MessageType::REQUEST, "test_method");
    
    // 设置参数
    message.setParam("string_param", std::string("hello"));
    message.setParam("int_param", 42);
    message.setParam("bool_param", true);
    message.setParam("double_param", 3.14);
    
    // 检查参数
    assert(message.hasParam("string_param") == true);
    assert(message.hasParam("nonexistent") == false);
    
    auto stringVal = std::get<std::string>(message.getParam("string_param"));
    assert(stringVal == "hello");
    
    auto intVal = std::get<int32_t>(message.getParam("int_param"));
    assert(intVal == 42);
    
    auto boolVal = std::get<bool>(message.getParam("bool_param"));
    assert(boolVal == true);
    
    auto doubleVal = std::get<double>(message.getParam("double_param"));
    assert(doubleVal == 3.14);
    
    std::cout << "Message parameters: PASSED" << std::endl;
}

void testMessageSerialization() {
    std::cout << "Testing message serialization..." << std::endl;
    
    IPC::Message original(IPC::MessageType::REQUEST, "test_method");
    original.setParam("test_param", std::string("test_value"));
    original.setResult(std::string("test_result"));
    
    // 序列化
    std::string json = original.toJson();
    assert(!json.empty());
    
    // 反序列化
    IPC::Message deserialized = IPC::Message::fromJson(json);
    
    assert(deserialized.type() == original.type());
    assert(deserialized.method() == original.method());
    assert(deserialized.id() == original.id());
    
    std::cout << "Message serialization: PASSED" << std::endl;
}

void testMessageTypes() {
    std::cout << "Testing message types..." << std::endl;
    
    // 请求消息
    IPC::Message request(IPC::MessageType::REQUEST, "request_method");
    assert(request.type() == IPC::MessageType::REQUEST);
    
    // 响应消息
    IPC::Message response(IPC::MessageType::RESPONSE, "response_method");
    assert(response.type() == IPC::MessageType::RESPONSE);
    
    // 通知消息
    IPC::Message notification(IPC::MessageType::NOTIFICATION, "notification_method");
    assert(notification.type() == IPC::MessageType::NOTIFICATION);
    
    std::cout << "Message types: PASSED" << std::endl;
}

void testMessageErrorHandling() {
    std::cout << "Testing message error handling..." << std::endl;
    
    IPC::Message message(IPC::MessageType::RESPONSE, "error_method");
    message.setError("Test error message");
    
    assert(message.error() == "Test error message");
    
    std::cout << "Message error handling: PASSED" << std::endl;
}

int main() {
    std::cout << "Running IPC Message Tests" << std::endl;
    
    try {
        testMessageCreation();
        testMessageParameters();
        testMessageSerialization();
        testMessageTypes();
        testMessageErrorHandling();
        
        std::cout << "\nAll tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}