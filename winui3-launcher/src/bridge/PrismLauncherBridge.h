#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>

#include <memory>
#include <string>
#include <vector>
#include <functional>

// 前向声明Qt相关类
class QApplication;
class Application;
class InstanceList;
class BaseInstance;
class LaunchController;

namespace winrt::PrismLauncherWinUI3::implementation
{
    // 实例信息结构
    struct InstanceInfo
    {
        std::wstring id;
        std::wstring name;
        std::wstring version;
        std::wstring lastPlayed;
        bool isRunning;
        bool isBroken;
        bool hasUpdate;
        std::wstring iconKey;
        std::wstring group;
        int64_t totalPlayTime;
        int64_t lastLaunch;
    };

    // 启动参数结构
    struct LaunchParameters
    {
        std::wstring instanceId;
        bool online;
        bool demo;
        std::wstring offlineName;
        std::wstring targetServer;
        std::wstring targetWorld;
    };

    // 事件回调类型
    using InstanceListChangedCallback = std::function<void()>;
    using InstanceStatusChangedCallback = std::function<void(const std::wstring& instanceId, bool isRunning)>;
    using LaunchStatusCallback = std::function<void(const std::wstring& instanceId, bool success, const std::wstring& error)>;

    // 主要的桥接类
    class PrismLauncherBridge
    {
    public:
        PrismLauncherBridge();
        ~PrismLauncherBridge();

        // 初始化和清理
        bool Initialize();
        void Shutdown();

        // 实例管理
        std::vector<InstanceInfo> GetInstances();
        InstanceInfo GetInstanceById(const std::wstring& id);
        bool CreateInstance(const std::wstring& name, const std::wstring& version);
        bool DeleteInstance(const std::wstring& id);
        bool CopyInstance(const std::wstring& sourceId, const std::wstring& newName);
        bool RenameInstance(const std::wstring& id, const std::wstring& newName);

        // 实例启动
        bool LaunchInstance(const LaunchParameters& params);
        bool KillInstance(const std::wstring& id);
        bool IsInstanceRunning(const std::wstring& id);

        // 设置管理
        std::wstring GetSetting(const std::wstring& key);
        void SetSetting(const std::wstring& key, const std::wstring& value);
        void SaveSettings();

        // 事件回调
        void SetInstanceListChangedCallback(InstanceListChangedCallback callback);
        void SetInstanceStatusChangedCallback(InstanceStatusChangedCallback callback);
        void SetLaunchStatusCallback(LaunchStatusCallback callback);

        // 工具方法
        std::wstring GetDataPath();
        std::wstring GetLogPath();
        std::vector<std::wstring> GetAvailableVersions();
        std::vector<std::wstring> GetGroups();

    private:
        // Qt应用实例
        std::unique_ptr<QApplication> m_qtApp;
        std::unique_ptr<Application> m_prismApp;
        
        // 核心组件
        std::shared_ptr<InstanceList> m_instanceList;
        
        // 事件回调
        InstanceListChangedCallback m_instanceListChangedCallback;
        InstanceStatusChangedCallback m_instanceStatusChangedCallback;
        LaunchStatusCallback m_launchStatusCallback;

        // 内部方法
        void OnInstanceListChanged();
        void OnInstanceStatusChanged(const std::wstring& instanceId, bool isRunning);
        void OnLaunchStatusChanged(const std::wstring& instanceId, bool success, const std::wstring& error);
        
        // 辅助方法
        InstanceInfo ConvertQtInstanceToInfo(std::shared_ptr<BaseInstance> instance);
        std::wstring QStringToWString(const QString& qstr);
        QString WStringToQString(const std::wstring& wstr);
    };
}