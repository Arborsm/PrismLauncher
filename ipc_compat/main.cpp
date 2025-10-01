// SPDX-License-Identifier: GPL-3.0-only
/*
 * Main Entry Point for PrismLauncher IPC
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "core/Application.h"
#include "launcher/LauncherCore.h"
#include "compat/String.h"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    std::cout << "PrismLauncher IPC Server Starting..." << std::endl;
    
    try {
        // 创建应用程序
        auto app = std::make_unique<Core::Application>(argc, argv);
        
        // 初始化应用程序
        if (!app->initialize()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return 1;
        }
        
        std::cout << "Application initialized successfully" << std::endl;
        std::cout << "Data path: " << app->dataPath().toStdString() << std::endl;
        std::cout << "Root path: " << app->rootPath().toStdString() << std::endl;
        
        // 创建启动器核心
        auto launcherCore = std::make_unique<Launcher::LauncherCore>();
        
        if (!launcherCore->initialize()) {
            std::cerr << "Failed to initialize launcher core" << std::endl;
            return 1;
        }
        
        std::cout << "Launcher core initialized successfully" << std::endl;
        
        // 显示实例列表
        auto instances = launcherCore->getInstances();
        std::cout << "Found " << instances.size() << " instances:" << std::endl;
        for (const auto& instance : instances) {
            std::cout << "  - " << instance.toStdString() << std::endl;
        }
        
        // 显示Java安装
        auto javaInstallations = launcherCore->getJavaInstallations();
        std::cout << "Found " << javaInstallations.size() << " Java installations:" << std::endl;
        for (const auto& java : javaInstallations) {
            std::cout << "  - " << java.toStdString() << std::endl;
        }
        
        // 显示可用版本
        auto versions = launcherCore->getAvailableVersions();
        std::cout << "Available versions:" << std::endl;
        for (const auto& version : versions) {
            std::cout << "  - " << version.toStdString() << std::endl;
        }
        
        // 显示模组平台
        auto platforms = launcherCore->getModPlatforms();
        std::cout << "Available mod platforms:" << std::endl;
        for (const auto& platform : platforms) {
            std::cout << "  - " << platform.toStdString() << std::endl;
        }
        
        std::cout << "Starting IPC server..." << std::endl;
        
        // 运行应用程序主循环
        int result = app->exec();
        
        std::cout << "Application finished with code: " << result << std::endl;
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}