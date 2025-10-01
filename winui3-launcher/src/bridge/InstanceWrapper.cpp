#include "InstanceWrapper.h"
#include <QString>
#include <QDateTime>

namespace winrt::PrismLauncherWinUI3::implementation
{
    InstanceWrapper::InstanceWrapper(std::shared_ptr<BaseInstance> instance)
        : m_instance(instance)
    {
    }

    InstanceWrapper::~InstanceWrapper()
    {
    }

    std::wstring InstanceWrapper::GetId() const
    {
        if (!m_instance)
            return L"";
        
        return m_instance->id().toStdWString();
    }

    std::wstring InstanceWrapper::GetName() const
    {
        if (!m_instance)
            return L"";
        
        return m_instance->name().toStdWString();
    }

    std::wstring InstanceWrapper::GetVersion() const
    {
        if (!m_instance)
            return L"";
        
        return m_instance->instanceType().toStdWString();
    }

    std::wstring InstanceWrapper::GetLastPlayed() const
    {
        if (!m_instance)
            return L"";
        
        QDateTime lastPlayed = QDateTime::fromMSecsSinceEpoch(m_instance->lastTimePlayed());
        if (lastPlayed.isValid())
        {
            return lastPlayed.toString("yyyy-MM-dd hh:mm").toStdWString();
        }
        
        return L"从未";
    }

    bool InstanceWrapper::IsRunning() const
    {
        if (!m_instance)
            return false;
        
        return m_instance->isRunning();
    }

    bool InstanceWrapper::IsBroken() const
    {
        if (!m_instance)
            return false;
        
        return m_instance->hasVersionBroken();
    }

    bool InstanceWrapper::HasUpdate() const
    {
        if (!m_instance)
            return false;
        
        return m_instance->hasUpdateAvailable();
    }

    std::wstring InstanceWrapper::GetIconKey() const
    {
        if (!m_instance)
            return L"";
        
        return m_instance->iconKey().toStdWString();
    }

    std::wstring InstanceWrapper::GetGroup() const
    {
        // 这里需要从InstanceList获取分组信息
        // 简化实现
        return L"默认";
    }

    int64_t InstanceWrapper::GetTotalPlayTime() const
    {
        if (!m_instance)
            return 0;
        
        return m_instance->totalTimePlayed();
    }

    int64_t InstanceWrapper::GetLastLaunch() const
    {
        if (!m_instance)
            return 0;
        
        return m_instance->lastLaunch();
    }

    void InstanceWrapper::SetName(const std::wstring& name)
    {
        if (!m_instance)
            return;
        
        m_instance->setName(QString::fromStdWString(name));
    }

    void InstanceWrapper::SetIconKey(const std::wstring& iconKey)
    {
        if (!m_instance)
            return;
        
        m_instance->setIconKey(QString::fromStdWString(iconKey));
    }

    void InstanceWrapper::SetGroup(const std::wstring& group)
    {
        // 这里需要实现设置分组的逻辑
        // 简化实现
    }

    bool InstanceWrapper::Launch(bool online, bool demo)
    {
        if (!m_instance)
            return false;
        
        // 这里需要实现启动逻辑
        // 简化实现
        return true;
    }

    bool InstanceWrapper::Kill()
    {
        if (!m_instance)
            return false;
        
        // 这里需要实现停止逻辑
        // 简化实现
        return true;
    }

    bool InstanceWrapper::CanLaunch() const
    {
        if (!m_instance)
            return false;
        
        return m_instance->canLaunch();
    }

    bool InstanceWrapper::CanEdit() const
    {
        if (!m_instance)
            return false;
        
        return m_instance->canEdit();
    }

    bool InstanceWrapper::CanExport() const
    {
        if (!m_instance)
            return false;
        
        return m_instance->canExport();
    }
}