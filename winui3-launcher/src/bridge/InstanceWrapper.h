#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <string>

// 前向声明
class BaseInstance;

namespace winrt::PrismLauncherWinUI3::implementation
{
    // 实例包装器类，用于在WinRT和Qt之间转换
    class InstanceWrapper
    {
    public:
        InstanceWrapper(std::shared_ptr<BaseInstance> instance);
        ~InstanceWrapper();

        // 获取实例信息
        std::wstring GetId() const;
        std::wstring GetName() const;
        std::wstring GetVersion() const;
        std::wstring GetLastPlayed() const;
        bool IsRunning() const;
        bool IsBroken() const;
        bool HasUpdate() const;
        std::wstring GetIconKey() const;
        std::wstring GetGroup() const;
        int64_t GetTotalPlayTime() const;
        int64_t GetLastLaunch() const;

        // 设置实例信息
        void SetName(const std::wstring& name);
        void SetIconKey(const std::wstring& iconKey);
        void SetGroup(const std::wstring& group);

        // 操作
        bool Launch(bool online = true, bool demo = false);
        bool Kill();
        bool CanLaunch() const;
        bool CanEdit() const;
        bool CanExport() const;

        // 获取底层实例
        std::shared_ptr<BaseInstance> GetInstance() const { return m_instance; }

    private:
        std::shared_ptr<BaseInstance> m_instance;
    };
}