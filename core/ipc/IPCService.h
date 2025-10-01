#pragma once

#include "IPCInterface.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <memory>

namespace PrismCore {
namespace IPC {

// IPC Service implementation for handling requests
class IPCService {
public:
    IPCService();
    virtual ~IPCService();
    
    bool initialize();
    void shutdown();
    bool isRunning() const;
    
    // Service methods
    IPCResponse processRequest(const IPCRequest& request);
    
    // Event broadcasting
    void broadcastEvent(const std::string& eventType, const std::string& data);
    
    // Service registration
    void registerService(std::shared_ptr<IPCInterface> service);
    
    // Instance management
    IPCResponse handleCreateInstance(const IPCRequest& request);
    IPCResponse handleDeleteInstance(const IPCRequest& request);
    IPCResponse handleListInstances(const IPCRequest& request);
    IPCResponse handleGetInstanceInfo(const IPCRequest& request);
    IPCResponse handleUpdateInstance(const IPCRequest& request);
    
    // Account management
    IPCResponse handleListAccounts(const IPCRequest& request);
    IPCResponse handleAddAccount(const IPCRequest& request);
    IPCResponse handleRemoveAccount(const IPCRequest& request);
    IPCResponse handleLoginAccount(const IPCRequest& request);
    IPCResponse handleLogoutAccount(const IPCRequest& request);
    IPCResponse handleGetAccountInfo(const IPCRequest& request);
    
    // Java management
    IPCResponse handleListJavaInstalls(const IPCRequest& request);
    IPCResponse handleGetJavaInfo(const IPCRequest& request);
    IPCResponse handleInstallJava(const IPCRequest& request);
    IPCResponse handleUninstallJava(const IPCRequest& request);
    IPCResponse handleCheckJavaCompatibility(const IPCRequest& request);
    
    // Launch control
    IPCResponse handleLaunchInstance(const IPCRequest& request);
    IPCResponse handleStopInstance(const IPCRequest& request);
    IPCResponse handleGetLaunchStatus(const IPCRequest& request);
    IPCResponse handleGetLaunchLogs(const IPCRequest& request);
    
    // Settings
    IPCResponse handleGetSettings(const IPCRequest& request);
    IPCResponse handleSetSettings(const IPCRequest& request);
    IPCResponse handleResetSettings(const IPCRequest& request);
    
    // Mod platform
    IPCResponse handleSearchMods(const IPCRequest& request);
    IPCResponse handleGetModInfo(const IPCRequest& request);
    IPCResponse handleInstallMod(const IPCRequest& request);
    IPCResponse handleUninstallMod(const IPCRequest& request);
    IPCResponse handleUpdateMod(const IPCRequest& request);
    
    // System info
    IPCResponse handleGetSystemInfo(const IPCRequest& request);
    IPCResponse handleGetLauncherInfo(const IPCRequest& request);
    
    // File operations
    IPCResponse handleOpenFile(const IPCRequest& request);
    IPCResponse handleOpenFolder(const IPCRequest& request);
    IPCResponse handleCopyFile(const IPCRequest& request);
    IPCResponse handleDeleteFile(const IPCRequest& request);

private:
    std::atomic<bool> m_running{false};
    std::vector<std::shared_ptr<IPCInterface>> m_services;
    std::mutex m_servicesMutex;
    
    // Data storage
    std::vector<InstanceInfo> m_instances;
    std::vector<AccountInfo> m_accounts;
    std::vector<JavaInfo> m_javaInstalls;
    std::map<std::string, LaunchStatus> m_launchStatuses;
    std::map<std::string, std::string> m_settings;
    std::vector<ModInfo> m_mods;
    SystemInfo m_systemInfo;
    
    std::mutex m_dataMutex;
    
    // Helper methods
    std::string generateId();
    bool validateRequest(const IPCRequest& request);
    std::string serializeData(const std::string& data);
    std::string deserializeData(const std::string& data);
    
    // Data management
    void loadInstances();
    void saveInstances();
    void loadAccounts();
    void saveAccounts();
    void loadSettings();
    void saveSettings();
    void loadSystemInfo();
};

} // namespace IPC
} // namespace PrismCore