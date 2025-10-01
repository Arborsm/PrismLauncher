// SPDX-License-Identifier: GPL-3.0-only
/*
 * String Compatibility Layer Tests
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "../compat/String.h"
#include <iostream>
#include <cassert>

void testStringBasic() {
    std::cout << "Testing basic string operations..." << std::endl;
    
    Compat::String str1("Hello");
    Compat::String str2("World");
    
    assert(str1.length() == 5);
    assert(str2.length() == 5);
    assert(str1.isEmpty() == false);
    assert(str1.isNull() == false);
    
    Compat::String str3 = str1 + " " + str2;
    assert(str3 == "Hello World");
    
    std::cout << "Basic string operations: PASSED" << std::endl;
}

void testStringOperations() {
    std::cout << "Testing string operations..." << std::endl;
    
    Compat::String str("Hello World");
    
    // 子字符串
    assert(str.left(5) == "Hello");
    assert(str.right(5) == "World");
    assert(str.mid(6, 5) == "World");
    
    // 查找
    assert(str.indexOf("World") == 6);
    assert(str.contains("Hello") == true);
    assert(str.startsWith("Hello") == true);
    assert(str.endsWith("World") == true);
    
    // 替换
    Compat::String replaced = str.replaced("World", "Universe");
    assert(replaced == "Hello Universe");
    
    // 大小写转换
    Compat::String upper = str.toUpper();
    Compat::String lower = str.toLower();
    assert(upper == "HELLO WORLD");
    assert(lower == "hello world");
    
    // 去除空白字符
    Compat::String trimmed = Compat::String("  Hello World  ").trimmed();
    assert(trimmed == "Hello World");
    
    std::cout << "String operations: PASSED" << std::endl;
}

void testStringList() {
    std::cout << "Testing string list..." << std::endl;
    
    Compat::StringList list;
    list.append("apple");
    list.append("banana");
    list.append("cherry");
    
    assert(list.size() == 3);
    assert(list.contains("banana") == true);
    assert(list.indexOf("cherry") == 2);
    
    // 连接
    Compat::String joined = list.join(", ");
    assert(joined == "apple, banana, cherry");
    
    // 过滤
    Compat::StringList filtered = list.filter("a");
    assert(filtered.size() == 2);
    
    std::cout << "String list: PASSED" << std::endl;
}

void testStringNumbers() {
    std::cout << "Testing string number conversions..." << std::endl;
    
    // 数字转字符串
    Compat::String str1 = Compat::String::number(42);
    Compat::String str2 = Compat::String::number(3.14);
    
    assert(str1 == "42");
    assert(str2 == "3.140000");
    
    // 字符串转数字
    Compat::String str3("123");
    Compat::String str4("45.67");
    
    bool ok;
    int intVal = str3.toInt(&ok);
    assert(ok == true);
    assert(intVal == 123);
    
    double doubleVal = str4.toDouble(&ok);
    assert(ok == true);
    assert(doubleVal == 45.67);
    
    std::cout << "String number conversions: PASSED" << std::endl;
}

void testStringFormatting() {
    std::cout << "Testing string formatting..." << std::endl;
    
    Compat::String template1("Hello %1");
    Compat::String result1 = template1.arg("World");
    assert(result1 == "Hello World");
    
    Compat::String template2("User %1 has %2 points");
    Compat::String result2 = template2.arg("Alice").arg("100");
    assert(result2 == "User Alice has 100 points");
    
    std::cout << "String formatting: PASSED" << std::endl;
}

int main() {
    std::cout << "Running String Compatibility Layer Tests" << std::endl;
    
    try {
        testStringBasic();
        testStringOperations();
        testStringList();
        testStringNumbers();
        testStringFormatting();
        
        std::cout << "\nAll tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}