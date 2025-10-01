// SPDX-License-Identifier: GPL-3.0-only
/*
 * IPC Message System for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "Message.h"
#include <sstream>
#include <iomanip>
#include <random>
#include <functional>

namespace IPC {

Message::Message(MessageType type, const std::string& method)
    : m_type(type), m_method(method) {
    // 生成唯一ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 16; ++i) {
        ss << dis(gen);
    }
    m_id = ss.str();
}

void Message::setParam(const std::string& key, const Value& value) {
    m_params[key] = value;
}

Value Message::getParam(const std::string& key) const {
    auto it = m_params.find(key);
    if (it != m_params.end()) {
        return it->second;
    }
    return nullptr;
}

bool Message::hasParam(const std::string& key) const {
    return m_params.find(key) != m_params.end();
}

std::string Message::toJson() const {
    std::ostringstream oss;
    oss << "{";
    
    // type
    oss << "\"type\":\"";
    switch (m_type) {
        case MessageType::REQUEST: oss << "request"; break;
        case MessageType::RESPONSE: oss << "response"; break;
        case MessageType::NOTIFICATION: oss << "notification"; break;
    }
    oss << "\"";
    
    // id
    if (!m_id.empty()) {
        oss << ",\"id\":\"" << m_id << "\"";
    }
    
    // method
    if (!m_method.empty()) {
        oss << ",\"method\":\"" << m_method << "\"";
    }
    
    // params
    if (!m_params.empty()) {
        oss << ",\"params\":{";
        bool first = true;
        for (const auto& [key, value] : m_params) {
            if (!first) oss << ",";
            oss << "\"" << key << "\":";
            // 简单的值序列化
            if (std::holds_alternative<std::string>(value)) {
                oss << "\"" << std::get<std::string>(value) << "\"";
            } else if (std::holds_alternative<bool>(value)) {
                oss << (std::get<bool>(value) ? "true" : "false");
            } else if (std::holds_alternative<int32_t>(value)) {
                oss << std::get<int32_t>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                oss << std::get<int64_t>(value);
            } else if (std::holds_alternative<double>(value)) {
                oss << std::get<double>(value);
            } else {
                oss << "null";
            }
            first = false;
        }
        oss << "}";
    }
    
    // result
    if (!std::holds_alternative<std::nullptr_t>(m_result)) {
        oss << ",\"result\":";
        // 简单的结果序列化
        if (std::holds_alternative<std::string>(m_result)) {
            oss << "\"" << std::get<std::string>(m_result) << "\"";
        } else if (std::holds_alternative<bool>(m_result)) {
            oss << (std::get<bool>(m_result) ? "true" : "false");
        } else if (std::holds_alternative<int32_t>(m_result)) {
            oss << std::get<int32_t>(m_result);
        } else if (std::holds_alternative<int64_t>(m_result)) {
            oss << std::get<int64_t>(m_result);
        } else if (std::holds_alternative<double>(m_result)) {
            oss << std::get<double>(m_result);
        } else {
            oss << "null";
        }
    }
    
    // error
    if (!m_error.empty()) {
        oss << ",\"error\":\"" << m_error << "\"";
    }
    
    oss << "}";
    return oss.str();
}

Message Message::fromJson(const std::string& json) {
    // 简化的JSON解析 - 实际实现中应使用专门的JSON库
    Message msg;
    
    // 这里应该实现完整的JSON解析
    // 为了简化，只实现基本结构
    
    size_t pos = json.find("\"type\":\"");
    if (pos != std::string::npos) {
        pos += 8;
        size_t end = json.find("\"", pos);
        if (end != std::string::npos) {
            std::string type = json.substr(pos, end - pos);
            if (type == "request") msg.m_type = MessageType::REQUEST;
            else if (type == "response") msg.m_type = MessageType::RESPONSE;
            else if (type == "notification") msg.m_type = MessageType::NOTIFICATION;
        }
    }
    
    pos = json.find("\"id\":\"");
    if (pos != std::string::npos) {
        pos += 6;
        size_t end = json.find("\"", pos);
        if (end != std::string::npos) {
            msg.m_id = json.substr(pos, end - pos);
        }
    }
    
    pos = json.find("\"method\":\"");
    if (pos != std::string::npos) {
        pos += 10;
        size_t end = json.find("\"", pos);
        if (end != std::string::npos) {
            msg.m_method = json.substr(pos, end - pos);
        }
    }
    
    return msg;
}

} // namespace IPC