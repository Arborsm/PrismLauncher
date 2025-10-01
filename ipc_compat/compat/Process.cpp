// SPDX-License-Identifier: GPL-3.0-only
/*
 * Process Compatibility Layer for PrismLauncher
 * Copyright (C) 2024 IPC Compat Layer
 */

#include "Process.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

namespace Compat {

Process::Process() = default;

Process::~Process() {
    if (isRunning()) {
        kill();
        waitForFinished(5000);
    }
}

void Process::setProgram(const String& program) {
    m_program = program;
}

void Process::setArguments(const StringList& arguments) {
    m_arguments = arguments;
}

void Process::setWorkingDirectory(const String& directory) {
    m_workingDirectory = directory;
}

void Process::setEnvironment(const std::map<String, String>& environment) {
    m_environment = environment;
}

bool Process::start() {
    if (m_program.isEmpty()) {
        m_errorString = "No program set";
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_processMutex);
    
    if (m_state != ProcessState::NotRunning) {
        m_errorString = "Process already running";
        return false;
    }
    
    m_state = ProcessState::Starting;
    m_shouldStop = false;
    
    startProcess();
    
    if (m_state == ProcessState::Running) {
        if (m_onStarted) {
            m_onStarted();
        }
        return true;
    } else {
        return false;
    }
}

bool Process::start(const String& program, const StringList& arguments) {
    setProgram(program);
    setArguments(arguments);
    return start();
}

bool Process::kill() {
    std::lock_guard<std::mutex> lock(m_processMutex);
    
    if (m_state != ProcessState::Running || m_processId <= 0) {
        return false;
    }
    
    if (::kill(m_processId, SIGKILL) == 0) {
        m_state = ProcessState::Finished;
        m_exitStatus = ExitStatus::CrashExit;
        return true;
    }
    
    return false;
}

bool Process::terminate() {
    std::lock_guard<std::mutex> lock(m_processMutex);
    
    if (m_state != ProcessState::Running || m_processId <= 0) {
        return false;
    }
    
    if (::kill(m_processId, SIGTERM) == 0) {
        return true;
    }
    
    return false;
}

void Process::close() {
    std::lock_guard<std::mutex> lock(m_processMutex);
    
    if (m_state == ProcessState::Running) {
        terminate();
        waitForFinished(5000);
    }
    
    cleanupProcess();
}

bool Process::waitForFinished(int msecs) {
    if (m_state == ProcessState::NotRunning) {
        return true;
    }
    
    if (m_monitorThread.joinable()) {
        m_monitorThread.join();
    }
    
    return m_state == ProcessState::Finished;
}

bool Process::waitForStarted(int msecs) {
    // 简化实现，假设启动是同步的
    return m_state == ProcessState::Running;
}

void Process::setStandardInputFile(const String& fileName) {
    m_stdInputFile = fileName;
}

void Process::setStandardOutputFile(const String& fileName) {
    m_stdOutputFile = fileName;
}

void Process::setStandardErrorFile(const String& fileName) {
    m_stdErrorFile = fileName;
}

String Process::readAllStandardOutput() {
    std::lock_guard<std::mutex> lock(m_outputMutex);
    String result = m_stdOutputBuffer;
    m_stdOutputBuffer.clear();
    return result;
}

String Process::readAllStandardError() {
    std::lock_guard<std::mutex> lock(m_outputMutex);
    String result = m_stdErrorBuffer;
    m_stdErrorBuffer.clear();
    return result;
}

String Process::readLine() {
    std::lock_guard<std::mutex> lock(m_outputMutex);
    
    size_t pos = m_stdOutputBuffer.find('\n');
    if (pos != std::string::npos) {
        String line = m_stdOutputBuffer.left(static_cast<int>(pos));
        m_stdOutputBuffer = m_stdOutputBuffer.mid(static_cast<int>(pos + 1));
        return line;
    }
    
    return String();
}

bool Process::write(const String& data) {
    // 简化实现，实际应该写入到进程的标准输入
    return true;
}

bool Process::writeLine(const String& data) {
    return write(data + "\n");
}

void Process::closeWriteChannel() {
    // 简化实现
}

void Process::onStarted(std::function<void()> callback) {
    m_onStarted = callback;
}

void Process::onFinished(std::function<void(int, ExitStatus)> callback) {
    m_onFinished = callback;
}

void Process::onErrorOccurred(std::function<void()> callback) {
    m_onErrorOccurred = callback;
}

void Process::onReadyReadStandardOutput(std::function<void()> callback) {
    m_onReadyReadStandardOutput = callback;
}

void Process::onReadyReadStandardError(std::function<void()> callback) {
    m_onReadyReadStandardError = callback;
}

void Process::startProcess() {
    // 创建管道用于进程间通信
    int pipe_stdin[2], pipe_stdout[2], pipe_stderr[2];
    
    if (pipe(pipe_stdin) == -1 || pipe(pipe_stdout) == -1 || pipe(pipe_stderr) == -1) {
        m_errorString = "Failed to create pipes";
        m_state = ProcessState::NotRunning;
        return;
    }
    
    // 创建子进程
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子进程
        close(pipe_stdin[1]);   // 关闭写端
        close(pipe_stdout[0]);  // 关闭读端
        close(pipe_stderr[0]);  // 关闭读端
        
        // 重定向标准输入输出
        dup2(pipe_stdin[0], STDIN_FILENO);
        dup2(pipe_stdout[1], STDOUT_FILENO);
        dup2(pipe_stderr[1], STDERR_FILENO);
        
        // 设置工作目录
        if (!m_workingDirectory.isEmpty()) {
            chdir(m_workingDirectory.c_str());
        }
        
        // 设置环境变量
        for (const auto& [key, value] : m_environment) {
            setenv(key.c_str(), value.c_str(), 1);
        }
        
        // 准备参数
        std::vector<char*> args;
        args.push_back(const_cast<char*>(m_program.c_str()));
        for (const auto& arg : m_arguments) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr);
        
