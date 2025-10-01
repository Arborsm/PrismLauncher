#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <string>
#include <vector>
#include <functional>

// 前向声明
class InstanceList;
class BaseInstance;

namespace winrt::PrismLauncherWinUI3::implementation
{
    // 实例列表包装器类
    class InstanceListWrapper
    {
    public:
        InstanceListWrapper(std::shared_ptr<InstanceList> instanceList);
        ~InstanceListWrapper();

        // 获取实例列表
        std::vector<std::shared_ptr<BaseInstance>> GetInstances() const;
        std::shared_ptr<BaseInstance> GetInstanceById(const std::wstring& id) const;
        int GetCount() const;

        // 实例操作
        bool CreateInstance(const std::wstring& name, const std::wstring& version);
        bool DeleteInstance(const std::wstring& id);
        bool CopyInstance(const std::wstring& sourceId, const std::wstring& newName);
        bool RenameInstance(const std::wstring& id, const std::wstring& newName);

        // 分组管理
        std::vector<std::wstring> GetGroups() const;
        std::wstring GetInstanceGroup(const std::wstring& id) const;
        void SetInstanceGroup(const std::wstring& id, const std::wstring& group);
        void DeleteGroup(const std::wstring& group);
        void RenameGroup(const std::wstring& oldName, const std::wstring& newName);

        // 事件回调
        using InstanceListChangedCallback = std::function<void()>;
        void SetInstanceListChangedCallback(InstanceListChangedCallback callback);

        // 获取底层实例列表
        std::shared_ptr<InstanceList> GetInstanceList() const { return m_instanceList; }

    private:
        std::shared_ptr<InstanceList> m_instanceList;
        InstanceListChangedCallback m_callback;
    };
}