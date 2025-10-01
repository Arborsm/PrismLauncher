// SPDX-License-Identifier: GPL-3.0-only
/*
 * IPC Message System for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#pragma once

#include <string>
#include <map>
#include <variant>
#include <memory>
#include <chrono>

namespace IPC {

// 消息类型
enum class MessageType {
    REQUEST,      // 请求
    RESPONSE,     // 响应
    NOTIFICATION  // 通知
};

// 参数值类型
using Value = std::variant<
    std::nullptr_t,
    bool,
    int32_t,
    int64_t,
    double,
    std::string,
    std::vector<Value>,
    std::map<std::string, Value>
>;

// 消息类
class Message {
public:
    Message() = default;
    Message(MessageType type, const std::string& method);
    
    // 设置消息属性
    void setType(MessageType type) { m_type = type; }
    void setId(const std::string& id) { m_id = id; }
    void setMethod(const std::string& method) { m_method = method; }
    void setParams(const std::map<std::string, Value>& params) { m_params = params; }
    void setResult(const Value& result) { m_result = result; }
    void setError(const std::string& error) { m_error = error; }
    
    // 获取消息属性
    MessageType type() const { return m_type; }
    const std::string& id() const { return m_id; }
    const std::string& method() const { return m_method; }
    const std::map<std::string, Value>& params() const { return m_params; }
    const Value& result() const { return m_result; }
    const std::string& error() const { return m_error; }
    
    // 参数操作
    void setParam(const std::string& key, const Value& value);
    Value getParam(const std::string& key) const;
    bool hasParam(const std::string& key) const;
    
    // 序列化/反序列化
    std::string toJson() const;
    static Message fromJson(const std::string& json);
    
    // 时间戳
    void setTimestamp(std::chrono::system_clock::time_point timestamp) { m_timestamp = timestamp; }
    std::chrono::system_clock::time_point timestamp() const { return m_timestamp; }

private:
    MessageType m_type = MessageType::REQUEST;
    std::string m_id;
    std::string m_method;
    std::map<std::string, Value> m_params;
    Value m_result;
    std::string m_error;
    std::chrono::system_clock::time_point m_timestamp = std::chrono::system_clock::now();
};

// 消息处理器接口
class MessageHandler {
public:
    virtual ~MessageHandler() = default;
    virtual Message handle(const Message& request) = 0;
};

// 消息回调接口
using MessageCallback = std::function<void(const Message&)>;

} // namespace IPC