// SPDX-License-Identifier: GPL-3.0-only
/*
 * String Compatibility Layer for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Compat {

// 替代QString的字符串类
class String {
public:
    String() = default;
    String(const char* str) : m_data(str ? str : "") {}
    String(const std::string& str) : m_data(str) {}
    String(const String& other) = default;
    String(String&& other) noexcept = default;
    
    String& operator=(const String& other) = default;
    String& operator=(String&& other) noexcept = default;
    
    // 转换操作符
    operator const std::string&() const { return m_data; }
    operator std::string&() { return m_data; }
    
    // 字符串操作
    bool isEmpty() const { return m_data.empty(); }
    bool isNull() const { return false; } // 始终返回false，因为我们使用空字符串
    int length() const { return static_cast<int>(m_data.length()); }
    int size() const { return static_cast<int>(m_data.size()); }
    
    // 访问字符
    char at(int index) const { return m_data.at(index); }
    char& operator[](int index) { return m_data[index]; }
    const char& operator[](int index) const { return m_data[index]; }
    
    // 子字符串
    String mid(int position, int n = -1) const;
    String left(int n) const;
    String right(int n) const;
    
    // 查找
    int indexOf(const String& str, int from = 0) const;
    int lastIndexOf(const String& str, int from = -1) const;
    bool contains(const String& str) const;
    bool startsWith(const String& str) const;
    bool endsWith(const String& str) const;
    
    // 替换
    String& replace(const String& before, const String& after);
    String replaced(const String& before, const String& after) const;
    
    // 大小写转换
    String toUpper() const;
    String toLower() const;
    
    // 去除空白字符
    String trimmed() const;
    String simplified() const;
    
    // 分割
    std::vector<String> split(const String& separator, SplitBehavior behavior = KeepEmptyParts) const;
    
    enum SplitBehavior {
        KeepEmptyParts,
        SkipEmptyParts
    };
    
    // 连接
    static String join(const std::vector<String>& list, const String& separator);
    
    // 数字转换
    int toInt(bool* ok = nullptr, int base = 10) const;
    long long toLongLong(bool* ok = nullptr, int base = 10) const;
    double toDouble(bool* ok = nullptr) const;
    float toFloat(bool* ok = nullptr) const;
    
    // 从数字创建
    static String number(int n, int base = 10);
    static String number(long long n, int base = 10);
    static String number(double n, char format = 'g', int precision = 6);
    
    // 格式化
    static String arg(const String& a1) const;
    static String arg(const String& a1, const String& a2) const;
    static String arg(const String& a1, const String& a2, const String& a3) const;
    static String arg(const String& a1, const String& a2, const String& a3, const String& a4) const;
    
    // 比较操作符
    bool operator==(const String& other) const { return m_data == other.m_data; }
    bool operator!=(const String& other) const { return m_data != other.m_data; }
    bool operator<(const String& other) const { return m_data < other.m_data; }
    bool operator<=(const String& other) const { return m_data <= other.m_data; }
    bool operator>(const String& other) const { return m_data > other.m_data; }
    bool operator>=(const String& other) const { return m_data >= other.m_data; }
    
    // 连接操作符
    String operator+(const String& other) const { return String(m_data + other.m_data); }
    String& operator+=(const String& other) { m_data += other.m_data; return *this; }
    
    // 流操作符
    friend std::ostream& operator<<(std::ostream& os, const String& str) {
        return os << str.m_data;
    }
    
    friend std::istream& operator>>(std::istream& is, String& str) {
        return is >> str.m_data;
    }
    
    // 获取C字符串
    const char* c_str() const { return m_data.c_str(); }
    const char* data() const { return m_data.data(); }
    
    // 获取std::string
    const std::string& toStdString() const { return m_data; }
    std::string& toStdString() { return m_data; }

private:
    std::string m_data;
};

// 字符串列表类
class StringList : public std::vector<String> {
public:
    StringList() = default;
    StringList(const std::vector<String>& list) : std::vector<String>(list) {}
    StringList(std::initializer_list<String> init) : std::vector<String>(init) {}
    
    // 查找
    bool contains(const String& str) const;
    int indexOf(const String& str, int from = 0) const;
    
    // 过滤
    StringList filter(const String& pattern) const;
    
    // 连接
    String join(const String& separator) const;
    
    // 排序
    void sort();
    
    // 去重
    void removeDuplicates();
    
    // 移除
    void removeAll(const String& str);
    void removeAt(int index);
    void removeFirst();
    void removeLast();
    
    // 插入
    void prepend(const String& str);
    void append(const String& str);
    void insert(int index, const String& str);
    
    // 替换
    void replaceInStrings(const String& before, const String& after);
};

} // namespace Compat