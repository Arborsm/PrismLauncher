// SPDX-License-Identifier: GPL-3.0-only
/*
 * String Compatibility Layer for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "String.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace Compat {

String String::mid(int position, int n) const {
    if (position < 0) position = 0;
    if (position >= static_cast<int>(m_data.length())) return String();
    
    if (n < 0) {
        n = static_cast<int>(m_data.length()) - position;
    }
    
    int end = std::min(position + n, static_cast<int>(m_data.length()));
    return String(m_data.substr(position, end - position));
}

String String::left(int n) const {
    if (n < 0) n = 0;
    n = std::min(n, static_cast<int>(m_data.length()));
    return String(m_data.substr(0, n));
}

String String::right(int n) const {
    if (n < 0) n = 0;
    int start = std::max(0, static_cast<int>(m_data.length()) - n);
    return String(m_data.substr(start));
}

int String::indexOf(const String& str, int from) const {
    if (from < 0) from = 0;
    size_t pos = m_data.find(str.m_data, from);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

int String::lastIndexOf(const String& str, int from) const {
    size_t searchFrom = from < 0 ? std::string::npos : from;
    size_t pos = m_data.rfind(str.m_data, searchFrom);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
}

bool String::contains(const String& str) const {
    return m_data.find(str.m_data) != std::string::npos;
}

bool String::startsWith(const String& str) const {
    return m_data.substr(0, str.m_data.length()) == str.m_data;
}

bool String::endsWith(const String& str) const {
    if (str.m_data.length() > m_data.length()) return false;
    return m_data.substr(m_data.length() - str.m_data.length()) == str.m_data;
}

String& String::replace(const String& before, const String& after) {
    size_t pos = 0;
    while ((pos = m_data.find(before.m_data, pos)) != std::string::npos) {
        m_data.replace(pos, before.m_data.length(), after.m_data);
        pos += after.m_data.length();
    }
    return *this;
}

String String::replaced(const String& before, const String& after) const {
    String result = *this;
    result.replace(before, after);
    return result;
}

String String::toUpper() const {
    std::string result = m_data;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return String(result);
}

String String::toLower() const {
    std::string result = m_data;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return String(result);
}

String String::trimmed() const {
    std::string result = m_data;
    
    // 去除前导空白字符
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    
    // 去除尾随空白字符
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), result.end());
    
    return String(result);
}

String String::simplified() const {
    std::string result = m_data;
    
    // 去除前导和尾随空白字符
    result = trimmed().m_data;
    
    // 将多个连续空白字符替换为单个空格
    std::string simplified;
    bool inWhitespace = false;
    for (char c : result) {
        if (std::isspace(c)) {
            if (!inWhitespace) {
                simplified += ' ';
                inWhitespace = true;
            }
        } else {
            simplified += c;
            inWhitespace = false;
        }
    }
    
    return String(simplified);
}

std::vector<String> String::split(const String& separator, SplitBehavior behavior) const {
    std::vector<String> result;
    std::string str = m_data;
    std::string sep = separator.m_data;
    
    size_t start = 0;
    size_t end = str.find(sep);
    
    while (end != std::string::npos) {
        String part = String(str.substr(start, end - start));
        if (behavior == KeepEmptyParts || !part.isEmpty()) {
            result.push_back(part);
        }
        start = end + sep.length();
        end = str.find(sep, start);
    }
    
    String lastPart = String(str.substr(start));
    if (behavior == KeepEmptyParts || !lastPart.isEmpty()) {
        result.push_back(lastPart);
    }
    
    return result;
}

String String::join(const std::vector<String>& list, const String& separator) {
    if (list.empty()) return String();
    
    std::string result = list[0].m_data;
    for (size_t i = 1; i < list.size(); ++i) {
        result += separator.m_data + list[i].m_data;
    }
    
    return String(result);
}

int String::toInt(bool* ok, int base) const {
    try {
        size_t pos;
        int result = std::stoi(m_data, &pos, base);
        if (ok) *ok = (pos == m_data.length());
        return result;
    } catch (...) {
        if (ok) *ok = false;
        return 0;
    }
}

long long String::toLongLong(bool* ok, int base) const {
    try {
        size_t pos;
        long long result = std::stoll(m_data, &pos, base);
        if (ok) *ok = (pos == m_data.length());
        return result;
    } catch (...) {
        if (ok) *ok = false;
        return 0;
    }
}

double String::toDouble(bool* ok) const {
    try {
        size_t pos;
        double result = std::stod(m_data, &pos);
        if (ok) *ok = (pos == m_data.length());
        return result;
    } catch (...) {
        if (ok) *ok = false;
        return 0.0;
    }
}

float String::toFloat(bool* ok) const {
    try {
        size_t pos;
        float result = std::stof(m_data, &pos);
        if (ok) *ok = (pos == m_data.length());
        return result;
    } catch (...) {
        if (ok) *ok = false;
        return 0.0f;
    }
}

String String::number(int n, int base) {
    std::stringstream ss;
    if (base == 16) {
        ss << std::hex << n;
    } else if (base == 8) {
        ss << std::oct << n;
    } else {
        ss << n;
    }
    return String(ss.str());
}

String String::number(long long n, int base) {
    std::stringstream ss;
    if (base == 16) {
        ss << std::hex << n;
    } else if (base == 8) {
        ss << std::oct << n;
    } else {
        ss << n;
    }
    return String(ss.str());
}

String String::number(double n, char format, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << n;
    return String(ss.str());
}

String String::arg(const String& a1) const {
    std::string result = m_data;
    size_t pos = result.find("%1");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a1.m_data);
    }
    return String(result);
}

String String::arg(const String& a1, const String& a2) const {
    std::string result = m_data;
    
    // 替换 %2 先，避免与 %1 冲突
    size_t pos = result.find("%2");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a2.m_data);
    }
    
    pos = result.find("%1");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a1.m_data);
    }
    
    return String(result);
}

String String::arg(const String& a1, const String& a2, const String& a3) const {
    std::string result = m_data;
    
    // 从后往前替换，避免位置冲突
    size_t pos = result.find("%3");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a3.m_data);
    }
    
    pos = result.find("%2");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a2.m_data);
    }
    
    pos = result.find("%1");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a1.m_data);
    }
    
    return String(result);
}

String String::arg(const String& a1, const String& a2, const String& a3, const String& a4) const {
    std::string result = m_data;
    
    // 从后往前替换，避免位置冲突
    size_t pos = result.find("%4");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a4.m_data);
    }
    
    pos = result.find("%3");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a3.m_data);
    }
    
    pos = result.find("%2");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a2.m_data);
    }
    
    pos = result.find("%1");
    if (pos != std::string::npos) {
        result.replace(pos, 2, a1.m_data);
    }
    
    return String(result);
}

// StringList 实现
bool StringList::contains(const String& str) const {
    return std::find(begin(), end(), str) != end();
}

int StringList::indexOf(const String& str, int from) const {
    if (from < 0) from = 0;
    auto it = std::find(begin() + from, end(), str);
    return it != end() ? static_cast<int>(it - begin()) : -1;
}

StringList StringList::filter(const String& pattern) const {
    StringList result;
    for (const auto& str : *this) {
        if (str.contains(pattern)) {
            result.push_back(str);
        }
    }
    return result;
}

String StringList::join(const String& separator) const {
    return String::join(*this, separator);
}

void StringList::sort() {
    std::sort(begin(), end());
}

void StringList::removeDuplicates() {
    std::sort(begin(), end());
    auto it = std::unique(begin(), end());
    erase(it, end());
}

void StringList::removeAll(const String& str) {
    erase(std::remove(begin(), end(), str), end());
}

void StringList::removeAt(int index) {
    if (index >= 0 && index < static_cast<int>(size())) {
        erase(begin() + index);
    }
}

void StringList::removeFirst() {
    if (!empty()) {
        erase(begin());
    }
}

void StringList::removeLast() {
    if (!empty()) {
        pop_back();
    }
}

void StringList::prepend(const String& str) {
    insert(begin(), str);
}

void StringList::append(const String& str) {
    push_back(str);
}

void StringList::insert(int index, const String& str) {
    if (index >= 0 && index <= static_cast<int>(size())) {
        std::vector<String>::insert(begin() + index, str);
    }
}

void StringList::replaceInStrings(const String& before, const String& after) {
    for (auto& str : *this) {
        str.replace(before, after);
    }
}

} // namespace Compat