// SPDX-License-Identifier: GPL-3.0-only
/*
 * Process Compatibility Layer for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#pragma once

#include "String.h"
#include <map>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

namespace Compat {

// 进程状态
enum class ProcessState {
    NotRunning,
    Starting,
    Running,
    Finished
};

// 进程退出状态
enum class ExitStatus {
    NormalExit,
    CrashExit
};

// 替代QProcess的进程类
class Process {
public:
    Process();
    virtual ~Process();
    
    // 设置程序和工作目录
    void setProgram(const String& program);
    void setArguments(const StringList& arguments);
    void setWorkingDirectory(const String& directory);
    void setEnvironment(const std::map<String, String>& environment);
    
    // 获取设置
    String program() const { return m_program; }
    StringList arguments() const { return m_arguments; }
    String workingDirectory() const { return m_workingDirectory; }
    
    // 启动进程
    bool start();
    bool start(const String& program, const StringList& arguments);
    
    // 进程控制
    bool kill();
    bool terminate();
    void close();
    
    // 等待进程结束
    bool waitForFinished(int msecs = 30000);
    bool waitForStarted(int msecs = 30000);
    
    // 进程状态
    ProcessState state() const { return m_state; }
    bool isRunning() const { return m_state == ProcessState::Running; }
    int exitCode() const { return m_exitCode; }
    ExitStatus exitStatus() const { return m_exitStatus; }
    
    // 标准输入输出
    void setStandardInputFile(const String& fileName);
    void setStandardOutputFile(const String& fileName);
    void setStandardErrorFile(const String& fileName);
    
    // 读取输出
    String readAllStandardOutput();
    String readAllStandardError();
    String readLine();
    
    // 写入输入
    bool write(const String& data);
    bool writeLine(const String& data);
    void closeWriteChannel();
    
    // 信号连接
    void onStarted(std::function<void()> callback);
    void onFinished(std::function<void(int, ExitStatus)> callback);
    void onErrorOccurred(std::function<void()> callback);
    void onReadyReadStandardOutput(std::function<void()> callback);
    void onReadyReadStandardError(std::function<void()> callback);
    
    // 错误处理
    String errorString() const { return m_errorString; }
    
    // 进程ID
    int processId() const { return m_processId; }

private:
    void startProcess();
    void monitorProcess();
    void handleProcessOutput();
    void handleProcessError();
    void cleanupProcess();
    
    String m_program;
    StringList m_arguments;
    String m_workingDirectory;
    std::map<String, String> m_environment;
    
    ProcessState m_state = ProcessState::NotRunning;
    int m_exitCode = 0;
    ExitStatus m_exitStatus = ExitStatus::NormalExit;
    String m_errorString;
    int m_processId = -1;
    
    // 标准输入输出
    String m_stdInputFile;
    String m_stdOutputFile;
    String m_stdErrorFile;
    
    // 输出缓冲区
    String m_stdOutputBuffer;
    String m_stdErrorBuffer;
    std::mutex m_outputMutex;
    
    // 回调函数
    std::function<void()> m_onStarted;
    std::function<void(int, ExitStatus)> m_onFinished;
    std::function<void()> m_onErrorOccurred;
    std::function<void()> m_onReadyReadStandardOutput;
    std::function<void()> m_onReadyReadStandardError;
    
    // 线程控制
    std::thread m_monitorThread;
    std::atomic<bool> m_shouldStop{false};
    std::mutex m_processMutex;
    
    // 进程句柄
    void* m_processHandle = nullptr;
    void* m_stdInputHandle = nullptr;
    void* m_stdOutputHandle = nullptr;
    void* m_stdErrorHandle = nullptr;
};

// 进程环境类
class ProcessEnvironment {
public:
    ProcessEnvironment();
    ProcessEnvironment(const ProcessEnvironment& other);
    ProcessEnvironment& operator=(const ProcessEnvironment& other);
    
    // 环境变量操作
    void insert(const String& key, const String& value);
    void remove(const String& key);
    bool contains(const String& key) const;
    String value(const String& key, const String& defaultValue = String()) const;
    
    // 获取所有环境变量
    std::map<String, String> toMap() const;
    
    // 从系统环境变量创建
    static ProcessEnvironment systemEnvironment();
    
    // 清空
    void clear();
    
    // 大小
    int size() const { return static_cast<int>(m_variables.size()); }
    bool isEmpty() const { return m_variables.empty(); }

private:
    std::map<String, String> m_variables;
};

} // namespace Compat