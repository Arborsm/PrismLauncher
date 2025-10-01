#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace PrismCore {

// Forward declarations
class InstanceList;
class InstanceTask;

// Base instance class without Qt dependencies
class BaseInstance {
public:
    virtual ~BaseInstance() = default;
    
    // Core properties
    virtual std::string id() const = 0;
    virtual std::string name() const = 0;
    virtual std::string instancePath() const = 0;
    virtual std::string iconPath() const = 0;
    
    // Instance management
    virtual bool isValid() const = 0;
    virtual bool isRunning() const = 0;
    virtual std::string status() const = 0;
    
    // Settings
    virtual std::map<std::string, std::string> getSettings() const = 0;
    virtual void setSettings(const std::map<std::string, std::string>& settings) = 0;
    virtual std::string getSetting(const std::string& key, const std::string& defaultValue = "") const = 0;
    virtual void setSetting(const std::string& key, const std::string& value) = 0;
    
    // File operations
    virtual bool createInstance() = 0;
    virtual bool deleteInstance() = 0;
    virtual bool copyInstance(const std::string& newName) = 0;
    
    // Launch operations
    virtual bool launch() = 0;
    virtual bool stop() = 0;
    virtual int getProcessId() const = 0;
    
    // Metadata
    virtual std::map<std::string, std::string> getMetadata() const = 0;
    virtual void setMetadata(const std::map<std::string, std::string>& metadata) = 0;
    
    // Timestamps
    virtual std::chrono::system_clock::time_point getCreatedTime() const = 0;
    virtual std::chrono::system_clock::time_point getLastPlayedTime() const = 0;
    virtual void setLastPlayedTime(const std::chrono::system_clock::time_point& time) = 0;
    
    // Tasks
    virtual std::shared_ptr<InstanceTask> createTask() = 0;
    
    // Events
    virtual void onStatusChanged(const std::string& status) = 0;
    virtual void onError(const std::string& error) = 0;
};

// Instance pointer type
using InstancePtr = std::shared_ptr<BaseInstance>;

// Instance list interface
class InstanceList {
public:
    virtual ~InstanceList() = default;
    
    virtual std::vector<InstancePtr> getInstances() const = 0;
    virtual InstancePtr getInstance(const std::string& id) const = 0;
    virtual InstancePtr createInstance(const std::string& name, const std::string& type = "minecraft") = 0;
    virtual bool deleteInstance(const std::string& id) = 0;
    virtual bool copyInstance(const std::string& id, const std::string& newName) = 0;
    
    virtual void reloadInstances() = 0;
    virtual void saveInstances() = 0;
    
    // Events
    virtual void onInstanceAdded(InstancePtr instance) = 0;
    virtual void onInstanceRemoved(const std::string& id) = 0;
    virtual void onInstanceUpdated(InstancePtr instance) = 0;
};

// Base task interface
class InstanceTask {
public:
    virtual ~InstanceTask() = default;
    
    virtual bool execute() = 0;
    virtual void cancel() = 0;
    virtual bool isRunning() const = 0;
    virtual bool isCompleted() const = 0;
    virtual bool hasError() const = 0;
    virtual std::string getError() const = 0;
    virtual std::string getStatus() const = 0;
    virtual double getProgress() const = 0;
    
    // Events
    virtual void onProgress(double progress) = 0;
    virtual void onStatusChanged(const std::string& status) = 0;
    virtual void onCompleted() = 0;
    virtual void onError(const std::string& error) = 0;
};

} // namespace PrismCore