        // 执行程序
        execvp(m_program.c_str(), args.data());
        
        // 如果execvp失败
        exit(1);
        
    } else if (pid > 0) {
        // 父进程
        m_processId = pid;
        
        close(pipe_stdin[0]);   // 关闭读端
        close(pipe_stdout[1]);   // 关闭写端
        close(pipe_stderr[1]);   // 关闭写端
        
        m_stdInputHandle = reinterpret_cast<void*>(pipe_stdin[1]);
        m_stdOutputHandle = reinterpret_cast<void*>(pipe_stdout[0]);
        m_stdErrorHandle = reinterpret_cast<void*>(pipe_stderr[0]);
        
        m_state = ProcessState::Running;
        
        // 启动监控线程
        m_monitorThread = std::thread(&Process::monitorProcess, this);
        
    } else {
        // fork失败
        m_errorString = "Failed to fork process";
        m_state = ProcessState::NotRunning;
        
        close(pipe_stdin[0]);
        close(pipe_stdin[1]);
        close(pipe_stdout[0]);
        close(pipe_stdout[1]);
        close(pipe_stderr[0]);
        close(pipe_stderr[1]);
    }
}

void Process::monitorProcess() {
    int status;
    pid_t result = waitpid(m_processId, &status, 0);
    
    std::lock_guard<std::mutex> lock(m_processMutex);
    
    if (result == m_processId) {
        m_state = ProcessState::Finished;
        
        if (WIFEXITED(status)) {
            m_exitCode = WEXITSTATUS(status);
            m_exitStatus = ExitStatus::NormalExit;
        } else if (WIFSIGNALED(status)) {
            m_exitCode = WTERMSIG(status);
            m_exitStatus = ExitStatus::CrashExit;
        }
        
        if (m_onFinished) {
            m_onFinished(m_exitCode, m_exitStatus);
        }
    }
    
    cleanupProcess();
}

void Process::handleProcessOutput() {
    // 简化实现，实际应该从管道读取数据
}

void Process::handleProcessError() {
    // 简化实现，实际应该从错误管道读取数据
}

void Process::cleanupProcess() {
    if (m_stdInputHandle) {
        close(reinterpret_cast<intptr_t>(m_stdInputHandle));
        m_stdInputHandle = nullptr;
    }
    
    if (m_stdOutputHandle) {
        close(reinterpret_cast<intptr_t>(m_stdOutputHandle));
        m_stdOutputHandle = nullptr;
    }
    
    if (m_stdErrorHandle) {
        close(reinterpret_cast<intptr_t>(m_stdErrorHandle));
        m_stdErrorHandle = nullptr;
    }
    
    m_processId = -1;
}

// ProcessEnvironment 实现
ProcessEnvironment::ProcessEnvironment() = default;

ProcessEnvironment::ProcessEnvironment(const ProcessEnvironment& other)
    : m_variables(other.m_variables) {
}

ProcessEnvironment& ProcessEnvironment::operator=(const ProcessEnvironment& other) {
    if (this != &other) {
        m_variables = other.m_variables;
    }
    return *this;
}

void ProcessEnvironment::insert(const String& key, const String& value) {
    m_variables[key] = value;
}

void ProcessEnvironment::remove(const String& key) {
    m_variables.erase(key);
}

bool ProcessEnvironment::contains(const String& key) const {
    return m_variables.find(key) != m_variables.end();
}

String ProcessEnvironment::value(const String& key, const String& defaultValue) const {
    auto it = m_variables.find(key);
    return it != m_variables.end() ? it->second : defaultValue;
}

std::map<String, String> ProcessEnvironment::toMap() const {
    return m_variables;
}

ProcessEnvironment ProcessEnvironment::systemEnvironment() {
    ProcessEnvironment env;
    
    extern char** environ;
    for (char** envp = environ; *envp; ++envp) {
        String envStr = String(*envp);
        int pos = envStr.indexOf('=');
        if (pos > 0) {
            String key = envStr.left(pos);
            String value = envStr.mid(pos + 1);
            env.insert(key, value);
        }
    }
    
    return env;
}

void ProcessEnvironment::clear() {
    m_variables.clear();
}

} // namespace Compat