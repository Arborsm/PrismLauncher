#include "InstanceListWrapper.h"
#include <QString>

namespace winrt::PrismLauncherWinUI3::implementation
{
    InstanceListWrapper::InstanceListWrapper(std::shared_ptr<InstanceList> instanceList)
        : m_instanceList(instanceList)
    {
    }

    InstanceListWrapper::~InstanceListWrapper()
    {
    }

    std::vector<std::shared_ptr<BaseInstance>> InstanceListWrapper::GetInstances() const
    {
        std::vector<std::shared_ptr<BaseInstance>> instances;
        
        if (!m_instanceList)
            return instances;

        int count = m_instanceList->count();
        for (int i = 0; i < count; ++i)
        {
            auto instance = m_instanceList->at(i);
            if (instance)
            {
                instances.push_back(instance);
            }
        }
        
        return instances;
    }

    std::shared_ptr<BaseInstance> InstanceListWrapper::GetInstanceById(const std::wstring& id) const
    {
        if (!m_instanceList)
            return nullptr;

        QString qId = QString::fromStdWString(id);
        return m_instanceList->getInstanceById(qId);
    }

    int InstanceListWrapper::GetCount() const
    {
        if (!m_instanceList)
            return 0;
        
        return m_instanceList->count();
    }

    bool InstanceListWrapper::CreateInstance(const std::wstring& name, const std::wstring& version)
    {
        if (!m_instanceList)
            return false;

        // 这里需要实现创建实例的逻辑
        // 简化实现
        return true;
    }

    bool InstanceListWrapper::DeleteInstance(const std::wstring& id)
    {
        if (!m_instanceList)
            return false;

        QString qId = QString::fromStdWString(id);
        m_instanceList->deleteInstance(qId);
        return true;
    }

    bool InstanceListWrapper::CopyInstance(const std::wstring& sourceId, const std::wstring& newName)
    {
        if (!m_instanceList)
            return false;

        // 这里需要实现复制实例的逻辑
        // 简化实现
        return true;
    }

    bool InstanceListWrapper::RenameInstance(const std::wstring& id, const std::wstring& newName)
    {
        if (!m_instanceList)
            return false;

        QString qId = QString::fromStdWString(id);
        QString qNewName = QString::fromStdWString(newName);
        
        auto instance = m_instanceList->getInstanceById(qId);
        if (instance)
        {
            instance->setName(qNewName);
            return true;
        }
        
        return false;
    }

    std::vector<std::wstring> InstanceListWrapper::GetGroups() const
    {
        std::vector<std::wstring> groups;
        
        if (!m_instanceList)
            return groups;

        QStringList qGroups = m_instanceList->getGroups();
        for (const QString& group : qGroups)
        {
            groups.push_back(group.toStdWString());
        }
        
        return groups;
    }

    std::wstring InstanceListWrapper::GetInstanceGroup(const std::wstring& id) const
    {
        if (!m_instanceList)
            return L"";

        QString qId = QString::fromStdWString(id);
        QString group = m_instanceList->getInstanceGroup(qId);
        return group.toStdWString();
    }

    void InstanceListWrapper::SetInstanceGroup(const std::wstring& id, const std::wstring& group)
    {
        if (!m_instanceList)
            return;

        QString qId = QString::fromStdWString(id);
        QString qGroup = QString::fromStdWString(group);
        m_instanceList->setInstanceGroup(qId, qGroup);
    }

    void InstanceListWrapper::DeleteGroup(const std::wstring& group)
    {
        if (!m_instanceList)
            return;

        QString qGroup = QString::fromStdWString(group);
        m_instanceList->deleteGroup(qGroup);
    }

    void InstanceListWrapper::RenameGroup(const std::wstring& oldName, const std::wstring& newName)
    {
        if (!m_instanceList)
            return;

        QString qOldName = QString::fromStdWString(oldName);
        QString qNewName = QString::fromStdWString(newName);
        m_instanceList->renameGroup(qOldName, qNewName);
    }

    void InstanceListWrapper::SetInstanceListChangedCallback(InstanceListChangedCallback callback)
    {
        m_callback = callback;
    }
}